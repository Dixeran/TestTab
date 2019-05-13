#ifndef WIDGETCONTAINER_H
#define WIDGETCONTAINER_H

#include <QWidget>
#include <QVBoxLayout>

class WidgetContainer : public QWidget
{
    Q_OBJECT
    QVBoxLayout* vLayout;
    QWidget* currentWidget;
public:
    explicit WidgetContainer(QWidget *parent = nullptr);
    void SetWidget(QWidget*);
    QWidget* DeleteWidget();
signals:

public slots:
};

#endif // WIDGETCONTAINER_H
