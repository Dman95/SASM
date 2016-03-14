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

#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <QObject>
#include <QMap>
#include <QList>
#include <QRegExp>
#include <QFile>
#include <QTextStream>
#include <QVector>
#include <QTextCharFormat>
#include <QSettings>
#include <QPalette>
#include "common.h"
#include "codeeditor.h"

/**
 * @file assembler.h
 * Base class for creating assembler instances
 */

/*! \brief This is the base class that all assemblers inherit
 *
 *  The Assembler class contains functions which can be used to retrieve assember specific
 * parameters such as the linker & assembler location, the default program text, and
 * user specified options.
 *
 */

class Assembler : public QObject
{
    Q_OBJECT
public:
    struct LineNum {
         //! String number in code in SASM code area ->What does string number in code mean?
        quint64 numInCode;
         //! Address of instruction in memory -> which instruction? Of the user's?
        quint64 numInMem;
        bool operator ==(const LineNum& ln)
        {
            return ln.numInCode == numInCode;
        }
    };
    struct HighlightingRule
    {
        HighlightingRule() : isComment(false) {
        }
        //! Pattern to highlight
        QRegExp pattern;
        //! Whighlighting format UNKNOWN FILL THIS IN
        QTextCharFormat format;
        bool isComment;
    };
    bool x86;

    //! Assembler constructor. Note that this is the stage where x86 is determined.
    explicit Assembler(bool x86, QObject *parent = 0);

     //! Return the default path to the assembler.
    virtual QString getAssemblerPath() = 0;

    //! Returns the default path to the linker.
    virtual QString getLinkerPath() = 0;

    //! get file with listing and name of entry label - main or start.
    //! Returns the offset of this label - number of strings and where the label is placed.
    virtual quint64 getMainOffset(QFile &lst, QString entryLabel) = 0;

    //! Parses the listing file lst and fills QVector lines with results of parsing.
    //! offset - difference between program code in memory and in file.
    virtual void parseLstFile(QFile &lst,
                              QVector<Assembler::LineNum> &lines,
                              quint64 offset) = 0;

    //! Fills QVector with the highlighting rules.
    //! Qlist with formats (see NASM, MASM, FASM, GAS as examples). commentStartExpression - /* in C++ for example,
    //! commentEndExpression - */ in C++ for example - expressions for multi line highlighting - workes if multiLineComments == true.
    virtual void fillHighligherRules(QVector<Assembler::HighlightingRule> &highlightingRules,
                                     QList<QTextCharFormat *> &formats,
                                     bool &multiLineComments,
                                     QRegExp &commentStartExpression,
                                     QRegExp &commentEndExpression) = 0;

     //! Return the default start text (default project code)
    virtual QString getStartText() = 0;

    //! Puts the debug string that makes frame (mov ebp, esp)
    virtual void putDebugString(CodeEditor *code) = 0;

     //! Returns the default assembler options
    virtual QString getAssemblerOptions() = 0;

     //! Returns the default linker options
    virtual QString getLinkerOptions() = 0;

    //! Determines if the system is x86
    bool isx86();

signals:

public slots:

};

#endif // ASSEMBLER_H














/*! \mainpage SASM Dev Guide
 *
 * \section intro_sec Introduction
 *
 * Below you will find what and where to modify appropriate header and source files when you want to add a feature.
 *
 * \section section1 Commenting
 *
 * When commenting use doxygen's syntax. Place the comment above the intended line. After adding the feature, run
 * doxygen.exe configfile to update the documentation. Be sure to double check the documentation to ensure the
 * added comments were parsed  correctly.
 *
 * \section section2 Adding Assembler Support
 *
 * Adding support is a relatively straight forward process. Each supported assembler is a derived Assembler and
 * has its own header and cpp file.
 *
 * \subsection step1 Step 1: Creating the header and cpp
 *
 * The first step is to create the new header and cpp file for the assembler. These should be named all lowercase without gaps
 *
 * \subsection step2 Step 2: Creating the assembler class
 *
 * The decleration of the class may best be discussed in light of already supported assemblers. Take for example, NASM. The NASM
 * class is defined as:
 * class NASM : public Assembler
 *
 * The generic definition is
 * class YOURASSEMBLER : public Assembler
 *
 * The variables and methods of YOURASSEMBLER should be the virtual methods of Assembler. If you are unsure what to add, refer to
 * the already supported assemler classes. You may copy and paste them.
 *
 * \subsection step3 Step 3: Adding it to the Build Options
 *
 * You should hopefully know enough QT to be able to add form controls. The code for modifying the build menu can be found in
 * mainwindow.cpp. Refer to its documentation for more reference on where to add/modify code.
 *
 * \section section3 Adding Language Support
 *
 *
 * \subsection Help File
 *
 * The help file, __.xxx, needs to be translated and saved into a new xx.xxx file. The
 * void MainWindow::openHelp()
 * must be modified to support the added language. This is done by adding another if statement.
 *
 * \subsection startText
 *
 * The default assembler skeleton needs to have its comments translated.
 */







