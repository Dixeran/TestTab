#ifndef WINDOWWRAPER_H
#define WINDOWWRAPER_H

#include <QWidget>
#include <Windows.h>

class WindowWraper : public QWidget
{
    Q_OBJECT
    HWND currentWindow;
    void resizeEvent(QResizeEvent*) override;
public:
    WindowWraper(HWND windowHandle);
    ~WindowWraper() override;
    HWND GetWindowHandle();
signals:

public slots:
};

#endif // WINDOWWRAPER_H
