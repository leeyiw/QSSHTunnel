#-------------------------------------------------
#
# Project created by QtCreator 2014-11-15T15:24:21
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QSSHTunnel
TEMPLATE = app


SOURCES += main.cpp\
    MainWindow.cpp \
    MyTemporaryFile.cpp

HEADERS  += MainWindow.h \
    MyTemporaryFile.h

RESOURCES += \
    resource.qrc

ICON = images/QSSHTunnel.icns

TRANSLATIONS = translates/$${TARGET}_zh_CN.ts
