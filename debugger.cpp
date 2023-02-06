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

#include "debugger.h"

/**
 * @file debugger.cpp
 * Sets debugger information and runs the debugger.
 */

namespace {
    // read struct like {<name1> = <value1>, ,,, <nameN> = <valueN>}
    // from stream. Values may be simple or complex, like {v1, v2, v3 .. }
    // all i have to be simple, two-level complex values unsupported
    bool readStruct(QTextStream& str, QMap<QString, QString>* map);
}


Debugger::Debugger(QTextEdit *tEdit,
                   const QString &exePathParam,
                   const QString &workingDirectoryPathParam,
                   const QString &inputPathParam,
                   Assembler *assembler,
                   QWidget *parent)
    : QObject(parent)
{
    QSettings settings("SASM Project", "SASM");
    gdbRun = false;
    c = 0;
    pid = 0;
    firstAction = true;
    textEdit = tEdit;
    exePath = exePathParam;
    workingDirectoryPath = workingDirectoryPathParam;
    inputPath = inputPathParam;
    registersOk = true;
    this->assembler = assembler;
    #ifdef Q_OS_WIN32
        QString gdb;
        QString objdump;
        if (settings.value("mode", QString("x86")).toString() == "x86") {
            gdb = QCoreApplication::applicationDirPath() + "/MinGW/bin/gdb.exe";
            objdump = QCoreApplication::applicationDirPath() + "/MinGW/bin/objdump.exe";
            if (! QFile::exists(gdb))
                gdb = QCoreApplication::applicationDirPath() + "/Windows/MinGW/bin/gdb.exe";
            if (! QFile::exists(objdump))
                objdump = QCoreApplication::applicationDirPath() + "/Windows/MinGW/bin/objdump.exe";
            exitMessage = "mingw_CRTStartup";
        } else {
            gdb = QCoreApplication::applicationDirPath() + "/MinGW64/bin/gdb.exe";
            objdump = QCoreApplication::applicationDirPath() + "/MinGW/bin/objdump.exe";
            if (! QFile::exists(gdb))
                gdb = QCoreApplication::applicationDirPath() + "/Windows/MinGW64/bin/gdb.exe";
            if (! QFile::exists(objdump))
                objdump = QCoreApplication::applicationDirPath() + "/Windows/MinGW/bin/objdump.exe";
            exitMessage = "__fu0__set_invalid_parameter_handler";
        }
    #else
        QString gdb = "gdb";
        QString objdump = "objdump";
        exitMessage = "libc_start_main";
    #endif
    cExitMessage = QRegExp("\\[Inferior .* exited");

    connect(this, SIGNAL(wasStopped()), this, SLOT(emitStarted()));

    //determine entry point
    QProcess objdumpProcess;
    QProcessEnvironment objdumpEnvironment = QProcessEnvironment::systemEnvironment();
    QStringList objdumpArguments;
    objdumpArguments << "-f" << exePath;
	objdumpEnvironment.insert("LC_MESSAGES", "en_US");
	objdumpProcess.setProcessEnvironment(objdumpEnvironment);
    objdumpProcess.start(objdump, objdumpArguments);
    objdumpProcess.waitForFinished();
    QString objdumpResult = QString(objdumpProcess.readAllStandardOutput());
    QString startAddress("start address ");
    int index = objdumpResult.indexOf(startAddress);
    objdumpResult = objdumpResult.mid(index + startAddress.length());
    entryPoint = objdumpResult.toLongLong(0, 16);


    QStringList arguments;
    arguments << "-nx" << exePath;

    process = new QProcess;
    process->start(gdb, arguments);

    QObject::connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(readOutputToBuffer()));
    QObject::connect(process, SIGNAL(readyReadStandardError()), this, SLOT(readOutputToBuffer()));

    bufferTimer = new QTimer;
    QObject::connect(bufferTimer, SIGNAL(timeout()), this, SLOT(processOutput()), Qt::QueuedConnection);
    bufferTimer->start(10);

    checkGdbRunTimer = new QTimer;
    QObject::connect(checkGdbRunTimer, SIGNAL(timeout()), this, SLOT(checkGdbRun()), Qt::QueuedConnection);
    checkGdbRunTimer->start(10000);
}

void Debugger::emitStarted()
{
    disconnect(this, SIGNAL(wasStopped()), this, SLOT(emitStarted()));
    emit started();
}

void Debugger::readOutputToBuffer()
{
    if (!process)
        return;
    QByteArray error = process->readAllStandardError();
    errorBuffer += QString::fromLocal8Bit(error.constData(), error.size());
    allErrorBuffer += QString::fromLocal8Bit(error.constData(), error.size());
    QByteArray output = process->readAllStandardOutput();
    buffer += QString::fromLocal8Bit(output.constData(), output.size());
    allBuffer += QString::fromLocal8Bit(output.constData(), output.size());
}

void Debugger::processOutput()
{
    bufferTimer->stop();
    int index = buffer.indexOf(QString("(gdb)"));
    /* static QString prevBuffer; // for debugging
    if (prevBuffer != buffer) {
        prevBuffer = buffer;
        emit printLog(buffer, Qt::magenta);
    } */
    int linefeedIndex = errorBuffer.indexOf("\n");
    if (index != -1) { //if whole message ready to processing (end of whole message is "(gdb)")
        QString output = buffer.left(index);
        QString error = errorBuffer.left(linefeedIndex);
        buffer.remove(0, index + 5); //remove processed message
        errorBuffer.remove(0, linefeedIndex + 1);
        //emit printLog(output, Qt::blue); // for debugging
        processMessage(output, error);
    }
    bufferTimer->start(10);
}

void Debugger::checkGdbRun()
{
    checkGdbRunTimer->stop();
    if (!gdbRun) {
        emit printLog(tr("GDB error\n"), Qt::red);
        emit printLog(allBuffer, Qt::red);
        emit printLog(allErrorBuffer, Qt::red);
        emit finished();
    }
}

void Debugger::processMessage(QString output, QString error)
{
    if (error.indexOf("PC register is not available") != -1) {
        emit printLog(tr("GDB error\n"), Qt::red);
        emit finished();
        return;
    }
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
        c++;
        doInput(QString("disas main\n"), none);
        return;
    }

    if (c == 1) {
    if (error.indexOf("No symbol",0,Qt::CaseInsensitive)!=-1) {
            dbgSymbols = false;
        } else {
            dbgSymbols = true;
        }
    }

    if (dbgSymbols) { //debug symbols exists
        if (c == 1 && output != " ") {
            /*Dump of assembler code for function sasmStartL:
               0x00401390 <+0>:	xor    %eax,%eax
               0x00401392 <+2>:	ret
               0x00401393 <+3>:	add    %dl,-0x77(%ebp)
            End of assembler dump.*/
            //skip this information (for gdb 8.0)
            c++;
            run(); //perform Debugger::run(), that run program and open I/O files
            return;
        }

        //determine run of program
        //wait for message like this: Breakpoint 1, 0x00401390 in sasmStartL ()
        if (c == 2 && output.indexOf(QString(" in ")) != -1) {
            //set accordance between program in memory and program in file
            //in example we need 0x00401390

            //count offset
            QRegExp r = QRegExp("0x[0-9a-fA-F]{8,16}");
            int index = r.indexIn(output);
            //take offset in hexadecimal representation (10 symbols) from string and convert it to int
            offset = output.mid(index, r.matchedLength()).toULongLong(0, 16);
            processLst(); //count accordance

            c++;
            actionTypeQueue.enqueue(ni);
            doInput("info inferiors\n", none);
        }

        //if an error with the wrong name of the section has occurred
        if ((c == 2 && output.indexOf(QString("Make breakpoint pending on future shared library load")) != -1)
                || (c == 1 && output == " ")) {
            actionTypeQueue.enqueue(anyAction);
            processAction(tr("An error has occurred in the debugger. Please check the names of the sections."));
            QObject::disconnect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(readOutputToBuffer()));
            emit finished();
        }
    } else { //debug symbols does not exists (for example, non-gcc linker)
        if (c == 1) {
            offset = entryPoint; //changes in processLst()
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
            doInput("info inferiors\n", none);
        }
    }

    //interrupt while debugging (program was stopped)
    #ifdef Q_OS_WIN32
        QString interruptSig("SIGTRAP");
    #else
        QString interruptSig("SIGINT");
    #endif
    if (output.indexOf(interruptSig) != -1) {
        stopped = true;
        emit wasStopped();
        return;
    }

    if (!pid) {
        QRegExp r("Num +Description +Executable");
        int index = output.indexOf(r);
        if (index != -1) {
            QString processString("process ");
            r = QRegExp(processString + "[0-9]+");
            index = output.indexOf(r);
            output = output.mid(index + processString.length());
            output = output.split(QChar(' ')).at(0);
            pid = output.toULongLong(0, 10);
            return;
        }
    }

    //process all actions after start
    if (c == 3)
        if (output.indexOf(QString("$1 =")) == -1) //input file
            processAction(output, error);
}

void Debugger::processAction(QString output, QString error)
{
    bool backtrace = (output.indexOf(QRegExp("#\\d+  0x[0-9a-fA-F]{8,16} in .* ()")) != -1);
    if (output.indexOf(exitMessage) != -1 && !backtrace) {
        doInput("c\n", none);
        return;
    }
    if (output.indexOf(QRegExp(cExitMessage)) != -1) { //if debug finished
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
            QString msg = output.left(index); //left part - probably output of program;
            msg.remove(0, 1); //remove first whitespace
            QRegExp continuingMsg("Continuing.\r?\n");
            QRegExp breakpointMsg("\r?\nBreakpoint \\d+, ");
            QRegExp threadMsg("\\[Switching to Thread [^\\]]*\\]\r?\n");
            QRegExp signalMsg("\r?\n(Program received signal.*)");
            msg.remove(continuingMsg);
            msg.remove(breakpointMsg);
            msg.remove(threadMsg);
            if (signalMsg.indexIn(msg) != -1) {
                QString recievedSignal = signalMsg.cap(1);
                if (QRegExp("SIG(TRAP|INT)").indexIn(recievedSignal) == -1) {
                    emit printLog(recievedSignal, Qt::red);
                }
                msg.remove(signalMsg);
            }
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
            gdbRun = true;
            emit highlightLine(lineNumber);
            stopped = true;
            emit wasStopped();

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
                output = output.trimmed();
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

        QSet<QString> general;
        QString ip;
        QSet<QString> segment;
        segment << "cs" << "ds" << "ss" << "es" << "fs" << "gs";
        QSet<QString> flags;
        flags << "eflags" << "mxcsr";
        QSet<QString> fpu_info;
        fpu_info << "fctrl" << "fstat" << "ftag" << "fiseg" << "fioff" << "foseg" << "fooff" << "fop";
        QRegExp mmx("mm\\d+");
        QRegExp xmm("xmm\\d+");
        QRegExp ymm("ymm\\d+");
        QRegExp fpu_stack("st\\d+");

        if (settings.value("mode", QString("x86")).toString() == "x86") {
            //x86
            general << "eax" << "ebx" << "ecx" << "edx" << "esi" << "edi" << "esp" << "ebp";
            ip = "eip";
        } else {
            //x64
            general << "rax" << "rbx" << "rcx" << "rdx" << "rsi" << "rdi" << "rsp" << "rbp" <<
                       "r8" << "r9" << "r10" << "r11" << "r12" << "r13" << "r14" << "r15";
            ip = "rip";
        }

        for (int i = 0; !registersStream.atEnd(); i++) {
            registersStream >> info.name;

            if (info.name.isEmpty()) {
                // last empty line
                break;
            }

            if (general.contains(info.name) || segment.contains(info.name) || fpu_info.contains(info.name) ||
                    info.name == ip || flags.contains(info.name) || fpu_stack.exactMatch(info.name)) {
                registersStream >> info.hexValue;
                registersStream.skipWhiteSpace();
                info.decValue = registersStream.readLine();
            } else if (mmx.exactMatch(info.name) || xmm.exactMatch(info.name) || ymm.exactMatch(info.name)) {
                QMap<QString, QString> fields;

                if (!readStruct(registersStream, &fields)) {
                    // bad news
                    emit printLog(QString("can not parse register data: ") + info.name + "\n", Qt::red);
                    break;
                }

                if (mmx.exactMatch(info.name))
                    info.decValue = fields["v8_int8"];
                else if (xmm.exactMatch(info.name))
                    info.decValue = fields["uint128"];
                else if (ymm.exactMatch(info.name))
                    info.decValue = fields["v2_int128"];

                info.hexValue = "";
            } else {
                // unknown register, try to skip it.
                registersStream.readLine();
                emit printLog(QString("unknown register: ") + info.name + "\n", Qt::red);
                continue;
            }

            registers.append(info);
            if (i == 0 && info.name != "eax" && info.name != "rax" && registersOk) {
                doInput(QString("info registers\n"), infoRegisters);
                registersOk = false;
                return;
            }
        }

        emit printRegisters(registers);
        return;
    }

    if (output == QString("\r\n") || output == QString("\n") ||
            output == QString("\r\n\n") || output == QString("\n\n")) //if empty
        return;

    //print information to log field
    emit printLog(output);
}

bool Debugger::isStopped()
{
    return stopped;
}

void Debugger::pause()
{
    if (pid == 0)
	return;
    actionTypeQueue.clear();
    #ifdef Q_OS_WIN32
        HANDLE proc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, (DWORD)pid);
        if (proc != NULL) {
    	    DebugBreakProcess(proc);
    	    CloseHandle(proc);
        }
    #else
    	kill(pid, SIGINT);
    #endif
}

void Debugger::doInput(QString command, DebugActionType actionType)
{
    if (actionType != none)
        actionTypeQueue.enqueue(actionType);
    //put \n after commands!
    if (actionType == ni || actionType == si || actionType == showLine) {
        emit highlightLine(-2); //does not turn off debug mode
        stopped = false;
    }
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
    lst.setFileName(Common::pathInTemp("program.lst"));
    if (lst.open(QIODevice::ReadOnly)) {
        //heuristic:
        //if debug symbols exists - offset is difference between main labels in listing and in executable
        //otherwise - offset is difference between entry point, labeled as "start", in listing and in executable
        quint64 mainOffset;
        if (dbgSymbols)
            mainOffset = assembler->getMainOffset(lst, "main");
        else
            mainOffset = assembler->getMainOffset(lst, "start");
        lst.close();
        if ((qint64) mainOffset == -1) {
            actionTypeQueue.enqueue(anyAction);
            processAction(
                tr("Entry point was not found! Entry point should have label \"main\" (if gcc linker is used) or \"start\" (otherwise)."));
            QObject::disconnect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(readOutputToBuffer()));
            emit finished();
            return;
        }
        offset -= mainOffset;
        lst.open(QIODevice::ReadOnly);
        assembler->parseLstFile(lst, lines, offset);
        lst.close();
        if (lines.isEmpty()) {
            actionTypeQueue.enqueue(anyAction);
            processAction(
                tr("Executable sections were not found! ") +
                tr("For correct debugging executable sections should have name \".text\" or \".code\" (for MASM only)."));
            QObject::disconnect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(readOutputToBuffer()));
            emit finished();
            return;
        }
    }
}

void Debugger::run()
{
    //set breakpoint on main, run program amd open output and input files
    //put \n after commands!
    //b main and run before others!
    if (dbgSymbols)
        doInput(QString("b *main\n"), none);
    else {
        doInput("b *0x" + QString::number(entryPoint, 16) + "\n", none);
    }
    doInput(QString("cd " + workingDirectoryPath + "\n"), none);
    doInput(QString("run < %1\n").arg(inputPath), none);
    doInput(QString("p freopen(\"%1\", \"r\", fdopen(0, \"r\"))\n").arg(inputPath), none);
}

void Debugger::changeBreakpoint(quint64 lineNumber, bool isAdded)
{
    bool wasStarted = false;
    if (!isStopped()) {
        wasStarted = true;
        QEventLoop eventLoop;
        connect(this, SIGNAL(wasStopped()), &eventLoop, SLOT(quit()));
        pause();
        eventLoop.exec();
    }
    quint64 numInMem = 0;
    LineNum brkpoint;
    for (int i = 0; i < lines.count(); i++) //find address of line
        if (lineNumber <= lines[i].numInCode) {
            numInMem = lines[i].numInMem;
            break;
        }
    brkpoint.numInCode = lineNumber;
    brkpoint.numInMem = numInMem;

    LineNum pair;
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
    if (wasStarted)
        emit needToContinue();
}

Debugger::~Debugger() {
    emit highlightLine(-1);
    if (process->state() == QProcess::Running) {
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

namespace {

    // read struct like {<name1> = <value1>, ,,, <nameN> = <valueN>}
    // from stream. Values may be simple or complex, like {v1, v2, v3 .. }
    // all vi have to be simple, two-level complex values unsupported
    //
    // test end of stream before any reading, desperately trying not to hang on it
    bool readStruct(QTextStream& str, QMap<QString, QString>* map) {
        str.skipWhiteSpace();
        if (str.atEnd())
            return false;

        QChar c;
        str >> c;
        if (c != '{')
            return false;

        // struct begin
        while (!str.atEnd()) {
            str.skipWhiteSpace();

            // may be it just empty
            str >> c;
            if (c == '}')
                return true;

            // read name, until space or '='
            QString name;
            name += c;

            for (;;) {
                if (str.atEnd())
                    return false;
                str >> c;
                if (c.isSpace() || (c == '=')) break;
                name += c;
            }

            // read '='
            if (c != '=') {
                str.skipWhiteSpace();

                if (str.atEnd())
                    return false;
                str >> c;
                if (c != '=') // it has to be assignment
                    return false;
            }

            str.skipWhiteSpace();

            // read value, simple (until space, or ',', or '}') or complex (until '}')
            QString value;

            if (str.atEnd())
                return false;
            str >> c;

            if (c != '{') {
                // simple value
                value += c;

                for (;;) {
                    if (str.atEnd())
                        return false;
                    str >> c;
                    if (c.isSpace() || (c == ',') || (c == '}')) break;
                    value += c;
                }
            } else {
                // complex value
                value += '{';

                for (;;) {
                    if (str.atEnd())
                        return false;
                    str >> c;
                    if (c == '}') break;
                    if (c.isSpace()) {
                        // replace all space characters by simple space
                        // and skip spaces after '{' and after space
                        if (!value.endsWith('{') && !value.endsWith(' '))
                            value += ' ';
                    } else {
                        value += c;
                    }
                }

                value += '}';
                c = ' '; // it was not last }, just end of value
            }

            (*map)[name] = value;

            // to next name-value pair
            if (c.isSpace()) {
                str.skipWhiteSpace();
                if (str.atEnd())
                    return false;
                str >> c;
            }

            if (c == '}')  // end of struct
                return true;
            if (c != ',')  // it has to be comma
                return false;

        }

        return false;
    }
}
