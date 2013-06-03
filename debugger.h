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

#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <QProcess>
#include <QString>
#include <QTextStream>
#include <QTextEdit>
#include <QFile>
#include <QVector>
#include <QCoreApplication>

enum DebugActionType {start, ni, si, infoRegisters, infoMemory, anyAction};

class Debugger : public QObject
{
    Q_OBJECT

public:
    Debugger(QTextEdit *tEdit, const QString &path, bool ioInc, QWidget *parent = 0);
    ~Debugger();

private:
    void processLst();
    void run();

    QProcess *process;
    QTextEdit *textEdit;
    unsigned int offset; //offset between program code in memory and in file
    unsigned int omitLinesCount; //for skipping strings from section .data - for processLst()

    struct lineNum {
        int numInCode;
        int numInMem;
    };
    QVector<lineNum> lines; //accordance between program lines in memory and in file

    int c; //counter for sequential performing of actions
    bool ioIncIncluded;
    int outputCount; //for processAction()
    int ioIncSize;

    DebugActionType actionType; //current action type from enum
    QString exitMessage; //message on exit in current platform

public slots:
    void processOutput();
    void processAction(QString output);
    void setActionType(DebugActionType);
    void doInput(QString command);

signals:
    void highlightLine(int); //highlight current debug line
    void finished();
};

#endif // DEBUGGER_H
