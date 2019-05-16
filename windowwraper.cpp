#include "windowwraper.h"
#include <QResizeEvent>
#include <QDebug>
WindowWraper::WindowWraper(HWND windowHandle)
{
    this->currentWindow = windowHandle;

    SetParent(windowHandle, (HWND)this->winId());
    LONG_PTR style = GetWindowLongPtr(windowHandle, GWL_STYLE);
    style = style & (~WS_BORDER) & (~WS_SYSMENU) & (~WS_CAPTION) & (~WS_SIZEBOX);
    SetWindowLongPtr(windowHandle, GWL_STYLE, style);

    MoveWindow(windowHandle, 0, 0, 800, 600, true); // must
}

WindowWraper::~WindowWraper()
{
    SetParent(this->currentWindow, nullptr);
    PostMessage(currentWindow, WM_CLOSE, 0, 0); // close window nicely
    qDebug() << "destroy wrapper";
}

void WindowWraper::resizeEvent(QResizeEvent *ev)
{
    MoveWindow(
                currentWindow,
                0,
                -40,
                ev->size().width(),
                ev->size().height() + 40,
                true
                );
}

HWND WindowWraper::GetWindowHandle()
{
    return currentWindow;
}
