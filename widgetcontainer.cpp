#include "widgetcontainer.h"
#include <QDebug>

WidgetContainer::WidgetContainer(QWidget *parent) : QWidget(parent)
{
    currentWidget = nullptr;
    vLayout = new QVBoxLayout();
    vLayout->setMargin(0);
    vLayout->setSpacing(0);
    this->setLayout(vLayout);
}

void WidgetContainer::SetWidget(QWidget *widget)
{
    // there should be only one widget
    qDebug() << "set widget" << widget;
    if(currentWidget != nullptr){
        vLayout->removeWidget(currentWidget);
        currentWidget->hide();
    }
    vLayout->addWidget(widget);
    currentWidget = widget;
    widget->show();
}

QWidget* WidgetContainer::DeleteWidget()
{
    if(currentWidget != nullptr){
        vLayout->removeWidget(currentWidget);
        QWidget* temp = currentWidget;
        currentWidget = nullptr;
        return temp;
    }
    return nullptr;
}
