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

#ifndef FASM_H
#define FASM_H

#include <QProcess>
#include <QLinkedList>
#include "assembler.h"

/**
 * @file fasm.h
 * Defines the FASM assembler
 */

/*! \brief This class defines the behavior for the FASM assembler.
 *
 *
 */

class FASM : public Assembler
{
    Q_OBJECT
public:
    explicit FASM(bool x86, QObject *parent = 0);
    QString getAssemblerPath();
    QString getLinkerPath();
    quint64 getMainOffset(QFile &lstOut, QString entryLabel);
    void parseLstFile(QFile &lstOut, QVector<Assembler::LineNum> &lines, quint64 offset);
    void fillHighligherRules(QVector<Assembler::HighlightingRule> &highlightingRules,
                             QList<QTextCharFormat *> &formats,
                             bool &multiLineComments,
                             QRegExp &commentStartExpression,
                             QRegExp &commentEndExpression);
    QString getStartText();
    void putDebugString(CodeEditor *code);
    QString getAssemblerOptions();
    QString getLinkerOptions();
    QString getListingFilePath(QFile &lstOut);

signals:

public slots:

};

#endif // FASM_H
