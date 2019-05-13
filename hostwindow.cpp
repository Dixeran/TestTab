#include "hostwindow.h"
#include <TlHelp32.h>
#include <QResizeEvent>
#include <QDateTime>
#include <tchar.h>
#include <QApplication>
#include <QDebug>
HostWindow::HostWindow(QWidget *parent) : QMainWindow(parent), timer(QDateTime::currentSecsSinceEpoch())
{
    this->resize(800, 400);
    // init components
    {
        replaceContainer = new QWidget(this);
        this->setCentralWidget(replaceContainer);// replace to set layout

        verticalLayout = new QVBoxLayout();
        verticalLayout->setMargin(0);
        verticalLayout->setSpacing(0);
        replaceContainer->setLayout(verticalLayout);

        // init tabBarLayout
        tabLayout = new QHBoxLayout();
        tabLayout->setSpacing(0);
        tabLayout->setMargin(0);

        // init tabBar
        tabBar = new QTabBar(this);
        tabBar->setTabsClosable(true);
//        tabBar->setMovable(true);
        tabBar->setStyleSheet("QTabBar::tab { "
                              "height: 40px;"
                              "}");
        tabBar->setExpanding(false);
        tabLayout->addWidget(tabBar);

        //init button
        addTabButton = new QPushButton(this);
        addTabButton->setText("+");
        addTabButton->setMinimumSize(QSize(40, 40));
        addTabButton->setMaximumSize(QSize(40, 40));
        tabLayout->addWidget(addTabButton);

        // place tabBars in widget
        tabWidget = new QWidget(this);
        tabWidget->setMaximumHeight(40);
        tabWidget->setMinimumHeight(40);
        tabWidget->setLayout(tabLayout);

        verticalLayout->addWidget(tabWidget);

        tabContentContianer = new WidgetContainer(this);
        verticalLayout->addWidget(tabContentContianer);
    }

    // tabBar resort event
    QObject::connect(
                tabBar,
                &QTabBar::tabMoved,
                [](int from, int to){
        qDebug() << from << to;
    });

    // tabBar close event
    QObject::connect(
                tabBar,
                &QTabBar::tabCloseRequested,
                [this](int index){
        // TODO: delete window wrapper
        qDebug() << "request close" << index;
        tabs.removeAt(index);
        delete tabContentContianer->DeleteWidget();
        tabBar->removeTab(index);

        if(tabs.length() == 0) QApplication::quit();
    });

    // select tab event
    QObject::connect(
                tabBar,
                &QTabBar::currentChanged,
                [this](int index){
        qDebug() << "set current tab" << index;
        if(index >= 0)
            tabContentContianer->SetWidget(tabs[index]);
    });

    // addTab event
    QObject::connect(
                addTabButton,
                &QPushButton::clicked,
                [this](){
        this->AddExplorer();
    });
}

void HostWindow::closeEvent(QCloseEvent *event)
{
    tabContentContianer->DeleteWidget();
    for(auto i = tabs.begin(); i != tabs.end(); i++)
    {
        delete (*i);
    }
    qDebug() << "destroy";
    event->accept();
}

void HostWindow::CatchWindow(HWND windowHandle)
{
    if(windowHandle == nullptr) exit(1);
    WindowWraper* newTab = new WindowWraper(windowHandle);
    tabs.push_back(newTab);
    tabBar->addTab("my computer");
    tabBar->setCurrentIndex(tabs.length() - 1);
}

BOOL CALLBACK EnumWindowsProcs(HWND hWnd,LPARAM lParam)
{
    EnumArg* arg = reinterpret_cast<EnumArg*>(lParam);
    LPCTSTR targetClassName = L"CabinetWClass\0";
    LPTSTR className = new wchar_t[14];
    GetClassName(hWnd, className, 14);
    bool isValid =
            GetParent(hWnd) == nullptr &&
            IsWindowVisible(hWnd) &&
            wcscmp(targetClassName, className) == 0;

    if(isValid){
        DWORD targetId = arg->target;
        DWORD pId;
        GetWindowThreadProcessId(hWnd, &pId);
        if(targetId == pId){
            qDebug() << "Get you!" << pId << "/" << hWnd;
            arg->callee->CatchWindow(hWnd);
            arg->finded = true;
            return false; // stop enum
        }
        return true;
    }
    return true; // continue
}

bool HostWindow::AddExplorer()
{
    // execute explorer
    SHELLEXECUTEINFO SEI = {0};
    SEI.cbSize = sizeof (SHELLEXECUTEINFO);
    SEI.fMask = SEE_MASK_NOCLOSEPROCESS;
    SEI.lpVerb = L"open";
    SEI.lpFile = L"explorer.exe";
    SEI.lpDirectory = nullptr;
    SEI.nShow = SW_SHOWDEFAULT;
    SEI.hInstApp = nullptr;

    if(ShellExecuteEx(&SEI)){
        DWORD targetId = NULL; // target process (explorer.exe) id
        PROCESSENTRY32 entry;
        entry.dwSize = sizeof (PROCESSENTRY32);

        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
        if(Process32First(snapshot, &entry)){
            while(Process32Next(snapshot, &entry)){
                qDebug() << entry.szExeFile;
                if(wcscmp(entry.szExeFile, L"explorer.exe") == 0){
                    targetId = entry.th32ProcessID;
                    break;
                }
            }
        }

        if(targetId == NULL){
            // shellExecute success
            // but not found target porcess
            return false;
        }

        EnumArg* arg = new EnumArg();
        arg->callee = this;
        arg->target = targetId;
        arg->finded = false;
        qDebug() << "exec explorer" << arg->target;

        timer = QDateTime::currentSecsSinceEpoch();
        while(QDateTime::currentSecsSinceEpoch() - timer < 5){
            // 5 seconds timeout
            if(!EnumWindows(&EnumWindowsProcs, reinterpret_cast<LPARAM>(arg))){
                DWORD error = GetLastError();
                if(error){
                    qDebug() << "enum window: result" << error;
                    break;
                }
                return true;
            }
        }
        // not found
        return false;
    }
    else{
        qDebug() << GetLastError();
        qDebug() << SEI.hInstApp;
        return false;
    }
}
