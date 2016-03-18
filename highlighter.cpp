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

 #include "highlighter.h"

/**
 * @file highlighter.cpp
 * The code highlighting features are defined here.
 */

 Highlighter::Highlighter(Assembler *assembler, QTextDocument *parent)
     : QSyntaxHighlighter(parent)
 {
     QTextCharFormat keywordFormat;
     QTextCharFormat quotationFormat;
     QTextCharFormat registerFormat;
     QTextCharFormat labelFormat;
     QTextCharFormat labelWithDotFormat;
     QTextCharFormat memoryFormat;
     QTextCharFormat systemFormat;
     QTextCharFormat numberFormat;
     QTextCharFormat iomacrosFormat;
     QList<QColor> defaultColors;
     QList<QString> names;
     QList<QTextCharFormat *> formats;
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
     for (int i = 0; i < formats.size(); i++) {
         formats[i]->setForeground(settings.value(names[i] + "color", defaultColors[i]).value<QColor>());
         formats[i]->setBackground(settings.value(names[i] + "colorbg", QPalette().color(QPalette::Base)).value<QColor>());
         if (settings.value(names[i] + "bold", (i == 0) ? true : false).toBool())
             formats[i]->setFontWeight(QFont::Bold);
         formats[i]->setFontItalic(settings.value(names[i] + "italic", false).toBool());
     }
     labelWithDotFormat.setForeground(settings.value("fontcolor", QPalette().color(QPalette::WindowText)).value<QColor>());
     formats << &labelWithDotFormat;
     assembler->fillHighligherRules(highlightingRules, formats, multiLineComments, commentStartExpression, commentEndExpression);
 }

void Highlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            if (rule.isComment) {
                if (!isCommentInQuote(text, index)) {
                    setFormat(index, length, rule.format);
                    index = expression.indexIn(text, index + length);
                } else {
                    index = expression.indexIn(text, index + 1);
                }
             } else {
                setFormat(index, length, rule.format);
                index = expression.indexIn(text, index + length);
             }
         }
     }
     if (multiLineComments) {
         setCurrentBlockState(0);
         int startIndex = 0;
         if (previousBlockState() != 1) {
             startIndex = commentStartExpression.indexIn(text);
             if (startIndex != -1 && isCommentInQuote(text, startIndex)) {
                 startIndex = -1;
             }
         }
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

bool Highlighter::isCommentInQuote(const QString &text, int index)
{
    bool inQuote = false;
    char quote = 0;
    for (int i = 0; i < index; ++i) {
        char c = text[i].toLatin1();
        switch (c) {
        case '"':
        case '\'':
        case '`':
            if (!inQuote) {
                inQuote = true;
                quote = c;
            } else if (quote == c) {
                inQuote = false;
            }
        }
    }
    return inQuote;
}

 Highlighter::~Highlighter()
 {
     highlightingRules.clear();
 }
