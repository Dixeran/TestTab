#ifndef HOSTWINDOW_H
#define HOSTWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QTabBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QList>
#include <QMap>
#include <QPushButton>
#include <Windows.h>
#include <processthreadsapi.h>
#include "widgetcontainer.h"
#include "windowwraper.h"

typedef struct HookPair{
    HWINEVENTHOOK titleHook;
//    HWINEVENTHOOK iconHook;
    HookPair(HWINEVENTHOOK _titleHook)
    {
        titleHook = _titleHook;
    }
} HookPair;

class HostWindow : public QMainWindow
{
    Q_OBJECT
    QVBoxLayout* verticalLayout; // mainLayout

    QHBoxLayout* tabLayout; // container of tabBar & addTabButton
    QTabBar* tabBar;
    QPushButton* addTabButton;
    QWidget* tabWidget; // wrapper for tabLayout

    WidgetContainer* tabContentContianer; // contain tab content

    QWidget* replaceContainer; // to replace origin widget

    QList<WindowWraper*> tabs; // opened tabs

    QMap<DWORD, HookPair> hooks;// process that set event hook

    //timer to get explorer window
    qint64 timer;

public:
    explicit HostWindow(QWidget *parent = nullptr);
    void closeEvent(QCloseEvent *event) override;
    void CatchWindow(HWND windowHandle, DWORD pId = 0);

    bool AddExplorer(); // functions to add a explorer

    void notifyTitleChanged(HWND changedWindow);
signals:

public slots:
};

typedef struct {
    HostWindow* callee;
    QList<DWORD> target;
    bool finded;
} EnumArg;

static HostWindow* hostInstance = nullptr;

#endif // HOSTWINDOW_H
