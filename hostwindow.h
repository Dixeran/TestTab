#ifndef HOSTWINDOW_H
#define HOSTWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QTabBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QList>
#include <QPushButton>
#include <Windows.h>
#include <processthreadsapi.h>
#include "widgetcontainer.h"
#include "windowwraper.h"

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

    //timer to get explorer window
    qint64 timer;

public:
    explicit HostWindow(QWidget *parent = nullptr);
    void closeEvent(QCloseEvent *event) override;
    void CatchWindow(HWND windowHandle);

    // functions to add a explorer
    bool AddExplorer();
signals:

public slots:
};

typedef struct {
    HostWindow* callee;
    QList<DWORD> target;
    bool finded;
} EnumArg;

#endif // HOSTWINDOW_H
