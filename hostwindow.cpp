﻿#include "hostwindow.h"
#include <TlHelp32.h>
#include <QResizeEvent>
#include <QDateTime>
#include <tchar.h>
#include <QApplication>
#include <QSettings>
#include <QFont>
#include <QDebug>
HostWindow::HostWindow(QWidget *parent) : QMainWindow(parent), timer(QDateTime::currentSecsSinceEpoch())
{
    hostInstance = this;

    // restore last pos/size
    QSettings setting("TabSoft", "TestTab");
    if(setting.contains("hostwindow/size")){
        qDebug() << "restore last size..";
        QSize lastSize = setting.value("hostwindow/size").toSize();
        this->resize(lastSize);
    }
    else
        this->resize(800, 400);

    if(setting.contains("hostwindow/pos")){
        QPoint lastPos = setting.value("hostwindow/pos").toPoint();
        this->move(lastPos);
    }

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
        tabBar = new MyTabBar(this);
        tabLayout->addWidget(tabBar);

        //init button
        addTabButton = new QPushButton(this);
        addTabButton->setIcon(QIcon(":/img/icons8-plus-math-40.png"));
        addTabButton->setIconSize(QSize(30, 30));
        addTabButton->setFlat(true);
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
        qDebug() << "request close" << index;
        if(index == tabBar->currentIndex())
            delete tabContentContianer->DeleteWidget();
        tabs.removeAt(index);
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

    //drag&drop event
    QObject::connect(
                tabBar,
                &MyTabBar::newTab,
                [this](QString path){
       this->AddExplorer(path);
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
    hostInstance = nullptr;
    tabContentContianer->DeleteWidget();
    for(auto i = tabs.begin(); i != tabs.end(); i++)
    {
        delete (*i);
    }

    for (auto i = hooks.begin(); i != hooks.end(); i++) {
        UnhookWinEvent((*i).titleHook);
    }

    qDebug() << "destroy";

    if(!isMinimized() && !isMaximized()){
        // save pos/size
        QSettings setting("TabSoft", "TestTab");
        setting.setValue("hostwindow/size", this->size());
        setting.setValue("hostwindow/pos", this->pos());
    }

    event->accept();
}

void CALLBACK HandleWinEventName(HWINEVENTHOOK hook, DWORD event, HWND hwnd,
                             LONG idObject, LONG idChild,
                             DWORD dwEventThread, DWORD dwmsEventTime)
{
    if(hostInstance == nullptr || hwnd == nullptr) return;
    hostInstance->notifyTitleChanged(hwnd);
}

void HostWindow::CatchWindow(HWND windowHandle, DWORD pId)
{
    if(windowHandle == nullptr) exit(1);
    WindowWraper* newTab = new WindowWraper(windowHandle);
    tabs.push_back(newTab);
    // get window Title
    int titleLength = GetWindowTextLength(windowHandle) + 1; // including "\0"
    wchar_t* title = new wchar_t[titleLength];
    GetWindowText(windowHandle, title, titleLength);
    QString titleQ = QString::fromWCharArray(title, titleLength);
    delete[] title;
    title = nullptr;
    tabBar->addTab(titleQ);
    tabBar->setCurrentIndex(tabs.length() - 1);

    // TODO: hook window icon event
    if(pId != 0 && !hooks.contains(pId)){
        qDebug() << "try to hook title..";
        HWINEVENTHOOK titlehook = SetWinEventHook(
                    EVENT_OBJECT_NAMECHANGE,
                    EVENT_OBJECT_NAMECHANGE,
                    nullptr,
                    HandleWinEventName,
                    pId,
                    0,
                    WINEVENT_OUTOFCONTEXT);
        hooks.insert(pId, HookPair(titlehook));
    }
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
        DWORD pId;
        GetWindowThreadProcessId(hWnd, &pId);
        if(arg->target.contains(pId)){
            qDebug() << "Get you!" << pId << "/" << hWnd;
            arg->callee->CatchWindow(hWnd, pId);
            arg->finded = true;
            return false; // stop enum
        }
        return true;
    }
    return true; // continue
}

bool HostWindow::AddExplorer(QString path)
{
    // execute explorer
    SHELLEXECUTEINFO SEI = {0};
    SEI.cbSize = sizeof (SHELLEXECUTEINFO);
    SEI.fMask = SEE_MASK_NOCLOSEPROCESS;
    SEI.lpVerb = NULL;
    SEI.lpFile = L"explorer.exe"; // Open an Explorer window at the 'Computer'
    SEI.lpParameters = L","; // default path
    if(path != ""){
        path = path.right(path.length() - 8); // cut prefix file:///
        path.replace("/", "\\");
        path.push_front("/root,");
        wchar_t* path_w = new wchar_t[path.length() + 1];
        path.toWCharArray(path_w);
        path_w[path.length()] = L'\0';
        SEI.lpParameters = path_w;
    }


    SEI.lpDirectory = nullptr;
    SEI.nShow = SW_MINIMIZE; // don't popup
    SEI.hInstApp = nullptr;

    if(ShellExecuteEx(&SEI)){
        EnumArg* arg = new EnumArg();
        arg->callee = this;
        arg->finded = false;
        PROCESSENTRY32 entry;
        entry.dwSize = sizeof (PROCESSENTRY32);

        timer = QDateTime::currentSecsSinceEpoch();
        while(QDateTime::currentSecsSinceEpoch() - timer < 5){
            arg->target.clear();
            HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
            if(Process32First(snapshot, &entry)){
                while(Process32Next(snapshot, &entry)){
                    if(wcscmp(entry.szExeFile, L"explorer.exe") == 0){
                        arg->target.push_back(entry.th32ProcessID);
                    }
                }
            }

            if(arg->target.length() == 0){
                // shellExecute success
                // but not found target porcess
                return false;
            }

            qDebug() << "exec explorer" << arg->target;

            // 5 seconds timeout
            if(!EnumWindows(&EnumWindowsProcs, reinterpret_cast<LPARAM>(arg))){
                delete arg;
                arg = nullptr;
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

void HostWindow::notifyTitleChanged(HWND changedWindow)
{
    qDebug() << "get notified title changed " << changedWindow;
    for (auto i = 0; i < tabs.length(); i++) {
        if(tabs[i]->GetWindowHandle() == changedWindow){
            int titleLength = GetWindowTextLength(changedWindow) + 1; // including "\0"
            wchar_t* title = new wchar_t[titleLength];
            GetWindowText(changedWindow, title, titleLength);
            QString titleQ = QString::fromWCharArray(title, titleLength);
            delete[] title;
            title = nullptr;

            tabBar->setTabText(i, titleQ);
        }
    }
}
