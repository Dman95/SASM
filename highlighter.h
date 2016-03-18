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

#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QSettings>
#include <QPalette>
#include <QTextDocument>
#include "assembler.h"

/**
 * @file highlighter.h
 * Defines the highlighting class.
 */

/*! \brief This class defines the rules of syntax highlighting.
 *
 *  Because some assemblers use different keywords, it is
 *  essential to distinguish between rules.
 *  Each rule set is defined as a Highlighter instance.
 */

class Highlighter : public QSyntaxHighlighter
{
public:
     Highlighter(Assembler *assembler, QTextDocument *parent = 0);
     ~Highlighter();

protected:
     void highlightBlock(const QString &text);

private:
     typedef Assembler::HighlightingRule HighlightingRule;
     QVector<HighlightingRule> highlightingRules;

     bool isCommentInQuote(const QString &text, int index);

     QRegExp commentStartExpression;
     QRegExp commentEndExpression;
     bool multiLineComments;
     QTextCharFormat commentFormat;
};

#endif // HIGHLIGHTER_H
