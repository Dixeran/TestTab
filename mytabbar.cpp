#include "mytabbar.h"
#include <QFont>
#include <QDebug>

MyTabBar::MyTabBar(QWidget *parent) : QTabBar (parent)
{
    setTabsClosable(true);
    setStyleSheet("QTabBar::tab { "
                                  "height: 40px;"
                                  "}");
    setExpanding(false);
    QFont font;
    font.setFamily("Microsoft Yahei");
    setFont(font);

    setAcceptDrops(true);
}

void MyTabBar::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void MyTabBar::dropEvent(QDropEvent *event)
{
    qDebug() << "new tab: " << event->mimeData()->text();
    emit newTab(event->mimeData()->text());
}
