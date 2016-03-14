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

#include "fasm.h"

/**
 * @file fasm.cpp
 * Impliments the FASM assembler
 */

FASM::FASM(bool x86, QObject *parent) :
    Assembler(x86, parent)
{
}

QString FASM::getAssemblerPath()
{
    #ifdef Q_OS_WIN32
        return Common::applicationDataPath() + "/FASM/fasm.exe";
    #else
        return "fasm";
    #endif
}

QString FASM::getLinkerPath()
{
    #ifdef Q_OS_WIN32
        if (isx86())
            return Common::applicationDataPath() + "/MinGW/bin/gcc.exe";
        else
            return Common::applicationDataPath() + "/MinGW64/bin/gcc.exe";
    #else
        return "gcc";
    #endif
}

QString FASM::getListingFilePath(QFile &lstOut)
{
    QString listingPath = Common::pathInTemp("fasmListing.txt");
    QFile::remove(listingPath);
    QProcess getLst;
    QStringList getLstArguments;
    getLstArguments << lstOut.fileName() << listingPath;
    getLst.setWorkingDirectory(Common::applicationDataPath() + "/include");
    #ifdef Q_OS_WIN32
        getLst.start(Common::applicationDataPath() + "/FASM/listing.exe", getLstArguments);
    #else
        getLst.start("listing", getLstArguments);
    #endif
    getLst.waitForFinished();
    return listingPath;
}

quint64 FASM::getMainOffset(QFile &lstOut, QString entryLabel)
{
    QFile lst(getListingFilePath(lstOut));
    lst.open(QFile::ReadOnly);
    QTextStream lstStream(&lst);
    QRegExp mainLabel(entryLabel + ":");
    bool flag = false;
    while (!lstStream.atEnd()) {
        QString line = lstStream.readLine();
        if (flag) {
            line = line.split(QChar(':')).at(0);
            if (line.length() > 16)
                continue;
            lst.close();
            return line.toULongLong(0, 16);
        } else {
            if (line.indexOf(mainLabel) != -1)
                flag = true;
        }
    }
    lst.close();
    return -1;
}

void FASM::parseLstFile(QFile &lstOut, QVector<Assembler::LineNum> &lines, quint64 offset)
{
    QFile lst(getListingFilePath(lstOut));
    lst.open(QFile::ReadOnly);

    QTextStream lstStream(&lst);
    QList<QPair<quint64, QString> > instrList;
    while (!lstStream.atEnd()) {
        QString line = lstStream.readLine();
        int index = line.indexOf(QChar(':'));
        if (index == -1 || index > 16)
            continue;
        QString first = line.left(index);
        QString second = line.mid(index + 1);
        int k = 0;
        while (k < second.length() && second[k].isSpace())
            k++;
        second.remove(0, k);
        instrList.append(QPair<quint64, QString>(first.toULongLong(0, 16) + offset, second));
    }
    lst.close();
    QFile programFile(Common::pathInTemp("program.asm"));
    programFile.open(QFile::ReadOnly);
    QTextStream programStream(&programFile);
    int i = 0; //offset in list
    int numInCode = 0;
    while (!programStream.atEnd()) {
        if (i >= instrList.size()) {
            break;
        }
        QString line = programStream.readLine();
        numInCode++;
        int k = 0;
        while (k < line.length() && line[k].isSpace())
            k++;
        line.remove(0, k);
        if (line == instrList[i].second) {
            LineNum l;
            l.numInCode = numInCode;
            l.numInMem = instrList[i].first;
            lines.append(l);
            i++;
        }
    }
    programFile.close();
}

QString FASM::getStartText()
{
    #ifdef Q_OS_WIN32
        if (isx86()) {
            return QString("format ELF\n\nsection '.text' executable\npublic _main\n") +
                   QString("_main:\n    ;write your code here\n    xor eax, eax\n    ret");
        } else {
            return QString("format ELF64\n\nsection '.text' executable\npublic main\n") +
                   QString("main:\n    ;write your code here\n    xor rax, rax\n    ret");
        }
    #else
        if (isx86()) {
            return QString("format ELF\n\nsection '.text' executable\npublic main\n") +
                   QString("main:\n    ;write your code here\n    xor eax, eax\n    ret");
        } else {
            return QString("format ELF64\n\nsection '.text' executable\npublic main\n") +
                   QString("main:\n    ;write your code here\n    xor rax, rax\n    ret");
        }
    #endif
}

void FASM::putDebugString(CodeEditor *code)
{
    //add : mov ebp, esp for making frame for correct debugging if this code has not been added yet
    int index = code->toPlainText().indexOf(QRegExp("main:"));
    if (index != -1) {
        index = code->toPlainText().indexOf(QChar(':'), index);
        if (isx86()) {
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
    }
}

QString FASM::getAssemblerOptions()
{
    return "$SOURCE$ $PROGRAM.OBJ$ -s $LSTOUTPUT$";
}

QString FASM::getLinkerOptions()
{
    QString options;
    if (isx86())
        options = "$PROGRAM.OBJ$ -g -o $PROGRAM$ -m32";
    else
        options = "$PROGRAM.OBJ$ -g -o $PROGRAM$ -m64";
    return options;
}

void FASM::fillHighligherRules(QVector<Assembler::HighlightingRule> &highlightingRules,
                              QList<QTextCharFormat *> &formats,
                              bool &multiLineComments,
                              QRegExp &commentStartExpression,
                              QRegExp &commentEndExpression)
{
    typedef Assembler::HighlightingRule HighlightingRule;
    QTextCharFormat &keywordFormat = *formats[0];
    QTextCharFormat &registerFormat = *formats[1];
    QTextCharFormat &numberFormat = *formats[2];
    QTextCharFormat &memoryFormat = *formats[3];
    QTextCharFormat &labelFormat = *formats[4];
    QTextCharFormat &commentFormat = *formats[5];
    QTextCharFormat &systemFormat = *formats[6];
    //QTextCharFormat &iomacrosFormat = *formats[7];
    QTextCharFormat &quotationFormat = *formats[8];
    QTextCharFormat &labelWithDotFormat = *formats[9];

    //setting up regular expressions
    HighlightingRule rule;

    //keywords
    QStringList keywordPatterns;
    keywordPatterns << "\\bAAA\\b" << "\\bAAD\\b" << "\\bAAM\\b" << "\\bAAS\\b" <<
                       "\\bADC\\b" << "\\bADD\\b" << "\\bAND\\b" << "\\bCALL\\b" <<
                       "\\bCBW\\b" << "\\bCLC\\b" << "\\bCLD\\b" << "\\bCLI\\b" <<
                       "\\bCMC\\b" << "\\bCMP\\b" << "\\bCMPSB\\b" << "\\bCMPSW\\b" <<
                       "\\bCWD\\b" << "\\bDAA\\b" << "\\bDAS\\b" << "\\bDEC\\b" <<
                       "\\bDIV\\b" << "\\bESC\\b" << "\\bHLT\\b" << "\\bIDIV\\b" <<
                       "\\bIMUL\\b" << "\\bIN\\b" << "\\bINC\\b" << "\\bINT\\b" <<
                       "\\bINTO\\b" << "\\bIRET\\b" << "\\bJA\\b" << "\\bJAE\\b" <<
                       "\\bJB\\b" << "\\bJBE\\b" << "\\bJC\\b" << "\\bJCXZ\\b" <<
                       "\\bJE\\b" << "\\bJG\\b" << "\\bJGE\\b" << "\\bJL\\b" <<
                       "\\bJLE\\b" << "\\bJNA\\b" << "\\bJNAE\\b" << "\\bJNB\\b" <<
                       "\\bJNBE\\b" << "\\bJNC\\b" << "\\bJNE\\b" << "\\bJNG\\b" <<
                       "\\bJNGE\\b" << "\\bJNL\\b" << "\\bJNLE\\b" << "\\bJNO\\b" <<
                       "\\bJNP\\b" << "\\bJNS\\b" << "\\bJNZ\\b" << "\\bJO\\b" <<
                       "\\bJP\\b" << "\\bJPE\\b" << "\\bJPO\\b" << "\\bJS\\b" <<
                       "\\bJZ\\b" << "\\bJMP\\b" << "\\bLAHF\\b" << "\\bLDS\\b" <<
                       "\\bLEA\\b" << "\\bLES\\b" << "\\bLOCK\\b" << "\\bLODSB\\b" <<
                       "\\bLODSW\\b" << "\\bLOOP\\b" << "\\bLOOPE\\b" << "\\bLOOPNE\\b" <<
                        "\\bLOOPNZ\\b" << "\\bLOOPZ\\b" << "\\bMOV\\b" << "\\bMOVSB\\b" <<
                        "\\bMOVSW\\b" << "\\bMUL\\b" << "\\bNEG\\b" << "\\bNOP\\b" <<
                        "\\bNOT\\b" << "\\bOR\\b" << "\\bOUT\\b" << "\\bPOP\\b" <<
                        "\\bPOPF\\b" << "\\bPUSH\\b" << "\\bPUSHF\\b" << "\\bRCL\\b" <<
                        "\\bRCR\\b" << "\\bREP\\b" << "\\bREPE\\b" << "\\bREPNE\\b" <<
                        "\\bREPNZ\\b" << "\\bREPZ\\b" << "\\bRET\\b" << "\\bRETN\\b" <<
                        "\\bRETF\\b" << "\\bROL\\b" << "\\bROR\\b" << "\\bSAHF\\b" <<
                        "\\bSAL\\b" << "\\bSAR\\b" << "\\bSBB\\b" << "\\bSCASB\\b" <<
                        "\\bSCASW\\b" << "\\bSHL\\b" << "\\bSHR\\b" << "\\bSTC\\b" <<
                        "\\bSTD\\b" << "\\bSTI\\b" << "\\bSTOSB\\b" << "\\bSTOSW\\b" <<
                        "\\bSUB\\b" << "\\bTEST\\b" << "\\bWAIT\\b" << "\\bXCHG\\b" <<
                        "\\bXLAT\\b" << "\\bXOR\\b" << "\\bBOUND\\b" << "\\bENTER\\b" <<
                        "\\bINS\\b" << "\\bLEAVE\\b" << "\\bOUTS\\b" << "\\bPOPA\\b" <<
                        "\\bPUSHA\\b" << "\\bARPL\\b" << "\\bCLTS\\b" << "\\bLAR\\b" <<
                        "\\bLGDT\\b" << "\\bLIDT\\b" << "\\bLLDT\\b" << "\\bLMSW\\b" <<
                        "\\bLOADALL\\b" << "\\bLSL\\b" << "\\bLTR\\b" << "\\bSGDT\\b" <<
                        "\\bSIDT\\b" << "\\bSLDT\\b" << "\\bSMSW\\b" << "\\bSTR\\b" <<
                        "\\bVERR\\b" << "\\bVERW\\b" << "\\bBSF\\b" << "\\bBSR\\b" <<
                        "\\bBT\\b" << "\\bBTC\\b" << "\\bBTR\\b" << "\\bBTS\\b" <<
                        "\\bCDQ\\b" << "\\bCMPSD\\b" << "\\bCWDE\\b" << "\\bINSD\\b" <<
                        "\\bIRET\\b" << "\\bIRETW\\b" << "\\bIRETD\\b" << "\\bJCXZ\\b" <<
                        "\\bJECXZ\\b" << "\\bLFS\\b" << "\\bLGS\\b" << "\\bLSS\\b" <<
                        "\\bLODSD\\b" << "\\bMOVSD\\b" << "\\bMOVSX\\b" << "\\bMOVZX\\b" <<
                        "\\bOUTSD\\b" << "\\bPOPAD\\b" << "\\bPOPFD\\b" << "\\bPUSHAD\\b" <<
                        "\\bPUSHFD\\b" << "\\bSCASD\\b" << "\\bSETA\\b" << "\\bSETAE\\b" <<
                        "\\bSETB\\b" << "\\bSETBE\\b" << "\\bSETC\\b" << "\\bSETE\\b" <<
                        "\\bSETG\\b" << "\\bSETGE\\b" << "\\bSETL\\b" << "\\bSETLE\\b" <<
                        "\\bSETNA\\b" << "\\bSETNAE\\b" << "\\bSETNB\\b" << "\\bSETNBE\\b" <<
                        "\\bSETNC\\b" << "\\bSETNE\\b" << "\\bSETNG\\b" << "\\bSETNGE\\b" <<
                        "\\bSETNL\\b" << "\\bSETNLE\\b" << "\\bSETNO\\b" << "\\bSETNP\\b" <<
                        "\\bSETNS\\b" << "\\bSETNZ\\b" << "\\bSETO\\b" << "\\bSETP\\b" <<
                        "\\bSETPE\\b" << "\\bSETPO\\b" << "\\bSETS\\b" << "\\bSETZ\\b" <<
                        "\\bSHLD\\b" << "\\bSHRD\\b" << "\\bSTOSD\\b" <<
                        "\\bPOPAD\\b" << "\\bPOPFD\\b" << "\\bPUSHAD\\b" << "\\bPUSHFD\\b" <<
                        "\\bSCASD\\b" << "\\bBSWAP\\b" << "\\bCMPXCHG\\b" << "\\bINVD\\b" <<
                        "\\bINVLPG\\b" << "\\bWBINVD\\b" << "\\bXADD\\b" << "\\bCPUID\\b" <<
                        "\\bCMPXCHG8B\\b" << "\\bRDMSR\\b" << "\\bRDTSC\\b" << "\\bWRMSR\\b" <<
                        "\\bRSM\\b" << "\\bRDPMC\\b" << "\\bCMOVA\\b" << "\\bCMOVAE\\b" <<
                        "\\bCMOVB\\b" << "\\bCMOVBE\\b" << "\\bCMOVC\\b" << "\\bCMOVE\\b" <<
                        "\\bCMOVG\\b" << "\\bCMOVGE\\b" << "\\bCMOVL\\b" << "\\bCMOVLE\\b" <<
                        "\\bCMOVNA\\b" << "\\bCMOVNAE\\b" << "\\bCMOVNB\\b" << "\\bCMOVNBE\\b" <<
                        "\\bCMOVNC\\b" << "\\bCMOVNE\\b" << "\\bCMOVNG\\b" << "\\bCMOVNGE\\b" <<
                        "\\bCMOVNL\\b" << "\\bCMOVNLE\\b" << "\\bCMOVNO\\b" << "\\bCMOVNP\\b" <<
                        "\\bCMOVNS\\b" << "\\bCMOVNZ\\b" << "\\bCMOVO\\b" << "\\bCMOVP\\b" <<
                        "\\bCMOVPE\\b" << "\\bCMOVPO\\b" << "\\bCMOVS\\b" << "\\bCMOVZ\\b" <<
                        "\\bF2XM1\\b" << "\\bFABS\\b" << "\\bFADD\\b" << "\\bFADDP\\b" <<
                        "\\bFBLD\\b" << "\\bFBSTP\\b" << "\\bFCHS\\b" << "\\bFCLEX\\b" <<
                        "\\bFCOM\\b" << "\\bFCOMP\\b" << "\\bFCOMPP\\b" << "\\bFDECSTP\\b" <<
                        "\\bFDISI\\b" << "\\bFDIV\\b" << "\\bFDIVP\\b" << "\\bFDIVR\\b" <<
                        "\\bFDIVRP\\b" << "\\bFENI\\b" << "\\bFFREE\\b" << "\\bFIADD\\b" <<
                        "\\bFICOM\\b" << "\\bFICOMP\\b" << "\\bFIDIV\\b" << "\\bFIDIVR\\b" <<
                        "\\bFILD\\b" << "\\bFIMUL\\b" << "\\bFINCSTP\\b" << "\\bFINIT\\b" <<
                        "\\bFIST\\b" << "\\bFISTP\\b" << "\\bFISUB\\b" << "\\bFISUBR\\b" <<
                        "\\bFLD\\b" << "\\bFLD1\\b" << "\\bFLDCW\\b" << "\\bFLDENV\\b" <<
                        "\\bFLDENVW\\b" << "\\bFLDL2E\\b" << "\\bFLDL2T\\b" << "\\bFLDLG2\\b" <<
                        "\\bFLDLN2\\b" << "\\bFLDPI\\b" << "\\bFLDZ\\b" << "\\bFMUL\\b" <<
                        "\\bFMULP\\b" << "\\bFNCLEX\\b" << "\\bFNDISI\\b" << "\\bFNENI\\b" <<
                        "\\bFNINIT\\b" << "\\bFNOP\\b" << "\\bFNSAVE\\b" << "\\bFNSAVEW\\b" <<
                        "\\bFNSTCW\\b" << "\\bFNSTENV\\b" << "\\bFNSTENVW\\b" << "\\bFNSTSW\\b" <<
                        "\\bFPATAN\\b" << "\\bFPREM\\b" << "\\bFPTAN\\b" << "\\bFRNDINT\\b" <<
                        "\\bFRSTOR\\b" << "\\bFRSTORW\\b" << "\\bFSAVE\\b" << "\\bFSAVEW\\b" <<
                        "\\bFSCALE\\b" << "\\bFSQRT\\b" << "\\bFST\\b" << "\\bFSTCW\\b" <<
                        "\\bFSTENV\\b" << "\\bFSTENVW\\b" << "\\bFSTP\\b" << "\\bFSTSW\\b" <<
                        "\\bFSUB\\b" << "\\bFSUBP\\b" << "\\bFSUBR\\b" << "\\bFSUBRP\\b" <<
                        "\\bFTST\\b" << "\\bFWAIT\\b" << "\\bFXAM\\b" << "\\bFXCH\\b" <<
                        "\\bFXTRACT\\b" << "\\bFYL2X\\b" << "\\bFYL2XP1\\b" << "\\bFSETPM\\b" <<
                        "\\bFCOS\\b" << "\\bFLDENVD\\b" << "\\bFSAVED\\b" << "\\bFSTENVD\\b" <<
                        "\\bFPREM1\\b" << "\\bFRSTORD\\b" << "\\bFSIN\\b" << "\\bFSINCOS\\b" <<
                        "\\bFSTENVD\\b" << "\\bFUCOM\\b" << "\\bFUCOMP\\b" << "\\bFUCOMPP\\b" <<
                        "\\bFCMOVB\\b" << "\\bFCMOVBE\\b" << "\\bFCMOVE\\b" << "\\bFCMOVNB\\b" <<
                        "\\bFCMOVNBE\\b" << "\\bFCMOVNE\\b" << "\\bFCMOVNU\\b" << "\\bFCMOVU\\b" <<
                        "\\bFCOMI\\b" << "\\bFCOMIP\\b" << "\\bFUCOMI\\b" << "\\bFUCOMIP\\b" <<
                        "\\bCDQE\\b" << "\\bCQO\\b" << "\\bMOVMSKPS\\b" << "\\bMOVMSKPD\\b" <<
                        "\\bPOPCNT\\b" << "\\bLZCNT\\b" << "\\bCMPSQ\\b" << "\\bSCASQ\\b" <<
                        "\\bMOVSQ\\b" << "\\bLODSQ\\b" << "\\bSTOSQ\\b" << "\\bJRCXZ\\b" <<
                        "\\bIRETQ\\b" << "\\bPUSHFQ\\b" << "\\bPOPFQ\\b" << "\\bCMPXCHG16B\\b" <<
                        "\\bJRCXZ\\b" << "\\bINSB\\b" << "\\bINSW\\b" << "\\bOUTSB\\b" <<
                        "\\bOUTSW\\b" << "\\bLFENCE\\b" << "\\bSFENCE\\b" << "\\bMFENCE\\b" <<
                        "\\bPREFETCH\\b" << "\\bPREFETCHL\\b" << "\\bPREFETCHW\\b" << "\\bCLFLUSH\\b" <<
                        "\\bSYSENTER\\b" << "\\bSYSEXIT\\b" << "\\bSYSCALL\\b" << "\\bSYSRET\\b";
    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.pattern.setCaseSensitivity(Qt::CaseInsensitive);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    //memory
    rule.format = memoryFormat;
    QStringList memoryPatterns;
    rule.pattern = QRegExp("\\[");
    highlightingRules.append(rule);
    rule.pattern = QRegExp("\\]");
    highlightingRules.append(rule);
    memoryPatterns << "\\bdb\\b" << "\\bdw\\b" << "\\bdu\\b" <<
                      "\\bdd\\b" << "\\bdp\\b" << "\\bdf\\b" <<
                      "\\bdq\\b" << "\\bdt\\b" << "\\bequ\\b" <<
                      "\\brb\\b" << "\\brw\\b" << "\\brd\\b" <<
                      "\\brp\\b" << "\\brf\\b" << "\\brq\\b" <<
                      "\\brt\\b" <<
                      "\\bbyte[\\s\\[]" << "\\bword[\\s\\[]" <<
                      "\\bdword[\\s\\[]" << "\\bfword[\\s\\[]" <<
                      "\\bpword[\\s\\[]" << "\\bqword[\\s\\[]" <<
                      "\\btword[\\s\\[]" << "\\bdqword[\\s\\[]" <<
                      "\\bxword[\\s\\[]" << "\\bqqword[\\s\\[]" <<
                      "\\byword[\\s\\[]" << "\\btbyte[\\s\\[]" <<
                      "\\bdup\\b" << "\\bfile\\b" << "\\bptr\\b" <<
                      "\\blabel\\b";
    foreach (const QString &pattern, memoryPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.pattern.setCaseSensitivity(Qt::CaseInsensitive);
        highlightingRules.append(rule);
    }

    //labels
    rule.pattern = QRegExp("\\S+:");
    rule.format = labelFormat;
    highlightingRules.append(rule);

    //numbers
    rule.format = numberFormat;
    rule.pattern = QRegExp("\\b[\\-\\+]?\\d+[bod]?\\b");
    highlightingRules.append(rule);
    rule.pattern = QRegExp("\\b0[bo]\\d+\\b");
    highlightingRules.append(rule);
    rule.pattern = QRegExp("\\b[0-9A-Fa-f]+h\\b"); //hexadecimal notation
    highlightingRules.append(rule);
    rule.pattern = QRegExp("\\b0[xh][0-9A-Fa-f]+\\b"); //hexadecimal notation
    highlightingRules.append(rule);

    //registers
    QStringList registerPatterns;
    registerPatterns << "\\beax\\b" << "\\bebx\\b" << "\\becx\\b" <<
                        "\\bedx\\b" << "\\bebp\\b" << "\\besp\\b" <<
                        "\\bedi\\b" << "\\besi\\b" << "\\bax\\b" <<
                        "\\bbx\\b" << "\\bcx\\b" << "\\bdx\\b" <<
                        "\\bbp\\b" << "\\bsp\\b" << "\\bsi\\b" <<
                        "\\bdi\\b" << "\\bal\\b" << "\\bah\\b" <<
                        "\\bbl\\b" << "\\bbh\\b" << "\\bcl\\b" <<
                        "\\bch\\b" << "\\bdl\\b" << "\\bdh\\b" <<
                        "\\brax\\b" << "\\brbx\\b" << "\\brcx\\b" << //x64
                        "\\brdx\\b" << "\\brbp\\b" << "\\brsp\\b" <<
                        "\\brdi\\b" << "\\brsi\\b" << "\\bspl\\b" <<
                        "\\bbpl\\b" << "\\bsil\\b" << "\\bdil\\b" <<
                        "\\br8\\b" << "\\br8d\\b" << "\\br8w\\b" <<
                        "\\br8b\\b" << "\\br9\\b" << "\\br9d\\b" <<
                        "\\br9w\\b" << "\\br9b\\b" << "\\br10\\b" <<
                        "\\br10d\\b" << "\\br10w\\b" << "\\br10b\\b" <<
                        "\\br11\\b" << "\\br11d\\b" << "\\br11w\\b" <<
                        "\\br11b\\b" << "\\br12\\b" << "\\br12d\\b" <<
                        "\\br12w\\b" << "\\br12b\\b" << "\\br13\\b" <<
                        "\\br13d\\b" << "\\br13w\\b" << "\\br13b\\b" <<
                        "\\br14\\b" << "\\br14d\\b" << "\\br14w\\b" <<
                        "\\br14b\\b" << "\\br15\\b" << "\\br15d\\b" <<
                        "\\br15w\\b" << "\\br15b\\b";
    foreach (const QString &pattern, registerPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.pattern.setCaseSensitivity(Qt::CaseInsensitive);
        rule.format = registerFormat;
        highlightingRules.append(rule);
    }

    //.labels and numbers with point
    rule.pattern = QRegExp("\\.[^\\s:]+[^:]");
    rule.format = labelWithDotFormat;
    highlightingRules.append(rule);
    rule.pattern = QRegExp("\\.[^\\s:]+:");
    rule.format = labelFormat;
    highlightingRules.append(rule);
    rule.pattern = QRegExp("\\b[\\-\\+]?\\d+\\.\\d+\\b");
    rule.format = numberFormat;
    highlightingRules.append(rule);

    //system instructions and preprocessor commands
    rule.format = systemFormat;
    QStringList systemPatterns;
    systemPatterns << "\\bformat\\b" << "\\buse16\\b" << "\\buse32\\b" <<
                      "\\bbinary\\b" << "\\bMZ\\b" << "\\bsegment\\b" <<
                      "\\bPE\\b" << "\\bstack\\b" << "\\bheap\\b" <<
                      "\\bconsole\\b" << "\\bnative\\b" << "\\bGUI\\b" <<
                      "\\bEFI\\b" << "\\bEFIboot\\b" << "\\bEFIruntime\\b" <<
                      "\\bDLL\\b" << "\\bWDM\\b" << "\\blarge\\b" <<
                      "\\bat\\b" << "\\bon\\b" << "\\bPE64\\b" <<
                      "\\bsection\\b" << "\\bcode\\b" << "\\bdata\\b" <<
                      "\\breadable\\b" << "\\bwriteable\\b" << "\\bexecutable\\b" <<
                      "\\bshareable\\b" << "\\bdiscardable\\b" << "\\bnotpageable\\b" <<
                      "\\bexport\\b" << "\\bimport\\b" << "\\bresource\\b" <<
                      "\\bfixups\\b" << "\\bentry\\b" <<
                      "\\brva\\b" << "\\bCOFF\\b" << "\\bMS COFF\\b" <<
                      "\\bMS64 COFF\\b" << "\\blinkremove\\b" << "\\blinkinfo\\b" <<
                      "\\bmod\\b" << "\\bshort\\b" <<
                      "\\bnear\\b" << "\\bfar\\b" <<
                      "\\bextrn\\b" << "\\bpublic\\b" <<
                      "\\bas\\b" << "\\bstatic\\b" << "\\bELF\\b" <<
                      "\\bELF64\\b" << "\\bplt\\b" << "\\binterpreter\\b" <<
                      "\\bdynamic\\b" << "\\bnote\\b" << "\\bif\\b" <<
                      "\\belse\\b" << "\\bend\\b" << "\\bused\\b" <<
                      "\\bdefined\\b" << "\\brelativeto\\b" << "\\beq\\b" <<
                      "\\bin\\b" << "\\beqtype\\b" << "\\btimes\\b" <<
                      "\\brepeat\\b" << "\\bbreak\\b" << "\\bwhile\\b" <<
                      "\\borg\\b" << "\\bload\\b" << "\\bfrom\\b" <<
                      "\\bstore\\b" << "\\bvirtual\\b" << "\\balign\\b" <<
                      "\\bdisplay\\b" << "\\berr\\b" << "\\bassert\\b" <<
                      "\\binclude\\b" << "\\brestore\\b" << "\\bmacro\\b" <<
                      "\\bpurge\\b" << "\\bforward\\b" << "\\breverse\\b" <<
                      "\\bcommon\\b" << "\\bstruc\\b" << "\\brept\\b" <<
                      "\\birp\\b" << "\\birps\\b" << "\\bmatch\\b" <<
                      "\\bfix\\b";
    foreach (const QString &pattern, systemPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.pattern.setCaseSensitivity(Qt::CaseInsensitive);
        highlightingRules.append(rule);
    }

    //quotations
    rule.pattern = QRegExp("\".*\"");
    rule.format = quotationFormat;
    rule.pattern.setMinimal(true);
    highlightingRules.append(rule);

    rule.pattern = QRegExp("'.*'");
    rule.pattern.setMinimal(true);
    highlightingRules.append(rule);

    rule.pattern = QRegExp("`.*`");
    rule.pattern.setMinimal(true);
    highlightingRules.append(rule);

    //comments
    rule.pattern = QRegExp(";[^\n]*");
    rule.format = commentFormat;
    rule.isComment = true;
    highlightingRules.append(rule);
    multiLineComments = false;
    commentStartExpression = QRegExp();
    commentEndExpression = QRegExp();
}
