#-------------------------------------------------
#
# Project created by QtCreator 2018-04-14T14:55:53
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets serialport

TARGET = app-ioctrl
TEMPLATE = app

RC_FILE += qrc/appsource.rc

HEADERS += \
    app/appioctrl.h

SOURCES += \
    app/appioctrl.cpp \
    app/main.cpp

RESOURCES += \
    qrc/appsource.qrc

