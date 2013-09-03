#-------------------------------------------------
#
# Project created by QtCreator 2013-03-22T02:19:57
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = sasm
TEMPLATE = app

binfile.files += sasm
binfile.path = /usr/bin/
data.files += Linux/*
data.path = /usr/share/sasm/
shortcutfiles.files += desktop/sasm.desktop
shortcutfiles.path = /usr/share/applications/
INSTALLS += binfile
INSTALLS += data
INSTALLS += shortcutfiles

SOURCES += main.cpp\
        mainwindow.cpp \
    tab.cpp \
    highlighter.cpp \
    codeeditor.cpp \
    debugger.cpp \
    memorydebugwindow.cpp \
    commanddebugwindow.cpp \
    finddialog.cpp \
    ruqtextedit.cpp \
    ruqplaintextedit.cpp \
    getstartedwidget.cpp

HEADERS  += mainwindow.h \
    tab.h \
    highlighter.h \
    codeeditor.h \
    debugger.h \
    memorydebugwindow.h \
    commanddebugwindow.h \
    finddialog.h \
    ruqtextedit.h \
    ruqplaintextedit.h \
    getstartedwidget.h

FORMS += settings.ui

RESOURCES += \
    Languages.qrc \
    Images.qrc \
    Help.qrc

RC_FILE = icon.rc

CODECFORTR      = UTF-8
TRANSLATIONS    += language_ru.ts
