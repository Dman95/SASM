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

#include "codeeditor.h"

CodeEditor::CodeEditor(QWidget *parent) : RuQPlainTextEdit(parent)
{
    lineNumberArea = new LineNumberArea(this);

    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
    currentDebugLine = -1;
}

int CodeEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;

    return space;
}

void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    lastRect = rect;
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void CodeEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(232,232,255);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), QColor(240,240,240));

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            if (blockNumber + 1 == currentDebugLine)
                painter.setPen(Qt::red);
            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}

void CodeEditor::updateDebugLine(int number)
{
    currentDebugLine = number;
    lineNumberArea->scroll(0, 1);
    lineNumberArea->scroll(0, -1);
}

void CodeEditor::putTab()
{
    QTextCursor cursor = this->textCursor();
    if (cursor.selectionEnd() - cursor.selectionStart() <= 0) {
        this->insertPlainText("    ");
    } else {
        QString selected =
                cursor.selectedText().replace(QString(QChar::ParagraphSeparator),
                                              QString(QChar::ParagraphSeparator) + QString("    "));
        selected.insert(0, QString("    "));
        cursor.removeSelectedText();
        cursor.insertText(selected);
    }
}

void CodeEditor::deleteTab()
{
    QTextCursor cursor = this->textCursor();
    if (cursor.selectionEnd() - cursor.selectionStart() <= 0) {
        //delete 4 spaces (tab)
        cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, 4);
        QString selected = cursor.selectedText();
        if (selected.left(4) == QString("    "))
            cursor.deletePreviousChar();
    } else {
        QString selected =
                cursor.selectedText().replace(QString(QChar::ParagraphSeparator) + QString("    "),
                                              QString(QChar::ParagraphSeparator));
        cursor.removeSelectedText();
        if (selected.left(4) == QString("    "))
            cursor.insertText(selected.right(selected.length() - 4));
        else
            cursor.insertText(selected);
    }
}

void CodeEditor::keyPressEvent(QKeyEvent *e)
{
    QString curString = this->textCursor().block().text();
    int indentWidth = -1;
    switch (e->key()) {
    case (Qt::Key_Tab) :
        if (e->modifiers() & Qt::ControlModifier)
            deleteTab();
        else
            putTab();
        break;
    case (Qt::Key_Enter) :
    case (Qt::Key_Return) :
        while (curString[++indentWidth] == ' ');
        QPlainTextEdit::keyPressEvent(e);
        for (int i = 0; i < indentWidth; i++)
            this->insertPlainText(" ");
        break;
    case (Qt::Key_Q) :
        if ((e->modifiers() & Qt::ShiftModifier) && (e->modifiers() & Qt::ControlModifier))
            this->commentSelectedCode();
        else
            QPlainTextEdit::keyPressEvent(e);
        break;
    case (Qt::Key_A) :
        if ((e->modifiers() & Qt::ShiftModifier) && (e->modifiers() & Qt::ControlModifier))
            this->uncommentSelectedCode();
        else
            QPlainTextEdit::keyPressEvent(e);
        break;
    default:
        QPlainTextEdit::keyPressEvent(e);
    }
}

CodeEditor::~CodeEditor()
{
    delete lineNumberArea;
}
