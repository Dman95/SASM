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

#ifndef TAB_H
#define TAB_H

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSettings>
#include <QTextStream>
#include <QFile>
#include <QMainWindow>
#include <QDockWidget>
#include "codeeditor.h"
#include "ruqtextedit.h"
#include "assembler.h"

/**
 * @file tab.h
 * Defines the Tab class.
 */

/*! \brief The Tab class represents tab in window.
*/

class Tab : public QMainWindow
{
    Q_OBJECT

public:
    explicit Tab(QWidget *parent = 0);
    virtual ~Tab();

    QTextDocument *getCodeDocument();
    void saveCodeToFile(const QString &filePath, Assembler *assembler, bool changeCodeModifiedFlag = true, bool debugMode = false);
    void saveInputToFile(const QString &filePath);
    void loadOutputFromFile(const QString &filePath);
    void loadCodeFromFile(const QString &filePath);
    void appendOutput(QString msg);
    QString getCurrentFilePath();
    void clearOutput();
    void setFonts();

    CodeEditor *code;


private: 
    //! Text fields
    QVBoxLayout *inputLayout;
    QVBoxLayout *outputLayout;
    RuQTextEdit *input;
    RuQTextEdit *output;
    QWidget *inputWidget;
    QWidget *outputWidget;

    QString currentFilePath;
};

#endif // TAB_H
