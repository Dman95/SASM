/****************************************************************************
** SASM - simple IDE for assembler development
** Copyright (C) 2013 Dmitriy Manushin
** Contact: site: http://dman95.github.io/SASM/
**          e-mail: Dman1095@gmail.com
**
** This file is part of SASM.
**
** SASM is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** SASM is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with SASM.  If not, see <http://www.gnu.org/licenses/>.
**
** Этот файл — часть SASM.
**
** SASM - свободная программа: вы можете перераспространять ее и/или
** изменять ее на условиях Стандартной общественной лицензии GNU в том виде,
** в каком она была опубликована Фондом свободного программного обеспечения;
** либо версии 3 лицензии, либо (по вашему выбору) любой более поздней
** версии.
**
** SASM распространяется в надежде, что она будет полезной,
** но БЕЗО ВСЯКИХ ГАРАНТИЙ; даже без неявной гарантии ТОВАРНОГО ВИДА
** или ПРИГОДНОСТИ ДЛЯ ОПРЕДЕЛЕННЫХ ЦЕЛЕЙ. Подробнее см. в Стандартной
** общественной лицензии GNU.
**
** Вы должны были получить копию Стандартной общественной лицензии GNU
** вместе с этой программой. Если это не так, см.
** <http://www.gnu.org/licenses/>.)
**
****************************************************************************/

#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QTextCodec>
#include <QSettings>
#include <QPushButton>
#include <QtSingleApplication>

/**
 * @file main.cpp
 *
 */

int main(int argc, char *argv[])
{
    QtSingleApplication a(argc, argv);
    if (a.isRunning()) {
        //we already have one instance of our application
        if (argc <= 1) {
            a.sendMessage(QString("activate"));
        } else {
            for (int i = 1; i < argc; ++i) {
                a.sendMessage(QString("run") + QString(argv[i]));
            }
        }
        return 0;
    }
    QTranslator translator, qtTranslator;
    QSettings settings("SASM Project", "SASM");
    #if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
        QTextCodec *codec = QTextCodec::codecForName("UTF-8");
        QTextCodec::setCodecForCStrings(codec);
    #endif
    if (! settings.contains("language")) { //language choosing
        QMessageBox msgBox;
        QPushButton *rusButton = msgBox.addButton(QString("Русский"), QMessageBox::NoRole);
        QPushButton *engButton = msgBox.addButton(QString("English"), QMessageBox::NoRole);
        QPushButton *turButton = msgBox.addButton(QString("Türk"), QMessageBox::NoRole);
        QPushButton *chiButton = msgBox.addButton(QString("中国"), QMessageBox::NoRole);
        QPushButton *gerButton = msgBox.addButton(QString("Deutsche"), QMessageBox::NoRole);
        QPushButton *itaButton = msgBox.addButton(QString("Italiano"), QMessageBox::NoRole);
        msgBox.setWindowTitle(QString("Choose language"));
        msgBox.setText(QString("Choose language / Выберите язык / Dil seçin / 选择你的语言 / Sprache auswählen / Scegli lingua"));
        msgBox.exec();

        if (msgBox.clickedButton() == rusButton) {
            settings.setValue("language", 0);
        } else if (msgBox.clickedButton() == engButton) {
            settings.setValue("language", 1);
        } else if (msgBox.clickedButton() == turButton) {
            settings.setValue("language", 2);
        } else if (msgBox.clickedButton() == chiButton) {
            settings.setValue("language", 3);
        } else if (msgBox.clickedButton() == gerButton) {
            settings.setValue("language", 4);
        } else if (msgBox.clickedButton() == itaButton) {
            settings.setValue("language", 5);
        }
    }
    if (settings.value("language", 0).toInt() == 0) { //russian language
        translator.load(":/translations/language_ru.qm");
        a.installTranslator(&translator);

        qtTranslator.load(":/translations/qt_ru.qm");
        a.installTranslator(&qtTranslator);
    } else if (settings.value("language", 0).toInt() == 2) { //turkish language
        translator.load(":/translations/language_tr.qm");
        a.installTranslator(&translator);

        qtTranslator.load(":/translations/qt_tr.qm");
        a.installTranslator(&qtTranslator);
    } else if (settings.value("language", 0).toInt() == 3) { //chinese language
        translator.load(":/translations/language_zh.qm");
        a.installTranslator(&translator);

        qtTranslator.load(":/translations/qt_zh.qm");
        a.installTranslator(&qtTranslator);
    } else if (settings.value("language", 0).toInt() == 4) { //german language
        translator.load(":/translations/language_de.qm");
        a.installTranslator(&translator);

        qtTranslator.load(":/translations/qt_de.qm");
        a.installTranslator(&qtTranslator);
    } else if (settings.value("language", 0).toInt() == 5) { //italian language
        translator.load(":/translations/language_it.qm");
        a.installTranslator(&translator);

        qtTranslator.load(":/translations/qt_it.qm");
        a.installTranslator(&qtTranslator);
    }

    MainWindow w(a.arguments());

    QObject::connect(&a, SIGNAL(messageReceived(const QString&)), &w, SLOT(onMessageReceived(const QString&)));

    w.show();
    w.activateWindow();
    return a.exec();
}
