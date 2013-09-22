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

#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <QProcess>
#include <QString>
#include <QTextStream>
#include <QTextEdit>
#include <QFile>
#include <QVector>
#include <QCoreApplication>
#include <QTimer>
#include <QQueue>


enum DebugActionType {ni, si, infoRegisters, infoMemory, anyAction, none, breakpoint};

class Debugger : public QObject
{
    Q_OBJECT

public:
    Debugger(QTextEdit *tEdit, const QString &path, bool ioInc, QString tmp, QWidget *parent = 0);
    ~Debugger();
    struct registersInfo {
        QString name;
        QString hexValue;
        QString decValue;
    };

private:
    void processLst();
    void run();

    QProcess *process;
    QTextEdit *textEdit;
    unsigned int offset; //offset between program code in memory and in file
    unsigned int omitLinesCount; //for skipping strings from section .data - for processLst()

    struct lineNum {
        unsigned int numInCode;
        unsigned int numInMem;
    };
    QVector<lineNum> lines; //accordance between program lines in memory and in file

    int c; //counter for sequential performing of actions
    bool ioIncIncluded;
    unsigned int ioIncSize;

    QQueue<DebugActionType> actionTypeQueue; //queue of actions type from enum
    QString exitMessage; //message on exit in current platform
    QString cExitMessage; //message on exit which shows when "continue" command used
    QString tmpPath;

    QString buffer; //global gdb output buffer
    QTimer *bufferTimer; //timer for checking output and sending ready output to processing with Debugger::processOutput() function

public slots:
    void readOutputToBuffer();
    void processOutput();
    void processMessage(QString output);
    void processAction(QString output);
    void doInput(QString command, DebugActionType actionType);
    void changeBreakpoint(int lineNumber, bool isAdded);

signals:
    void highlightLine(int); //highlight current debug line
    void finished();
    void started(); //emited when debugger is ready to get commands like step into, etc.
    void printRegisters(Debugger::registersInfo*);
};

#endif // DEBUGGER_H
