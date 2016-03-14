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

#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <QProcess>
#include <QProcessEnvironment>
#include <QString>
#include <QTextStream>
#include <QTextEdit>
#include <QFile>
#include <QVector>
#include <QCoreApplication>
#include <QTimer>
#include <QQueue>
#include <QSettings>
#include <QFile>
#include "assembler.h"

#ifdef Q_OS_WIN32
    #include <Windows.h>
    #include <stddef.h>
    #include <stdlib.h>
#else
    #include <signal.h>
#endif

/**
 * @file debugger.h
 * Defines the debugger.
 */

enum DebugActionType {ni, si, showLine, infoRegisters, infoMemory, anyAction, none, breakpoint};


/**
 * ! \brief This class represents the debugger.
 *
 *
 * */
class Debugger : public QObject
{
    Q_OBJECT

public:
    Debugger(QTextEdit *tEdit, const QString &path, QString tmp, Assembler *assembler, QWidget *parent = 0);
    ~Debugger();
    void setWatchesCount(int count);

    struct registersInfo {
        QString name;
        QString hexValue;
        QString decValue;
    };

    struct memoryInfo {
        QString value;
        bool isValid;
    };

    typedef Assembler::LineNum LineNum;

    bool isStopped();
    void pause();

private:
    void processLst();
    void run();

    QProcess *process;
    QTextEdit *textEdit;

    //! Offset between program code in memory and in file
    quint64 offset;

    //! Accordance between program lines in memory and in file
    QVector<LineNum> lines;
    //! Counter for sequential performing of actions
    int c;
    bool registersOk;
    //! Queue of actions type from enum
    QQueue<DebugActionType> actionTypeQueue;

    //! Message on exit in current platform
    QString exitMessage;

    //! Message on exit which shows when "continue" command used
    QRegExp cExitMessage;

    QString tmpPath;

    //! Global gdb output buffer
    QString buffer;

    //! Global gdb error buffer
    QString errorBuffer;

    //! Timer for checking output and sending ready output to processing with Debugger::processOutput() function
    QTimer *bufferTimer;

    //! The number of variable watches
    int watchesCount;

    //! List of the variable watches
    QList<Debugger::memoryInfo> watches;

    //! UNKNOWN
    bool firstAction;

    //! UNKNOWN
    QList<LineNum> breakPairs;

    Assembler *assembler;

    bool stopped;
    quint64 pid;
    bool dbgSymbols;

    quint64 entryPoint;

public slots:
    void readOutputToBuffer();
    void processOutput();
    void processMessage(QString output, QString error);
    void processAction(QString output, QString error = QString());
    void doInput(QString command, DebugActionType actionType);
    void changeBreakpoint(quint64 lineNumber, bool isAdded);
    void emitStarted();

signals:
    //! Highlight the current debug line.
    void highlightLine(int);
    void finished();
    //! Emited when debugger is ready to get commands like step into, etc. -> Omitted or Emited?
    void started();
    void printRegisters(QList<Debugger::registersInfo>);
    void printMemory(QList<Debugger::memoryInfo>);
    void printLog(QString msg, QColor color = QColor(Qt::black));
    void printOutput(QString msg);
    void inMacro();
    void wasStopped();
    void needToContinue();
};

#endif // DEBUGGER_H
