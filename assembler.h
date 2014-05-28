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

class Assembler : public QObject //Abstract class
{
    Q_OBJECT
public:
    struct LineNum {
        quint64 numInCode;
        quint64 numInMem;
        bool operator ==(const LineNum& ln)
        {
            return ln.numInCode == numInCode;
        }
    };
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    bool x86;
    explicit Assembler(bool x86, QObject *parent = 0);
    virtual QString getAssemblerPath() = 0;
    virtual QString getLinkerPath() = 0;
    virtual quint64 getMainOffset(QFile &lst, QString entryLabel) = 0;
    virtual void parseLstFile(QFile &lst, QVector<Assembler::LineNum> &lines, bool ioIncIncluded, quint64 ioIncSize, quint64 offset) = 0;
    virtual void fillHighligherRules(QVector<Assembler::HighlightingRule> &highlightingRules,
                                     QList<QTextCharFormat *> &formats,
                                     bool &multiLineComments,
                                     QRegExp &commentStartExpression,
                                     QRegExp &commentEndExpression) = 0;
    virtual QString getStartText() = 0;
    virtual void putDebugString(CodeEditor *code) = 0;
    virtual QString getAssemblerOptions() = 0;
    virtual QString getLinkerOptions() = 0;
    bool isx86();

signals:
    
public slots:
    
};

#endif // ASSEMBLER_H
