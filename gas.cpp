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

#include "gas.h"

/**
 * @file gas.cpp
 * Impliments the GAS assembler.
 */

GAS::GAS(bool x86, QObject *parent) :
    Assembler(x86, parent)
{
}

QString GAS::getAssemblerPath()
{
    #ifdef Q_OS_WIN32
        if (isx86())
            return Common::applicationDataPath() + "/GAS/32/as.exe";
        else
            return Common::applicationDataPath() + "/GAS/64/as.exe";
    #else
        return "as";
    #endif
}

QString GAS::getLinkerPath()
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

quint64 GAS::getMainOffset(QFile &lst, QString entryLabel)
{
    QTextStream lstStream(&lst);
    QRegExp mainLabel(entryLabel + ":");
    bool flag = false;
    while (!lstStream.atEnd()) {
        QString line = lstStream.readLine();
        if (flag) {
            if (line.length() <= 19) { //omit strings with data only
                //if in list : line number, address, data and it is all (without instruction) -
                //omit this string
                continue;
            }
            QByteArray lineArr = line.toLocal8Bit();
            const char *s = lineArr.constData();
            quint64 a, b, c;
            if (sscanf(s, "%llu %llx %llx", &a, &b, &c) == 3) {
                if (!(b == 0 && c == 0)) { //exclude 0 0
                    return b;
                }
            }
        } else {
            if (line.indexOf(mainLabel) != -1)
                flag = true;
        }
    }
    return -1;
}

void GAS::parseLstFile(QFile &lst, QVector<Assembler::LineNum> &lines, quint64 offset)
{
    bool inTextSection = false;
    QRegExp sectionTextRegExp("\\.text");
    QRegExp sectionBssRegExp("\\.bss");
    QRegExp sectionDataRegExp("\\.data");
    QTextStream lstStream(&lst);
    while (!lstStream.atEnd()) {
        QString line = lstStream.readLine();
        if (line.indexOf(sectionTextRegExp) != -1) {
            inTextSection = true;
        } else if (line.indexOf(sectionDataRegExp) != -1 || line.indexOf(sectionBssRegExp) != -1) {
            inTextSection = false;
        }
        if (line.length() <= 19) { //omit strings with data only
            //if in list : line number, address, data and it is all (without instruction) -
            //omit this string and subtract 1 from offset
            continue;
        }
        if (inTextSection) {
            QByteArray lineArr = line.toLocal8Bit();
            const char *s = lineArr.constData();
            quint64 a, b, c;
            if (sscanf(s, "%llu %llx %llx", &a, &b, &c) == 3){
                if (!(b == 0 && c == 0)) { //exclude 0 0
                    LineNum l;
                    l.numInCode = a;
                    l.numInMem = b + offset;
                    lines.append(l);
                }
            }
        }
    }
}

QString GAS::getStartText()
{
    if (isx86()) {
        #ifdef Q_OS_WIN32
            return QString(".text\n.global _main\n_main:\n    # write your code here\n    xorl  %eax, %eax\n    ret\n");
        #else
            return QString(".text\n.global main\nmain:\n    # write your code here\n    xorl  %eax, %eax\n    ret\n");
        #endif
    } else {
        return QString(".text\n.global main\nmain:\n    # write your code here\n    xorq  %rax, %rax\n    ret\n");
    }
}

void GAS::putDebugString(CodeEditor *code)
{
    //add : mov ebp, esp for making frame for correct debugging if this code has not been added yet
    int index = code->toPlainText().indexOf(QRegExp("CMAIN:|main:"));
    if (index != -1) {
        index = code->toPlainText().indexOf(QChar(':'), index);
        int intelIndex = code->toPlainText().lastIndexOf("intel_syntax", index + 1);
        int attIndex = code->toPlainText().lastIndexOf("att_syntax", index + 1);
        if (intelIndex == -1 || attIndex > intelIndex) { //AT&T syntax
            if (isx86()) {
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
            if (isx86()) {
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

QString GAS::getAssemblerOptions()
{
    QString options;
        if (isx86())
            options = "$SOURCE$ -o $PROGRAM.OBJ$ --32 -a=$LSTOUTPUT$";
        else
            options = "$SOURCE$ -o $PROGRAM.OBJ$ --64 -a=$LSTOUTPUT$";
    return options;
}

QString GAS::getLinkerOptions()
{
    QString options;
    if (isx86())
        options = "$PROGRAM.OBJ$ -g -o $PROGRAM$ -m32";
    else
        options = "$PROGRAM.OBJ$ -g -o $PROGRAM$ -m64";
    return options;
}

void GAS::fillHighligherRules(QVector<Assembler::HighlightingRule> &highlightingRules,
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
    foreach (QString pattern, keywordPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.pattern.setCaseSensitivity(Qt::CaseInsensitive);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
        //with prefix
        int index = pattern.length() - 2;
        pattern.insert(index, 'b');
        rule.pattern = QRegExp(pattern);
        rule.pattern.setCaseSensitivity(Qt::CaseInsensitive);
        highlightingRules.append(rule);
        pattern[index] = 'w';
        rule.pattern = QRegExp(pattern);
        rule.pattern.setCaseSensitivity(Qt::CaseInsensitive);
        highlightingRules.append(rule);
        pattern[index] = 'l';
        rule.pattern = QRegExp(pattern);
        rule.pattern.setCaseSensitivity(Qt::CaseInsensitive);
        highlightingRules.append(rule);
        pattern[index] = 'q';
        rule.pattern = QRegExp(pattern);
        rule.pattern.setCaseSensitivity(Qt::CaseInsensitive);
        highlightingRules.append(rule);
    }

    //memory
    rule.format = memoryFormat;
    QStringList memoryPatterns;
    memoryPatterns << "\\(" << "\\)";
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
        rule.pattern = QRegExp('%' + pattern);
        rule.pattern.setCaseSensitivity(Qt::CaseInsensitive);
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
    systemPatterns << "\\.\\w+\\b";
    foreach (const QString &pattern, systemPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.pattern.setCaseSensitivity(Qt::CaseInsensitive);
        highlightingRules.append(rule);
    }

    //quotations
    rule.pattern = QRegExp("\".*\"");
    rule.pattern.setMinimal(true);
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    rule.pattern = QRegExp("'.*'");
    rule.pattern.setMinimal(true);
    highlightingRules.append(rule);

    rule.pattern = QRegExp("`.*`");
    rule.pattern.setMinimal(true);
    highlightingRules.append(rule);

    //comments
    commentStartExpression = QRegExp("/\\*");
    commentEndExpression = QRegExp("[^\\\\]\\*/");
    rule.format = commentFormat;
    rule.pattern = QRegExp("#[^\n]*");
    rule.isComment = true;
    highlightingRules.append(rule);
    multiLineComments = true;
}
