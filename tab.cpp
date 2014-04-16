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

    //restore state
    restoreGeometry(settings.value("tabgeometry").toByteArray());
    restoreState(settings.value("tabwindowstate").toByteArray());
}

QTextDocument * Tab::getCodeDocument()
{
    return code->document();
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
            QSettings settings("SASM Project", "SASM");
            QString assembler = settings.value("assembler", QString("NASM")).toString();
            if (assembler == "NASM")
                if (settings.value("mode", QString("x86")).toString() == "x86") {
                    if (code->toPlainText().indexOf(
                                QRegExp("\\s+[Mm][Oo][Vv] +[Ee][Bb][Pp] *, *[Ee][Ss][Pp]"), index + 1) != index + 1) {
                        QTextCursor cursor = code->textCursor();
                        cursor.movePosition(QTextCursor::Start);
                        cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, index + 1);
                        cursor.insertText(QString("\n    mov ebp, esp; for correct debugging"));
                    }
                } else {
                    if (code->toPlainText().indexOf(
                                QRegExp("\\s+[Mm][Oo][Vv] +[Rr][Bb][Pp] *, *[Rr][Ss][Pp]"), index + 1) != index + 1) {
                        QTextCursor cursor = code->textCursor();
                        cursor.movePosition(QTextCursor::Start);
                        cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, index + 1);
                        cursor.insertText(QString("\n    mov rbp, rsp; for correct debugging"));
                    }
                }
            else if (assembler == "GAS") {
                int intelIndex = code->toPlainText().lastIndexOf("intel_syntax", index + 1);
                int attIndex = code->toPlainText().lastIndexOf("att_syntax", index + 1);
                if (intelIndex == -1 || attIndex > intelIndex) { //AT&T syntax
                    if (settings.value("mode", QString("x86")).toString() == "x86") {
                        if (code->toPlainText().indexOf(
                                    QRegExp("\\s+[Mm][Oo][Vv][Ll] +%?[Ee][Ss][Pp] *, *%?[Ee][Bb][Pp]"), index + 1) != index + 1) {
                            QTextCursor cursor = code->textCursor();
                            cursor.movePosition(QTextCursor::Start);
                            cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, index + 1);
                            cursor.insertText(QString("\n    movl %esp, %ebp #for correct debugging"));
                        }
                    } else {
                        if (code->toPlainText().indexOf(
                                    QRegExp("\\s+[Mm][Oo][Vv][Qq] +%?[Rr][Ss][Pp] *, *%?[Rr][Bb][Pp]"), index + 1) != index + 1) {
                            QTextCursor cursor = code->textCursor();
                            cursor.movePosition(QTextCursor::Start);
                            cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, index + 1);
                            cursor.insertText(QString("\n    movq %rsp, %rbp #for correct debugging"));
                        }
                    }
                } else { //Intel syntax
                    if (settings.value("mode", QString("x86")).toString() == "x86") {
                        if (code->toPlainText().indexOf(
                                    QRegExp("\\s+[Mm][Oo][Vv] +%?[Ee][Bb][Pp] *, *%?[Ee][Ss][Pp]"), index + 1) != index + 1) {
                            QTextCursor cursor = code->textCursor();
                            cursor.movePosition(QTextCursor::Start);
                            cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, index + 1);
                            cursor.insertText(QString("\n    mov %ebp, %esp #for correct debugging"));
                        }
                    } else {
                        if (code->toPlainText().indexOf(
                                    QRegExp("\\s+[Mm][Oo][Vv] +%?[Rr][Bb][Pp] *, *%?[Rr][Ss][Pp]"), index + 1) != index + 1) {
                            QTextCursor cursor = code->textCursor();
                            cursor.movePosition(QTextCursor::Start);
                            cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, index + 1);
                            cursor.insertText(QString("\n    mov %rbp, %rsp #for correct debugging"));
                        }
                    }
                }
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
