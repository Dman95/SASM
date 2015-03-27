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

#include "tab.h"

Tab::Tab(QWidget *parent) :
    QMainWindow(parent)
{
    //Setting code field
    code = new CodeEditor;
    setCentralWidget(code);

    //Setting input and output fields
    input = new RuQTextEdit;
    output = new RuQTextEdit;
    output->setReadOnly(true);
    inputLayout = new QVBoxLayout;
    outputLayout = new QVBoxLayout;
    inputLayout->addWidget(input);
    outputLayout->addWidget(output);

    setDockOptions(QMainWindow::AllowNestedDocks | QMainWindow::AnimatedDocks);

    QDockWidget *inputDock = new QDockWidget(tr("Input"), this);
    inputDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    inputWidget = new QWidget(inputDock);
    inputWidget->setLayout(inputLayout);
    inputDock->setWidget(inputWidget);
    inputDock->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
    addDockWidget(Qt::RightDockWidgetArea, inputDock);
    inputDock->setObjectName("inputDock");

    QDockWidget *outputDock = new QDockWidget(tr("Output"), this);
    outputDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    outputWidget = new QWidget(outputDock);
    outputWidget->setLayout(outputLayout);
    outputDock->setWidget(outputWidget);
    outputDock->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
    addDockWidget(Qt::RightDockWidgetArea, outputDock);
    outputDock->setObjectName("outputDock");

    //Setting io and code fonts
    setFonts();

    //restore state
    QSettings settings("SASM Project", "SASM");
    restoreGeometry(settings.value("tabgeometry").toByteArray());
    restoreState(settings.value("tabwindowstate").toByteArray());
}

void Tab::setFonts()
{
    QSettings settings("SASM Project", "SASM");

    QFont codeFont;
    codeFont.setPointSize(settings.value("fontsize", 12).toInt());
    codeFont.setFamily(settings.value("fontfamily", QString("Courier")).value<QString>());
    codeFont.setStyleHint(QFont::Monospace);
    code->setFont(codeFont);
    code->repaintLineNumberArea();

    QFont logFont;
    logFont.setPointSize(settings.value("fontsize", 12).toInt());
    input->setFont(logFont);
    output->setFont(logFont);
}

QTextDocument * Tab::getCodeDocument()
{
    return code->document();
}

QString Tab::getCurrentFilePath()
{
    return currentFilePath;
}

void Tab::saveCodeToFile(const QString &filePath, Assembler *assembler, bool changeCodeModifiedFlag, bool debugMode)
{
    QFile outfile;
    outfile.setFileName(filePath);
    outfile.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&outfile);
    if (debugMode) {
        assembler->putDebugString(code);
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
    msg = output->toPlainText().right(2000) + msg.right(2000);
    msg = msg.right(2000);
    output->setText(msg);
    //scroll
    QTextCursor cursor = output->textCursor();
    cursor.movePosition(QTextCursor::End);
    output->setTextCursor(cursor);
}

void Tab::clearOutput()
{
    output->clear();
}

Tab::~Tab()
{
    delete input;
    delete output;

    delete inputLayout;
    delete outputLayout;

    delete code;

    delete inputWidget;
    delete outputWidget;
}
