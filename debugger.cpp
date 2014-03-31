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

#include "debugger.h"

Debugger::Debugger(QTextEdit *tEdit, const QString &path, bool ioInc, QString tmp, QWidget *parent)
    : QObject(parent)
{
    QSettings settings("SASM Project", "SASM");
    omitLinesCount = 0;
    c = 0;
    firstAction = true;
    textEdit = tEdit;
    ioIncIncluded = ioInc;
    tmpPath = tmp;
    registersOk = true;
    //determine ioIncSize
    QString ioIncPath =  tmpPath + "io.inc";
    QFile ioIncFile(ioIncPath);
    ioIncFile.open(QFile::ReadOnly);
    QTextStream stream(&ioIncFile);
    ioIncSize = 0;
    while (!stream.atEnd()) {
        stream.readLine();
        ioIncSize++;
    }
    ioIncFile.close();
    #ifdef Q_OS_WIN32
        QString gdb;
        if (settings.value("mode", QString("x86")).toString() == "x86") {
            gdb = QCoreApplication::applicationDirPath() + "/NASM/MinGW/bin/gdb.exe";
            if (! QFile::exists(gdb))
                gdb = QCoreApplication::applicationDirPath() + "/Windows/NASM/MinGW/bin/gdb.exe";
            exitMessage = "mingw_CRTStartup";
        } else {
            gdb = QCoreApplication::applicationDirPath() + "/NASM/MinGW64/bin/gdb.exe";
            if (! QFile::exists(gdb))
                gdb = QCoreApplication::applicationDirPath() + "/Windows/NASM/MinGW64/bin/gdb.exe";
            exitMessage = "__fu0__set_invalid_parameter_handler";
        }
    #else
        QString gdb = "gdb";
        exitMessage = "libc_start_main";
    #endif
    cExitMessage = QRegExp("\\[Inferior .* exited");

    QStringList arguments;
    arguments << path;

    process = new QProcess;
    process->start(gdb, arguments);

    QObject::connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(readOutputToBuffer()));
    QObject::connect(process, SIGNAL(readyReadStandardError()), this, SLOT(readOutputToBuffer()));

    bufferTimer = new QTimer;
    QObject::connect(bufferTimer, SIGNAL(timeout()), this, SLOT(processOutput()), Qt::QueuedConnection);
    bufferTimer->start(10);
}

void Debugger::readOutputToBuffer()
{
    if (!process)
        return;
    errorBuffer += process->readAllStandardError();
    buffer += process->readAllStandardOutput();
}

void Debugger::processOutput()
{
    bufferTimer->stop();
    int index = buffer.indexOf(QString("(gdb)"));
    int linefeedIndex = errorBuffer.indexOf("\n");
    if (index != -1) { //if whole message ready to processing (end of whole message is "(gdb)")
        processMessage(buffer.left(index), errorBuffer.left(linefeedIndex));
        buffer.remove(0, index + 5); //remove processed message
        errorBuffer.remove(0, linefeedIndex + 1);
    }
    bufferTimer->start(10);
}

void Debugger::processMessage(QString output, QString error)
{
    if (c == 0) { //in start wait for printing of start gdb text like this:
        /*GNU gdb (GDB) 7.4
        Copyright (C) 2012 Free Software Foundation, Inc.
        License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
        This is free software: you are free to change and redistribute it.
        There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
        and "show warranty" for details.
        This GDB was configured as "i686-pc-mingw32".
        For bug reporting instructions, please see:
        <http://www.gnu.org/software/gdb/bugs/>...
        Reading symbols from C:\Users\Dmitri\Dropbox\Projects\SASMstatic\release\Program\SASMprog.exe...
        done.
        (gdb)*/

        doInput(QString("disas sasmStartL\n"), none);
        c++;
        return;
    }

    if (c == 1 && output != " ") {
        //next disassembly top part of code for setting according with program in memory and program in file
        /*Dump of assembler code for function sasmStartL:
           0x00401390 <+0>:	xor    %eax,%eax
           0x00401392 <+2>:	ret
           0x00401393 <+3>:	add    %dl,-0x77(%ebp)
        End of assembler dump.*/
        //we need first number (0x00401390)

        //count offset
        QRegExp r = QRegExp("0x[0-9a-fA-F]{8,16}");
        int index = r.indexIn(output);
        offset = output.mid(index, r.matchedLength()).toULongLong(0, 16);
        //take offset in hexadecimal representation (10 symbols) from string and convert it to int
        c++;
        processLst(); //count accordance
        run(); //perform Debugger::run(), that run program and open I/O files
        return;
    }

    //determine run of program
    //wait for message like this: Breakpoint 1, 0x00401390 in sasmStartL ()
    if (c == 2 && output.indexOf(QString(" in ")) != -1) {
        c++;
        actionTypeQueue.enqueue(ni);
        emit started(); //emit start signal
    }

    //if an error with the wrong name of the section has occurred
    if ((c == 2 && output.indexOf(QString("Make breakpoint pending on future shared library load")) != -1)
            || (c == 1 && output == " ")) {
        actionTypeQueue.enqueue(anyAction);
        processAction(tr("An error has occurred in the debugger. Please check the names of the sections."));
        QObject::disconnect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(readOutputToBuffer()));
        emit finished();
    }

    //process all actions after start
    if (c == 3)
        if (output.indexOf(QString("$1 =")) == -1) //input file
            processAction(output, error);
}

void Debugger::processAction(QString output, QString error)
{
    bool backtrace = (output.indexOf(QRegExp("#\\d+  0x[0-9a-fA-F]{8,16} in .* ()")) != -1);
    if ((output.indexOf(exitMessage) != -1 && !backtrace) ||
            output.indexOf(QRegExp(cExitMessage)) != -1) { //if debug finished
        //print output - message like bottom
            /*Breakpoint 1, 0x08048510 in sasmStartL ()
            "
            " Continuing.
            abccba123 //program output
            567 //program output
            [Inferior 1 (process 5693) exited normally]
            " */
        QRegExp continuingMsg("Continuing.\r?\n");
        QRegExp inferiorMsg("\\[Inferior ");
        int continuingIndex = continuingMsg.indexIn(output);
        int continuingLength = continuingMsg.matchedLength();
        int inferiorIndex = inferiorMsg.indexIn(output);
        if (continuingIndex != -1) {
            int outputLength = inferiorIndex - continuingIndex - continuingLength;
            if (outputLength > 0) {
                QString msg = output.mid(continuingIndex + continuingLength, outputLength); //program output
                emit printOutput(msg);
            }
        }

        //exit from debugging
        QObject::disconnect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(readOutputToBuffer()));
        emit finished();
        return;
    }

    if (actionTypeQueue.isEmpty()) {
        return;
    }
    DebugActionType actionType = actionTypeQueue.dequeue();

    int failIndex = output.indexOf(QString("Program received signal")); //program was completed incorrectly
    if (failIndex == -1) { //if program works normally
        if (actionType == breakpoint)
            return;

        if (actionType == si || actionType == ni || actionType == showLine) {
            //message is: line number + data
            //print line number and other data if si or ni and print line number only if showLine
            //scan line number in memory
            QRegExp r = QRegExp("0x[0-9a-fA-F]{8,16}");
            int index = r.indexIn(output);


            //print output
            if (index > 1 && !firstAction) {
                QString msg = output.left(index); //left part - probably ouput of program;
                msg.remove(0, 1); //remove first whitespace
                QRegExp continuingMsg("Continuing.\r?\n");
                QRegExp breakpointMsg("\r?\nBreakpoint \\d+, ");
                msg.remove(continuingMsg);
                msg.remove(breakpointMsg);
                emit printOutput(msg);
            }
            firstAction = false;

            quint64 lineNumber = output.mid(index, r.matchedLength()).toULongLong(0, 16);
            //take line number in hexadecimal representation
            //(10 symbols) in memory from string and convert it to int

            //find line number in accordance array and get number line in file with code
            bool found = false;
            for (int i = lines.count() - 1; i >= 0; i--) {
                if (lineNumber == lines[i].numInMem) {
                    lineNumber = lines[i].numInCode;
                    found = true;
                    break;
                }
            }

            if (!found) {
                //output = tr("Inside the macro or outside the program.") + '\n';
                emit inMacro();
                return;
            } else { //if found highlight and print it
                //highlight line number
                emit highlightLine(lineNumber);

                //print string number and all after it
                //output = QString::number(lineNumber) + tr(" line") + output.mid(output.indexOf("()") + 2);
                if (actionType == showLine)
                    return;
                output.remove(0, output.indexOf("()") + 2);
            }
        }

        if (actionType == anyAction) {
            if (output[output.length() - 1] != '\n')
                output += QChar('\n');
            //process as ni or si
            if (output.indexOf(QRegExp("0x[0-9a-fA-F]{8,16} in ")) != -1
                    && !backtrace) {
                actionTypeQueue.enqueue(showLine);
                processAction(output);
            }
            if (!error.isEmpty()) {
                if (output != " \n")
                    output = error + output;
                else
                    output = error;
            }
        }

        if (actionType == infoMemory) {
            bool isValid = false;
            if (output.indexOf(QString("No symbol")) == -1 &&
                    output.indexOf(QString("no debug info")) == -1 && output != QString(" ")) {
                //if variable exists (isValid = true)
                isValid = true;
                int index = output.indexOf(QRegExp("\\$\\d+ = .*"));
                if (index == -1)
                    isValid = false;
                else {
                    output = output.right(output.length() - index);
                    output = output.right(output.length() - output.indexOf(QChar('=')) - 1);
                    for (int i = output.size() - 1; i >= 0; i--) {
                        if (output[i].isSpace())
                            output.remove(i, 1);
                    }
                }
            }
            memoryInfo info;
            if (isValid)
                info.value = output;
            info.isValid = isValid;
            watches.append(info);
            if (watchesCount == watches.size()) {
                emit printMemory(watches);
                watches.clear();
            }
            return;
        }

        if (actionType == infoRegisters) {
            QTextStream registersStream(&output);
            QList<registersInfo> registers;
            registersInfo info;
            QSettings settings("SASM Project", "SASM");
            if (settings.value("mode", QString("x86")).toString() == "x86") {
                for (int i = 0; i < 16; i++) {
                    if (i == 9) {
                        registersStream >> info.name >> info.hexValue;
                        registersStream.skipWhiteSpace();
                        info.decValue = registersStream.readLine();
                    } else if (i == 8) {
                        registersStream >> info.name >> info.hexValue;
                        registersStream.skipWhiteSpace();
                        char c;
                        registersStream >> c;
                        while (c != ' ')
                            registersStream >> c;
                        info.decValue = registersStream.readLine();
                    } else {
                        registersStream >> info.name >> info.hexValue >> info.decValue;
                    }
                    registers.append(info);
                    if (i == 0 && info.name != "eax" && registersOk) {
                        doInput(QString("info registers\n"), infoRegisters);
                        registersOk = false;
                        return;
                    }
                }
            } else { //x64
                for (int i = 0; i < 24; i++) {
                    if (i == 17) {
                        registersStream >> info.name >> info.hexValue;
                        registersStream.skipWhiteSpace();
                        info.decValue = registersStream.readLine();
                    } else if (i == 16) {
                        registersStream >> info.name >> info.hexValue;
                        registersStream.skipWhiteSpace();
                        char c;
                        registersStream >> c;
                        while (c != ' ')
                            registersStream >> c;
                        info.decValue = registersStream.readLine();
                    } else {
                        registersStream >> info.name >> info.hexValue >> info.decValue;
                    }
                    registers.append(info);
                    if (i == 0 && info.name != "rax" && registersOk) {
                        doInput(QString("info registers\n"), infoRegisters);
                        registersOk = false;
                        return;
                    }
                }
            }
            emit printRegisters(registers);
            return;
        }

        if (output == QString("\r\n") || output == QString("\n") ||
                output == QString("\r\n\n") || output == QString("\n\n")) //if empty
            return;
    } else { //if program fail
        output = output.mid(failIndex);
    }

    //print information to log field
    emit printLog(output);

    if (failIndex != -1) {
        QObject::disconnect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(readOutputToBuffer()));
        emit finished();
        return;
    }
}

void Debugger::doInput(QString command, DebugActionType actionType)
{
    if (actionType != none)
        actionTypeQueue.enqueue(actionType);
    //put \n after commands!
    if (process)
        process->write(command.toLatin1());
}

void Debugger::setWatchesCount(int count)
{
    watchesCount = count;
}

void Debugger::processLst()
{
    //set accordance with .lst file and program in memory
    QFile lst;
    lst.setFileName(tmpPath + "program.lst");
    lst.open(QIODevice::ReadOnly);
    QTextStream lstStream(&lst);

    while (!lstStream.atEnd()) {
        QString line = lstStream.readLine();
        if (line.length() <= 37) { //omit strings with data only
            //if in list : line number, address, data and it is all (without instruction) -
            //omit this string and subtract 1 from offset
            omitLinesCount++;
            continue;
        }
        char *s = line.toLocal8Bit().data();
        quint64 a, b, c;
        if (sscanf(s, "%llu %llx %llx", &a, &b, &c) == 3){
            if (!(b == 0 && c == 0)) { //exclude 0 0
                lineNum l;
                l.numInCode = a - 1 - omitLinesCount; //-1 for missing of sasmStartL:
                if (ioIncIncluded) {
                    l.numInCode -= ioIncSize;
                }
                l.numInMem = b + offset;
                lines.append(l);
            }
        }
    }
    lst.close();
}

void Debugger::run()
{
    //set breakpoint on main, run program amd open output and input files
    //put \n after commands!
    //b main and run before others!
    doInput(QString("b main\n"), none);
    doInput(QString("cd " + tmpPath + "\n"), none);
    doInput(QString("run\n"), none);
    doInput(QString("p dup2(open(\"input.txt\",0),0)\n"), none);
}

void Debugger::changeBreakpoint(quint64 lineNumber, bool isAdded)
{
    quint64 numInMem = 0;
    lineNum brkpoint;
    for (int i = 0; i < lines.count(); i++) //find address of line
        if (lineNumber <= lines[i].numInCode) {
            numInMem = lines[i].numInMem;
            break;
        }
    brkpoint.numInCode = lineNumber;
    brkpoint.numInMem = numInMem;

    lineNum pair;
    int count = 0;
    foreach (pair, breakPairs) {
        if (pair.numInMem == numInMem)
            count++;
    }
    if (isAdded) { //if breakpoint was added then set it
        breakPairs.append(brkpoint);
        if (count == 0)
            doInput(QString("b *0x") + QString::number(numInMem, 16) + QString("\n"), breakpoint);
    } else { //if breakpoint was deleted then remove it
        breakPairs.removeOne(brkpoint);
        if (count == 1)
            doInput(QString("clear *0x") + QString::number(numInMem, 16) + QString("\n"), breakpoint);
    }
}

Debugger::~Debugger() {
    emit highlightLine(-1);
    if (process->state() == QProcess::Running) {
        doInput("c\n", none);
        doInput("quit\n", none);
        process->waitForFinished(1000);
        if (process->state() == QProcess::Running) { //if still running
            doInput("quit\n", none);
            process->terminate();
        }
    }
    delete process;
    process = 0;
    lines.clear();
    delete bufferTimer;
}
