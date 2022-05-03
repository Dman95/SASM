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
    bsd {
        PREFIX = /usr/local
    } else {
        PREFIX = /usr
    }
}

BINDIR = $$PREFIX/bin
DATADIR = $$PREFIX/share

unix {
    appdata.files = res/io.github.dman95.SASM.metainfo.xml
    appdata.path = $$DATADIR/metainfo
    desktop.files = res/io.github.dman95.SASM.desktop
    desktop.path = $$DATADIR/applications
    icons.files = res/app/icons/hicolor
    icons.path = $$DATADIR/icons

    binfile.files += sasm
    binfile.path = $$BINDIR
    data.files += Unix/share/sasm
    data.path = $$DATADIR
    docfiles.files += Unix/share/doc
    docfiles.path = $$DATADIR
}

INSTALLS += appdata
INSTALLS += desktop
INSTALLS += icons
INSTALLS += binfile
INSTALLS += data
INSTALLS += docfiles

include(singleapplication/qtsingleapplication.pri)

SOURCES += main.cpp\
        mainwindow.cpp \
    tab.cpp \
    highlighter.cpp \
    codeeditor.cpp \
    debugger.cpp \
    finddialog.cpp \
    ruqtextedit.cpp \
    ruqplaintextedit.cpp \
    getstartedwidget.cpp \
    debugtablewidget.cpp \
    watchsettingswidget.cpp \
    debuganycommandwidget.cpp \
    assembler.cpp \
    nasm.cpp \
    gas.cpp \
    common.cpp \
    fasm.cpp \
    signallocker.cpp \
    masm.cpp \
    gccbasedassembler.cpp

HEADERS  += mainwindow.h \
    tab.h \
    highlighter.h \
    codeeditor.h \
    debugger.h \
    finddialog.h \
    ruqtextedit.h \
    ruqplaintextedit.h \
    getstartedwidget.h \
    debugtablewidget.h \
    watchsettingswidget.h \
    debuganycommandwidget.h \
    assembler.h \
    nasm.h \
    gas.h \
    common.h \
    fasm.h \
    signallocker.h \
    masm.h \
    gccbasedassembler.h

FORMS += settings.ui

RESOURCES += \
    Languages.qrc \
    Images.qrc \
    Help.qrc

RC_FILE = icon.rc

CODECFORTR      = UTF-8
TRANSLATIONS    += language_ru.ts \
                   language_tr.ts \
                   language_zh.ts \
                   language_de.ts \
                   language_it.ts \
                   language_pl.ts \
                   language_he.ts \
                   language_es.ts \
                   language_pt.ts
