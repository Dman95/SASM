#-------------------------------------------------
#
# Project created by QtCreator 2013-03-22T02:19:57
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = sasm
TEMPLATE = app

isEmpty(PREFIX) {
 PREFIX = /usr
}

BINDIR = $$PREFIX/bin
DATADIR = $$PREFIX/share

binfile.files += sasm
binfile.path = $$BINDIR
data.files += Linux/share/sasm/*
data.path = $$DATADIR/sasm/
shortcutfiles.files += Linux/share/applications/sasm.desktop
shortcutfiles.path = $$DATADIR/applications/
docfiles.files += Linux/share/doc/sasm/changelog.gz
docfiles.files += Linux/share/doc/sasm/copyright
docfiles.path = $$DATADIR/doc/sasm/
INSTALLS += binfile
INSTALLS += data
INSTALLS += shortcutfiles
INSTALLS += docfiles

SOURCES += main.cpp\
        mainwindow.cpp \
    tab.cpp \
    highlighter.cpp \
    codeeditor.cpp \
    debugger.cpp \
    commanddebugwindow.cpp \
    finddialog.cpp \
    ruqtextedit.cpp \
    ruqplaintextedit.cpp \
    getstartedwidget.cpp \
    debugtablewidget.cpp \
    watchsettinswidget.cpp

HEADERS  += mainwindow.h \
    tab.h \
    highlighter.h \
    codeeditor.h \
    debugger.h \
    commanddebugwindow.h \
    finddialog.h \
    ruqtextedit.h \
    ruqplaintextedit.h \
    getstartedwidget.h \
    debugtablewidget.h \
    watchsettinswidget.h

FORMS += settings.ui

RESOURCES += \
    Languages.qrc \
    Images.qrc \
    Help.qrc

RC_FILE = icon.rc

CODECFORTR      = UTF-8
TRANSLATIONS    += language_ru.ts
