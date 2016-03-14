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
#include <QObject>
#include "singleapplication.h"

/**
 * @file main.cpp
 *
 */

int main(int argc, char *argv[])
{
//    QApplication a(argc, argv);
    QApplication::setApplicationName("SASM");
    QApplication::setOrganizationName("YourOrganizationName");

    SingleApplication a(argc, argv);

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
        msgBox.setWindowTitle(QString("Choose language"));
        msgBox.setText(QString("Choose language / Выберите язык"));
        msgBox.exec();

        if (msgBox.clickedButton() == rusButton) {
            settings.setValue("language", 0);
        } else if (msgBox.clickedButton() == engButton) {
            settings.setValue("language", 1);
        }
    }
    if (settings.value("language", 0).toInt() == 0) { //russian language
        translator.load(":/translations/language_ru.qm");
        a.installTranslator(&translator);

        qtTranslator.load(":/translations/qt_ru.qm");
        a.installTranslator(&qtTranslator);
    }

    MainWindow w(a.arguments());

    w.show();
    w.activateWindow();
    return a.exec();
}
