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

class Assembler : public QObject	//Abstract class. Need to implement to add new assembler in SASM.
	// See examples of implementation in classes NASM, MASM, FASM and GAS.
{
	Q_OBJECT

public:

	/**
	 * Struct: LineNum
	 *
	 * @struct LineNum
	 *
	 * @brief A line number.
	 *
	 * @date 08:43 04 February 2016
	 */

	struct LineNum {
		/** @brief string number in code in SASM code area. */
		quint64 numInCode;
		/** @brief address of instruction in memory. */
		quint64 numInMem;
		bool

		/**
		 * Operator: ==
		 *
		 * @fn operator==(const LineNum& ln)
		 *
		 * @brief Equality operator.
		 *
		 * @date 08:44 04 February 2016
		 *
		 * @param ln The line.
		 */

		operator ==(const LineNum& ln) {
			return ln.numInCode == numInCode;
		}
	};

	/**
	 * Struct: HighlightingRule
	 *
	 * @struct HighlightingRule
	 *
	 * @brief A highlighting rule.
	 *
	 * @date 08:44 04 February 2016
	 */

	struct HighlightingRule {
		HighlightingRule() : isComment(false) { }

		/** @brief pattern to highlight. */
		QRegExp pattern;
		/** @brief highlighting format. */
		QTextCharFormat format;
		/** @brief true if this object is comment. */
		bool isComment;
	};

	/** @brief true to 86. */
	bool x86;

	explicit

	/**
	 * Constructor: Assembler
	 *
	 * @fn Assembler::Assembler(bool x86, QObject* parent = 0);
	 *
	 * @brief Constructor.
	 *
	 * @date 08:44 04 February 2016
	 *
	 * @param x86			  true to 86.
	 * @param [in,out] parent (Optional) If non-null, the parent.
	 */

	Assembler(bool x86, QObject* parent = 0);

	/**
	 * Method: getAssemblerPath
	 *
	 * @fn virtual QString Assembler::getAssemblerPath() = 0;
	 *
	 * @brief Gets assembler path.
	 *
	 * @date 08:44 04 February 2016
	 *
	 * @return The assembler path.
	 */

	virtual QString
	getAssemblerPath() = 0; //should return default path to assembler

	/**
	 * Method: getLinkerPath
	 *
	 * @fn virtual QString Assembler::getLinkerPath() = 0;
	 *
	 * @brief Gets linker path.
	 *
	 * @date 08:45 04 February 2016
	 *
	 * @return The linker path.
	 */

	virtual QString
	getLinkerPath() = 0; //should return default path to linker

	/**
	 * Method: getMainOffset
	 *
	 * @fn virtual quint64 Assembler::getMainOffset(QFile& lst, QString entryLabel) = 0;
	 *
	 * @brief Gets main offset.
	 *
	 * @date 08:45 04 February 2016
	 *
	 * @param [in,out] lst The list.
	 * @param entryLabel   The entry label.
	 *
	 * @return The main offset.
	 */

	virtual quint64
	getMainOffset(QFile& lst, QString entryLabel) = 0;

	/**
	 * Method: parseLstFile
	 *
	 * @fn virtual void Assembler::parseLstFile(QFile& lst, QVector<Assembler::LineNum>& lines, quint64 offset) = 0;
	 *
	 * @brief get file with listing and name of entry label - main or start. Should return offset of
	 * 		  this label - number of string, where label is placed.
	 *
	 * @date 08:45 04 February 2016
	 *
	 * @param [in,out] lst   The list.
	 * @param [in,out] lines The lines.
	 * @param offset		 The offset.
	 */

	virtual void parseLstFile(QFile& lst,
	                          QVector<Assembler::LineNum>& lines,
	                          quint64 offset) = 0;

	/**
	 * Method: fillHighligherRules
	 *
	 * @fn virtual void Assembler::fillHighligherRules(QVector<Assembler::HighlightingRule>& highlightingRules, QList<QTextCharFormat *>& formats, bool& multiLineComments, QRegExp& commentStartExpression, QRegExp& commentEndExpression) = 0;
	 *
	 * @brief should parse listing file lst and fill QVector lines with results of parsing. offset -
	 * 		  difference between program code in memory and in file.
	 *
	 * @date 08:45 04 February 2016
	 *
	 * @param [in,out] highlightingRules	  The highlighting rules.
	 * @param [in,out] formats				  [in,out] If non-null, the formats.
	 * @param [in,out] multiLineComments	  The multi line comments.
	 * @param [in,out] commentStartExpression The comment start expression.
	 * @param [in,out] commentEndExpression   The comment end expression.
	 */

	virtual void fillHighligherRules(QVector<Assembler::HighlightingRule>& highlightingRules,
	                                 QList<QTextCharFormat *>& formats,
	                                 bool& multiLineComments,
	                                 QRegExp& commentStartExpression,
	                                 QRegExp& commentEndExpression) = 0;

	/**
	 * Method: getStartText
	 *
	 * @fn virtual QString Assembler::getStartText() = 0;
	 *
	 * @brief should fill QVector with highlighting rules. Qlist with formats (see NASM, MASM, FASM,
	 * 		  GAS as examples). commentStartExpression - /* in C++ for example,
	 * 		  commentEndExpression -  in C++ for example - expressions for multi line
	 * 		  highlighting - workes if multiLineComments == true.
	 *
	 * @date 08:45 04 February 2016
	 *
	 * @return The start text.
	 */

	virtual QString
	getStartText() = 0; //should return default start text

	/**
	 * Method: putDebugString
	 *
	 * @fn virtual void Assembler::putDebugString(CodeEditor* code) = 0;
	 *
	 * @brief Puts debug string.
	 *
	 * @date 08:45 04 February 2016
	 *
	 * @param [in,out] code If non-null, the code.
	 */

	virtual void
	putDebugString(CodeEditor* code) = 0; //should put debug string, that makes frame (mov ebp, esp)

	/**
	 * Method: getAssemblerOptions
	 *
	 * @fn virtual QString Assembler::getAssemblerOptions() = 0;
	 *
	 * @brief Gets assembler options.
	 *
	 * @date 08:45 04 February 2016
	 *
	 * @return The assembler options.
	 */

	virtual QString
	getAssemblerOptions() = 0; //should return default assembler options

	/**
	 * Method: getLinkerOptions
	 *
	 * @fn virtual QString Assembler::getLinkerOptions() = 0;
	 *
	 * @brief Gets linker options.
	 *
	 * @date 08:46 04 February 2016
	 *
	 * @return The linker options.
	 */

	virtual QString
	getLinkerOptions() = 0; //should return default linker options

	/**
	 * Method: isx86
	 *
	 * @fn bool Assembler::isx86();
	 *
	 * @brief Query if this object is x86.
	 *
	 * @date 08:46 04 February 2016
	 *
	 * @return true if it succeeds, false if it fails.
	 */

	bool
	isx86();

	signals:

public slots:

};

#endif // ASSEMBLER_H


