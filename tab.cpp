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

#include "tab.h"

Tab::Tab(QWidget *parent) :
    QSplitter(parent)
{
    //Setting code field
    code = new CodeEditor;
    connect(code, SIGNAL(textChanged()), this, SIGNAL(codeChanged()));

    //Setting input and output fields
    input = new RuQTextEdit;
    output = new RuQTextEdit;
    output->setReadOnly(true);
    inputLayout = new QVBoxLayout;
    outputLayout = new QVBoxLayout;
    outputLabel = new QLabel;
    inputLabel = new QLabel;
    outputLabel->setText(tr("Output:"));
    inputLabel->setText(tr("Input:"));
    inputLabel->setStyleSheet("font-size: 13pt; font-weight: normal;");
    outputLabel->setStyleSheet("font-size: 13pt; font-weight: normal;");
    inputLayout->addWidget(inputLabel);
    inputLayout->addWidget(input);
    outputLayout->addWidget(outputLabel);
    outputLayout->addWidget(output);

    //Setting buttons
    createButtons();
    inputLayout->addLayout(inputButtonsLayout);
    outputLayout->addLayout(outputButtonsLayout);
    connect(buildButton, SIGNAL(clicked()), this, SIGNAL(buildClick()));
    buildButton->setToolTip(QString("Ctrl+F9"));
    connect(runButton, SIGNAL(clicked()), this, SIGNAL(runClick()));
    runButton->setToolTip(QString("F9"));
    connect(stopButton, SIGNAL(clicked()), this, SIGNAL(stopClick()));
    connect(saveAsmButton, SIGNAL(clicked()), this, SIGNAL(saveAsmClick()));
    connect(saveExeButton, SIGNAL(clicked()), this, SIGNAL(saveExeClick()));

    //Place buttons and fields on form
    inputWidget = new QWidget;
    outputWidget = new QWidget;
    inputWidget->setLayout(inputLayout);
    outputWidget->setLayout(outputLayout);

    //Place main widgets according to settings
    QSettings settings("SASM Project", "SASM");
    //position of code field: 0 - left, 1 - center, 2 - right
    int codePosition = settings.value("codeposition", 0).toInt();
    switch (codePosition) {
        case 0:
            this->addWidget(code);
            this->addWidget(inputWidget);
            this->addWidget(outputWidget);
            break;
        case 1:
            this->addWidget(inputWidget);
            this->addWidget(code);
            this->addWidget(outputWidget);
            break;
        case 2:
            this->addWidget(inputWidget);
            this->addWidget(outputWidget);
            this->addWidget(code);
            break;
    }

    //Setting widget's proportions
    QSizePolicy size;
    size.setHorizontalPolicy(QSizePolicy::Expanding);
    size.setVerticalPolicy(QSizePolicy::Expanding);
    size.setHorizontalStretch(2);
    code->setSizePolicy(size);
    size.setHorizontalStretch(1);
    inputWidget->setSizePolicy(size);
    outputWidget->setSizePolicy(size);

    //Setting io and code fonts
    setFonts();

    connect(code, SIGNAL(undoAvailable(bool)), this, SLOT(setCodeModified()));
    setCodeModified(false);
}

void Tab::setFonts()
{
    QFont codeFont;
    codeFont.setPointSize(12);
    codeFont.setFamily("Courier");
    codeFont.setFixedPitch(true);
    code->setFont(codeFont);

    input->setFontPointSize(12);
    output->setFontPointSize(12);
}

void Tab::createButtons()
{
    QSizePolicy size;
    size.setHorizontalPolicy(QSizePolicy::Fixed);

    inputButtonsLayout = new QHBoxLayout;
    buildButton = new QPushButton;
    buildButton->setText(tr("Build"));
    buildButton->setSizePolicy(size);
    saveAsmButton = new QPushButton;
    saveAsmButton->setText(tr("Save .asm"));
    saveAsmButton->setSizePolicy(size);
    saveExeButton = new QPushButton;
    saveExeButton->setText(tr("Save .exe"));
    saveExeButton->setSizePolicy(size);
    inputButtonsLayout->addWidget(saveAsmButton);
    inputButtonsLayout->addWidget(buildButton);
    inputButtonsLayout->addWidget(saveExeButton);

    outputButtonsLayout = new QHBoxLayout;
    runButton = new QPushButton;
    runButton->setText(tr("Run"));
    runButton->setSizePolicy(size);
    stopButton = new QPushButton;
    stopButton->setText(tr("Stop"));
    stopButton->setSizePolicy(size);
    outputButtonsLayout->addWidget(runButton);
    outputButtonsLayout->addWidget(stopButton);
}

QTextDocument * Tab::getCodeDocument()
{
    return this->code->document();
}

QString Tab::getCurrentFilePath()
{
    return currentFilePath;
}

void Tab::saveCodeToFile(const QString &filePath, bool changeCodeModifiedFlag, bool debugMode)
{
    QFile outfile;
    outfile.setFileName(filePath);
    outfile.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&outfile);
    if (debugMode)
        out << "sasmStartL:\n";
    out << code->toPlainText();
    if (changeCodeModifiedFlag) {
        currentFilePath = filePath;
        setCodeModified(false);
    }
}

bool Tab::isIoIncIncluded()
{
    if (code->toPlainText().indexOf(QString("io.inc")) == -1)
        return false;
    return true;
}

void Tab::saveInputToFile(const QString &filePath)
{
    QFile outfile;
    outfile.setFileName(filePath);
    outfile.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&outfile);
    out << input->toPlainText();
}

void Tab::loadOutputFromFile(const QString &filePath)
{
    QFile outputFile;
    outputFile.setFileName(filePath);
    outputFile.open(QIODevice::ReadOnly);
    QTextStream programOut(&outputFile);
    QString out = programOut.readAll();
    output->setPlainText(out);
}

void Tab::loadCodeFromFile(const QString &filePath)
{
    QFile file;
    file.setFileName(filePath);
    file.open(QIODevice::ReadOnly);
    QTextStream text(&file);
    QString source = text.readAll();
    code->setPlainText(source);
    currentFilePath = filePath;
    setCodeModified(false);
}

void Tab::setCodeModified(bool value)
{
    //true by default
    codeModified = value;
}

bool Tab::isCodeModified()
{
    return codeModified;
}

void Tab::highlightDebugLine(int num)
{
    code->updateDebugLine(num);
}

void Tab::clearOutput()
{
    output->clear();
}

Tab::~Tab()
{
    delete inputButtonsLayout;
    delete outputButtonsLayout;
    delete buildButton;
    delete saveAsmButton;
    delete saveExeButton;
    delete runButton;
    delete stopButton;

    delete inputLabel;
    delete outputLabel;
    delete input;
    delete output;

    delete inputLayout;
    delete outputLayout;

    delete code;

    delete inputWidget;
    delete outputWidget;
}