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
 * If the developer wishes to add support for a new assember, please refer to the implimentations
 * of the NASM, MASM, FASM, and GAS assemblers, found in their respective header files.
 */

class Assembler : public QObject
{
    Q_OBJECT
public:
    struct LineNum {
         //! String number in code in SASM code area
        quint64 numInCode;
         //! Address of instruction in memory
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
        //! Pattern to highlight - WHICH PATTERN UNKNOWN
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

    virtual quint64 getMainOffset(QFile &lst, QString entryLabel) = 0;
    //get file with listing and name of entry label - main or start.
    //Should return offset of this label - number of string, where label is placed.

    virtual void parseLstFile(QFile &lst,
                              QVector<Assembler::LineNum> &lines,
                              quint64 offset) = 0;
    //should parse listing file lst and fill QVector lines with results of parsing.
    //offset - difference between program code in memory and in file.

    virtual void fillHighligherRules(QVector<Assembler::HighlightingRule> &highlightingRules,
                                     QList<QTextCharFormat *> &formats,
                                     bool &multiLineComments,
                                     QRegExp &commentStartExpression,
                                     QRegExp &commentEndExpression) = 0;
    //should fill QVector with highlighting rules.
    //Qlist with formats (see NASM, MASM, FASM, GAS as examples). commentStartExpression - /* in C++ for example,
    //commentEndExpression - */ in C++ for example - expressions for multi line highlighting - workes if multiLineComments == true.
     //! Return the default start text (default project code)
    virtual QString getStartText() = 0;
    //! Puts the debug string that makes frame (mov ebp, esp)
    virtual void putDebugString(CodeEditor *code) = 0;
     //! Returns the default assembler options
    virtual QString getAssemblerOptions() = 0;
     //! Returns the default linker options
    virtual QString getLinkerOptions() = 0;
    bool isx86();

signals:

public slots:

};

#endif // ASSEMBLER_H
