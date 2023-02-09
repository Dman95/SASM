#include "gccbasedassembler.h"

#include <QProcess>
#include <QProcessEnvironment>
#include <QStringList>

#ifndef Q_OS_WIN32
namespace
{
   bool testPieOnUnix()
   {
       QProcess gccProcess;
       QProcessEnvironment gccEnvironment = QProcessEnvironment::systemEnvironment();
       QStringList gccArguments;
       gccArguments << "-v";
       gccEnvironment.insert("LC_MESSAGES", "en_US");
       gccProcess.setProcessEnvironment(gccEnvironment);
       gccProcess.start("gcc", gccArguments);
       gccProcess.waitForFinished();
       QString gccResult = QString(gccProcess.readAllStandardError());
       return gccResult.indexOf("--enable-default-pie") != -1;
   }
}
#endif

GccBasedAssembler::GccBasedAssembler(bool x86, QObject *parent)
    : Assembler(x86, parent)
    , m_isPieEnabled(false)
{
#ifndef Q_OS_WIN32
    m_isPieEnabled = testPieOnUnix();
#endif

}


QString GccBasedAssembler::getLinkerPath()
{
#ifdef Q_OS_WIN32
    if (isx86())
        return Common::applicationDataPath() + "/MinGW/bin/gcc.exe";
    else
        return Common::applicationDataPath() + "/MinGW64/bin/gcc.exe";
#elif defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__) || defined(__DragonFly__)
    return "/usr/bin/cc";
#else
    return "gcc";
#endif
}

QString GccBasedAssembler::getLinkerOptions()
{
    QString options;
    if (isx86())
        options = "$PROGRAM.OBJ$ $MACRO.OBJ$ -g -o $PROGRAM$ -m32";
    else
        options = "$PROGRAM.OBJ$ $MACRO.OBJ$ -g -o $PROGRAM$ -m64";
    if (m_isPieEnabled)
        options += " -no-pie";
    return options;
}

