#-------------------------------------------------
#
# Project created by QtCreator 2019-05-10T18:08:36
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TestTab
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        hostwindow.cpp \
        main.cpp \
        mytabbar.cpp \
        widgetcontainer.cpp \
        windowwraper.cpp

HEADERS += \
        hostwindow.h \
        mytabbar.h \
        widgetcontainer.h \
        windowwraper.h

FORMS += \
        mainwindow.ui

win32: RC_ICONS += icon.ico

win32: LIBS += -lUser32

DISTFILES +=

win32: LIBS += -lkernel32

RESOURCES += \
    static.qrc
