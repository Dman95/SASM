/****************************************************************************
** SASM - simple IDE for NASM development
** Copyright (C) 2013 Dmitriy Manushin
** Contact: site: https://sites.google.com/site/simpleasmproject/
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
#include <QLabel>
#include <QPushButton>
#include <QSplitter>
#include <QSettings>
#include <QTextStream>
#include "codeeditor.h"
#include "ruqtextedit.h"

class Tab : public QSplitter
{
    Q_OBJECT

public:
    explicit Tab(QWidget *parent = 0);
    virtual ~Tab();

    QTextDocument *getCodeDocument();
    void saveCodeToFile(const QString &filePath, bool changeCodeModifiedFlag = true, bool debugMode = false);
    void saveInputToFile(const QString &filePath);
    void loadOutputFromFile(const QString &filePath);
    void loadCodeFromFile(const QString &filePath);
    bool isCodeModified();
    QString getCurrentFilePath();
    bool isIoIncIncluded();
    void clearOutput();

    CodeEditor *code;

private: 
    //text fields
    QVBoxLayout *inputLayout;
    QVBoxLayout *outputLayout;
    QLabel *inputLabel;
    QLabel *outputLabel;
    RuQTextEdit *input;
    RuQTextEdit *output;
    QWidget *inputWidget;
    QWidget *outputWidget;

    //buttons
    QHBoxLayout *inputButtonsLayout;
    QHBoxLayout *outputButtonsLayout;
    QPushButton *buildButton;
    QPushButton *saveAsmButton;
    QPushButton *saveExeButton;
    QPushButton *runButton;
    QPushButton *stopButton;

    QString currentFilePath;
    bool codeModified;

    void setFonts();
    void createButtons();
    
signals:
    void buildClick();
    void buildProgram();
    void runClick();
    void stopClick();
    void saveAsmClick();
    void saveExeClick();
    void codeChanged();
    
public slots:
    void setCodeModified(bool value = true);
    void highlightDebugLine(int);   
};

#endif // TAB_H
