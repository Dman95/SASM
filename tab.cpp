/****************************************************************************
** SASM - simple IDE for NASM development
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

#include "tab.h"

Tab::Tab(QWidget *parent) :
    QSplitter(parent)
{
    //Setting code field
    code = new CodeEditor;

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
}

void Tab::setFonts()
{
    QSettings settings("SASM Project", "SASM");

    QFont codeFont;
    codeFont.setPointSize(settings.value("fontsize", 12).toInt());
    codeFont.setFamily(settings.value("fontfamily", QString("Courier")).value<QString>());
    code->setFont(codeFont);
    code->repaintLineNumberArea();

    QFont logFont;
    logFont.setPointSize(settings.value("fontsize", 12).toInt());
    input->setFont(logFont);
    output->setFont(logFont);
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
    if (debugMode) {
        out << "sasmStartL:\n";
        //add : mov ebp, esp for making frame for correct debugging if this code has not been added yet
        int index;
        #ifdef Q_OS_WIN32
            index = code->toPlainText().indexOf(QRegExp("CMAIN:|_main:"));
        #else
            index = code->toPlainText().indexOf(QRegExp("CMAIN:|main:"));
        #endif
        if (index != -1) {
            index = code->toPlainText().indexOf(QChar(':'), index);
            if ( code->toPlainText().indexOf(QRegExp("\\s+mov +ebp *, *esp"), index + 1) != index + 1) {
                code->setPlainText(code->toPlainText().insert(index + 1, QString("\n    mov ebp, esp; for correct debugging")));
            }
        }
    }
    out << code->toPlainText();
    if (changeCodeModifiedFlag) {
        currentFilePath = filePath;
        code->document()->setModified(false);
    }
    outfile.close();
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
    code->document()->setModified(false);
    file.close();
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
    outfile.close();
}

void Tab::loadOutputFromFile(const QString &filePath)
{
    QFile outputFile;
    outputFile.setFileName(filePath);
    outputFile.open(QIODevice::ReadOnly);
    QTextStream programOut(&outputFile);
    QString out = programOut.readAll();
    output->setPlainText(out);
    outputFile.close();
}

void Tab::appendOutput(QString msg)
{
    output->append(msg);
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
