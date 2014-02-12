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

CodeEditor::CodeEditor(QWidget *parent, bool withBeakpoints) :
    RuQPlainTextEdit(parent), debugImage(":/images/debugLine.png"),
    breakpointImage(":/images/breakpoint.png")
{
    hasBreakpoints = withBeakpoints;
    prevBlockCount = -1;
    this->setDebugMode(false);
    debugAreaWidth = 3 + debugImage.width() + 1;
    this->setWordWrapMode(QTextOption::NoWrap);
    firstTopMargin = contentOffset().y();
    lineNumberArea = new LineNumberArea(this);

    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));
    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(shiftBreakpoints(int)));

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

    int space = fontMetrics().width(QLatin1Char('9')) * digits + debugAreaWidth;

    return space;
}

void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());
        //indirectly invokes CodeEditor::lineNumberAreaPaintEvent() throw virtual LineNumberArea::paintEvent()

    if (rect.contains(viewport()->rect())) //viewport - visible part of widget
        updateLineNumberAreaWidth(0);
}

void CodeEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    //paint on line number area
    QPainter painter(lineNumberArea);
    QSettings settings("SASM Project", "SASM");
    painter.fillRect(event->rect(), settings.value("linenumberpanelcolor", this->palette().color(QPalette::Window)).value<QColor>());

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    //set debugAreaWidth before drawing
    debugAreaWidth = 3 + debugImage.width() + 1; //left margin + arrow width + right margin
    updateLineNumberAreaWidth(0);

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(settings.value("fontcolor", this->palette().color(QPalette::Text)).value<QColor>());
            painter.drawText(0, top, lineNumberArea->width() - debugAreaWidth, fontMetrics().height(),
                             Qt::AlignRight, number);

            if (blockNumber + 1 == currentDebugLine)
                //blocks counting starts with 0, line number is equivalent (block number + 1)
                //if QTextOption::NoWrap is not set, lines count - visible lines, block count - lines divided by '\n'
                painter.drawPixmap(lineNumberArea->width() - debugAreaWidth + 3,
                                   top + fontMetrics().height() / 2 - debugImage.height() / 2,
                                   debugImage.width(), debugImage.height(), debugImage);
            if (breakpoints.contains(blockNumber + 1) && hasBreakpoints)
                painter.drawPixmap(lineNumberArea->width() - debugAreaWidth + 3,
                                   top + fontMetrics().height() / 2 - breakpointImage.height() / 2,
                                   breakpointImage.width(), breakpointImage.height(), breakpointImage);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}

void CodeEditor::lineNumberAreaMousePressEvent(QMouseEvent *event)
{
    if (hasBreakpoints) {
        //if mouse click has been made - add breakpoint
        //counting line number
        int lineNumber = 0;
        int sumHeight = 0;
        QTextBlock block = this->firstVisibleBlock();
        while (block.isValid() && event->y() > sumHeight) {
            sumHeight += blockBoundingGeometry(block).height();
            block = block.next();
            lineNumber++;
        }
        lineNumber += this->verticalScrollBar()->value(); //+ invisible lines

        if (lineNumber <= this->document()->lineCount()) {
            //blocks counting starts with 0
            lineNumber = document()->findBlockByLineNumber(lineNumber - 1).blockNumber() + 1; //line number to paint

            //add or remove line number in list
            if (breakpoints.contains(lineNumber)) {
                breakpoints.removeOne(lineNumber);
                emit breakpointsChanged(lineNumber, false);
            } else {
                breakpoints.append(lineNumber);
                emit breakpointsChanged(lineNumber, true);
            }

            repaintLineNumberArea();
        }
    }
}

void CodeEditor::repaintLineNumberArea()
{
    //repaint
    QRect lineNumberAreaRect(lineNumberArea->x(), lineNumberArea->y(),
                             lineNumberArea->width(), lineNumberArea->height());
    emit updateRequest(lineNumberAreaRect, 0);
}

QList<int> *CodeEditor::getBreakpoints()
{
    return &breakpoints;
}

void CodeEditor::highlightCurrentLine()
{
    QSettings settings("SASM Project", "SASM");
    if (!debugMode && settings.value("currentlinemode", true).toBool()) {
        QList<QTextEdit::ExtraSelection> extraSelections;

        if (!isReadOnly()) {
            QTextEdit::ExtraSelection selection;

            QColor lineColor = settings.value("currentlinecolor", QColor(232, 232, 255)).value<QColor>();

            selection.format.setBackground(lineColor);
            selection.format.setProperty(QTextFormat::FullWidthSelection, true);
            selection.cursor = textCursor();
            selection.cursor.clearSelection();
            extraSelections.append(selection);
        }

        setExtraSelections(extraSelections);
    }
}

void CodeEditor::highlightDebugLine(int lineNumber)
{
    if (debugMode) {
        QList<QTextEdit::ExtraSelection> extraSelections;

        if (!isReadOnly()) {
            QTextEdit::ExtraSelection selection;

            QSettings settings("SASM Project", "SASM");
            QColor lineColor = settings.value("debuglinecolor", QColor(235, 200, 40)).value<QColor>();

            selection.format.setBackground(lineColor);
            selection.format.setProperty(QTextFormat::FullWidthSelection, true);
            selection.cursor = QTextCursor(this->document());
            selection.cursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor, lineNumber - 1);
            selection.cursor.clearSelection();
            setTextCursor(selection.cursor); //scroll to debugging line
            extraSelections.append(selection);
        }

        setExtraSelections(extraSelections);
    }
}

void CodeEditor::updateDebugLine(int number)
{
    if (number == -1)
        setDebugMode(false);
    else
        setDebugMode(true);
    currentDebugLine = number;

    //create rectangle of line number area and highlight debug line throw updateRequest()
    QRect lineNumberAreaRect(lineNumberArea->x(), lineNumberArea->y(),
                             lineNumberArea->width(), lineNumberArea->height());
    emit updateRequest(lineNumberAreaRect, 0);

    highlightDebugLine(number);
    highlightCurrentLine();
}

void CodeEditor::setDebugMode(bool mode)
{
    debugMode = mode;
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
    QString curString = textCursor().block().text();
    int indentWidth = -1;
    switch (e->key()) {
    case (Qt::Key_Tab) :
        if (! (e->modifiers() & Qt::ShiftModifier))
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

void CodeEditor::shiftBreakpoints(int blockCount)
{
    if (prevBlockCount == -1) {
        prevBlockCount = this->blockCount();
        return;
    }
    //remove breakpoints in range
    int changedLine = textCursor().blockNumber() + 1; //blocks starts from 0
    if (blockCount < prevBlockCount) {
        for (int i = changedLine + 1; i <= changedLine + prevBlockCount - blockCount; i++) {
            breakpoints.removeOne(i);
        }
    }

    //shift breakpoints
    if (blockCount > prevBlockCount) {
        for (int i = 0; i < breakpoints.size(); i++) {
            if (breakpoints[i] >= changedLine)
                breakpoints[i] += blockCount - prevBlockCount;
        }
    } else {
        for (int i = 0; i < breakpoints.size(); i++) {
            if (breakpoints[i] > changedLine)
                breakpoints[i] += blockCount - prevBlockCount;
        }
    }
    prevBlockCount = blockCount;
    repaintLineNumberArea();
}

bool CodeEditor::isMacroOnCurrentDebugLine()
{
    if (currentDebugLine > 0) {
        QTextBlock block = document()->findBlockByLineNumber(currentDebugLine - 1);
        QString text = block.text();
        QStringList macrosPatterns;
        macrosPatterns << "\\bPRINT_DEC\\b" << "\\bPRINT_HEX\\b" <<
                          "\\bPRINT_CHAR\\b" << "\\bPRINT_STRING\\b" <<
                          "\\bNEWLINE\\b" << "\\bPRINT_UDEC\\b" <<
                          "\\bGET_UDEC\\b" << "\\bGET_DEC\\b" <<
                          "\\bGET_HEX\\b" << "\\bGET_CHAR\\b" <<
                          "\\bGET_STRING\\b" << "\\bCMAIN\\b" << "\\bCEXTERN\\b";
        foreach (const QString &pattern, macrosPatterns) {
            QRegExp regExp(pattern, Qt::CaseSensitive);
            if (text.indexOf(regExp) != -1)
                return true;
        }
    }
    return false;
}

CodeEditor::~CodeEditor()
{
    delete lineNumberArea;
}
