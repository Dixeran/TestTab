#include "hostwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    HostWindow mw;
    mw.show();
    mw.AddExplorer(); // assume there is no existing explorer
    return a.exec();
}
