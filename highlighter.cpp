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

 #include "highlighter.h"

 Highlighter::Highlighter(QTextDocument *parent)
     : QSyntaxHighlighter(parent)
 {
     //setting up text formats
     names << "keywords" << "registers" << "numbers" << "memory" <<
                   "labels" << "comments" << "system" << "iomacro" <<
                   "quotation";
     defaultColors << QColor(Qt::blue) << QColor(153, 0, 204) << //according to colorNames
                      QColor(255, 122, 0) << QColor(0, 128, 255) <<
                      QColor(128, 0, 0) << QColor(Qt::darkGreen) <<
                      QColor(Qt::darkCyan) << QColor(Qt::blue) <<
                      QColor(128, 128, 128);

     formats << &keywordFormat << &registerFormat << &numberFormat << &memoryFormat <<
                &labelFormat << &commentFormat << &systemFormat << &iomacrosFormat <<
                &quotationFormat;

     QSettings settings("SASM Project", "SASM");
     QString assembler = settings.value("assembler", QString("NASM")).toString();
     for (int i = 0; i < formats.size(); i++) {
         formats[i]->setForeground(settings.value(names[i] + "color", defaultColors[i]).value<QColor>());
         formats[i]->setBackground(settings.value(names[i] + "colorbg", QPalette().color(QPalette::Base)).value<QColor>());
         if (settings.value(names[i] + "bold", (i == 0) ? true : false).toBool())
             formats[i]->setFontWeight(QFont::Bold);
         formats[i]->setFontItalic(settings.value(names[i] + "italic", false).toBool());
     }
     labelWithDotFormat = format(0);

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
     if (assembler == "GAS") {
         foreach (QString pattern, keywordPatterns) {
             rule.format = keywordFormat;
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
     }

     //IO macroses
     if (assembler == "NASM") {
         rule.format = iomacrosFormat;
         QStringList macrosPatterns;
         macrosPatterns << "\\bPRINT_DEC\\b" << "\\bPRINT_HEX\\b" <<
                           "\\bPRINT_CHAR\\b" << "\\bPRINT_STRING\\b" <<
                           "\\bNEWLINE\\b" << "\\bPRINT_UDEC\\b" <<
                           "\\bGET_UDEC\\b" << "\\bGET_DEC\\b" <<
                           "\\bGET_HEX\\b" << "\\bGET_CHAR\\b" <<
                           "\\bGET_STRING\\b" << "\\bCMAIN\\b" << "\\bCEXTERN\\b";
         foreach (const QString &pattern, macrosPatterns) {
             rule.pattern = QRegExp(pattern);
             rule.pattern.setCaseSensitivity(Qt::CaseSensitive);
             highlightingRules.append(rule);
         }
     }

     //memory
     rule.format = memoryFormat;
     QStringList memoryPatterns;
     if (assembler == "NASM") {
         rule.pattern = QRegExp("\\[");
         highlightingRules.append(rule);
         rule.pattern = QRegExp("\\]");
         highlightingRules.append(rule);
         memoryPatterns << "\\bresb\\b" << "\\bresw\\b" << "\\bresd\\b" <<
                           "\\bresq\\b" << "\\brest\\b" << "\\breso\\b" <<
                           "\\bresy\\b" << "\\bddq\\b" << "\\bresdq\\b" <<
                           "\\bdb\\b" << "\\bdw\\b" << "\\bdd\\b" <<
                           "\\bdq\\b" << "\\bdt\\b" << "\\bdo\\b" <<
                           "\\bdy\\b" << "\\bequ\\b" <<
                           "\\bbyte[\\s\\[]" << "\\bword[\\s\\[]" <<
                           "\\bdword[\\s\\[]" << "\\bqword[\\s\\[]" <<
                           "\\btword[\\s\\[]" << "\\boword[\\s\\[]" <<
                           "\\yword[\\s\\[]" << "\\bDEFAULT\\b" <<
                           "\\bABS\\b" << "\\bREL\\b";
     } else if (assembler == "GAS") {
         memoryPatterns << "\\(" << "\\)";
     }
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
     rule.pattern = QRegExp("[\\-\\+]?\\d+[bod]?\\b");
     highlightingRules.append(rule);
     rule.pattern = QRegExp("0[bo]\\d+\\b");
     highlightingRules.append(rule);
     rule.pattern = QRegExp("[0-9A-Fa-f]+h\\b"); //hexadecimal notation
     highlightingRules.append(rule);
     rule.pattern = QRegExp("0[xh][0-9A-Fa-f]+\\b"); //hexadecimal notation
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
     if (assembler == "GAS") {
         foreach (const QString &pattern, registerPatterns) {
             rule.format = registerFormat;
             rule.pattern = QRegExp('%' + pattern);
             rule.pattern.setCaseSensitivity(Qt::CaseInsensitive);
             highlightingRules.append(rule);
         }
     }


     //.labels and numbers with point
     rule.pattern = QRegExp("\\.[^\\s:]+[^:]");
     rule.format = labelWithDotFormat;
     highlightingRules.append(rule);
     rule.pattern = QRegExp("\\.[^\\s:]+:");
     rule.format = labelFormat;
     highlightingRules.append(rule);
     rule.pattern = QRegExp("[\\-\\+]?\\d+\\.\\d+\\b");
     rule.format = numberFormat;
     highlightingRules.append(rule);

     //system instructions and preprocessor commands
     rule.format = systemFormat;
     QStringList systemPatterns;
     if (assembler == "NASM") {
         systemPatterns << "\\btimes\\b" << "\\bsection\\b" << "\\.bss\\b" <<
                           "\\.text\\b" << "\\.data\\b" <<
                           "\\bglobal\\b" << "\\.rodata\\b" <<
                           "\\bextern\\b" <<
                           "\\%arg\\b" << "\\%assign\\b" << "\\%clear\\b" <<      //macro
                           "\\%comment\\b" << "\\%define\\b" << "\\%defstr\\b" <<
                           "\\%deftok\\b" << "\\%depend\\b" <<
                           "\\%line\\b" << "\\%local\\b" << "\\%macro\\b" <<
                           "\\%n\\b" << "\\%pathsearch\\b" << "\\%pop\\b" <<
                           "\\%push\\b" << "\\%rep\\b" << "\\%repl\\b" <<
                           "\\%rotate\\b" << "\\%stacksize\\b" << "\\%strcat\\b" <<
                           "\\%strlen\\b" << "\\%substr\\b" << "\\%undef\\b" <<
                           "\\%unmacro\\b" << "\\%use\\b" << "\\%warning\\b" <<
                           "\\%xdefine\\b" << "\\%endcomment\\b" << "\\%endif\\b" <<
                           "\\%endmacro\\b" << "\\%endrep\\b" << "\\%error\\b" <<
                           "\\%exitrep\\b" << "\\%fatal\\b" << "\\%idefine\\b" <<
                           "\\%else\\b" << "\\%imacro\\b" << "\\%include\\b" <<
                           "\\%if\\b" << "\\%ifctx\\b" << "\\%ifdef\\b" <<           //ifs
                           "\\%ifempty\\b" << "\\%ifenv\\b" << "\\%ifidn\\b" <<
                           "\\%ifidni\\b" << "\\%ifmacro\\b" << "\\%ifstr\\b" <<
                           "\\%iftoken\\b" << "\\%ifnum\\b" << "\\%ifid\\b" <<
                           "\\%elif\\b" << "\\%elifctx\\b" << "\\%elifdef\\b" <<     //elifs
                           "\\%elifempty\\b" << "\\%elifenv\\b" << "\\%elifidn\\b" <<
                           "\\%elifidni\\b" << "\\%elifmacro\\b" << "\\%elifstr\\b" <<
                           "\\%eliftoken\\b" << "\\%elifnum\\b" << "\\%elifid\\b" <<
                           "\\%ifn\\b" << "\\%ifnctx\\b" << "\\%ifndef\\b" <<           //ifns
                           "\\%ifnempty\\b" << "\\%ifnenv\\b" << "\\%ifnidn\\b" <<
                           "\\%ifnidni\\b" << "\\%ifnmacro\\b" << "\\%ifnstr\\b" <<
                           "\\%ifntoken\\b" << "\\%ifnnum\\b" << "\\%ifnid\\b" <<
                           "\\%elifn\\b" << "\\%elifnctx\\b" << "\\%elifndef\\b" <<     //elifns
                           "\\%elifnempty\\b" << "\\%elifnenv\\b" << "\\%elifnidn\\b" <<
                           "\\%elifnidni\\b" << "\\%elifnmacro\\b" << "\\%elifnstr\\b" <<
                           "\\%elifntoken\\b" << "\\%elifnnum\\b" << "\\%elifnid\\b";
     } else if (assembler == "GAS") {
         systemPatterns << "\\.\\w+\\b";
     }
     foreach (const QString &pattern, systemPatterns) {
         rule.pattern = QRegExp(pattern);
         rule.pattern.setCaseSensitivity(Qt::CaseInsensitive);
         highlightingRules.append(rule);
     }


     //quotations
     rule.pattern = QRegExp("\".*\"");
     rule.format = quotationFormat;
     highlightingRules.append(rule);

     rule.pattern = QRegExp("'.*'");
     highlightingRules.append(rule);

     rule.pattern = QRegExp("`.*`");
     highlightingRules.append(rule);

     //comments
     commentStartExpression = QRegExp("/\\*");
     commentEndExpression = QRegExp("\\*/");
     if (assembler == "NASM") {
         rule.pattern = QRegExp(";[^\n]*");
         rule.format = commentFormat;
         highlightingRules.append(rule);
         multiLineComments = false;
     } else if (assembler == "GAS") {
         rule.format = commentFormat;
         rule.pattern = QRegExp("#[^\n]*");
         highlightingRules.append(rule);
         multiLineComments = true;
     }
 }

 void Highlighter::highlightBlock(const QString &text)
 {
     foreach (const HighlightingRule &rule, highlightingRules) {
         QRegExp expression(rule.pattern);
         int index = expression.indexIn(text);
         while (index >= 0) {
             int length = expression.matchedLength();
             setFormat(index, length, rule.format);
             index = expression.indexIn(text, index + length);
         }
     }

     if (multiLineComments) {
         setCurrentBlockState(0);
         int startIndex = 0;
         if (previousBlockState() != 1)
             startIndex = commentStartExpression.indexIn(text);
         while (startIndex >= 0) {
             int endIndex = commentEndExpression.indexIn(text, startIndex);
             int commentLength;
             if (endIndex == -1) {
                 setCurrentBlockState(1);
                 commentLength = text.length() - startIndex;
             } else {
                 commentLength = endIndex - startIndex
                                 + commentEndExpression.matchedLength();
             }
             setFormat(startIndex, commentLength, commentFormat);
             startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
         }
     }
 }

 Highlighter::~Highlighter()
 {
     highlightingRules.clear();
 }
