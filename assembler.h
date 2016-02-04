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

class Assembler : public QObject //Abstract class. Need to implement to add new assembler in SASM.
	//See examples of implementation in classes NASM, MASM, FASM and GAS.
{
	Q_OBJECT

public:
	struct LineNum {
		quint64 numInCode; //string number in code in SASM code area
		quint64 numInMem; //address of instruction in memory
		bool
		operator ==(const LineNum& ln) {
			return ln.numInCode == numInCode;
		}
	};

	struct HighlightingRule {
		HighlightingRule() : isComment(false) { }

		QRegExp pattern; //pattern to highlight
		QTextCharFormat format; //highlighting format
		bool isComment;
	};

	bool x86;
	explicit
	Assembler(bool x86, QObject* parent = 0);

	virtual QString
	getAssemblerPath() = 0; //should return default path to assembler
	virtual QString
	getLinkerPath() = 0; //should return default path to linker

	virtual quint64
	getMainOffset(QFile& lst, QString entryLabel) = 0;
	//get file with listing and name of entry label - main or start.
	//Should return offset of this label - number of string, where label is placed.

	virtual void parseLstFile(QFile& lst,
	                          QVector<Assembler::LineNum>& lines,
	                          quint64 offset) = 0;
	//should parse listing file lst and fill QVector lines with results of parsing.
	//offset - difference between program code in memory and in file.

	virtual void fillHighligherRules(QVector<Assembler::HighlightingRule>& highlightingRules,
	                                 QList<QTextCharFormat *>& formats,
	                                 bool& multiLineComments,
	                                 QRegExp& commentStartExpression,
	                                 QRegExp& commentEndExpression) = 0;
	//should fill QVector with highlighting rules.
	//Qlist with formats (see NASM, MASM, FASM, GAS as examples). commentStartExpression - /* in C++ for example,
	//commentEndExpression - */ in C++ for example - expressions for multi line highlighting - workes if multiLineComments == true.

	virtual QString
	getStartText() = 0; //should return default start text
	virtual void
	putDebugString(CodeEditor* code) = 0; //should put debug string, that makes frame (mov ebp, esp)
	virtual QString
	getAssemblerOptions() = 0; //should return default assembler options
	virtual QString
	getLinkerOptions() = 0; //should return default linker options
	bool
	isx86();

	signals:

public slots:

};

#endif // ASSEMBLER_H


