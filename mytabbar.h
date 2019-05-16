#ifndef MYTABBAR_H
#define MYTABBAR_H
#include <QTabBar>
#include <QWidget>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>

class MyTabBar : public QTabBar
{
    Q_OBJECT
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

public:
    explicit MyTabBar(QWidget *parent = nullptr);

signals:
    void newTab(QString path);
};

#endif // MYTABBAR_H
