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

#include "ruqplaintextedit.h"

/**
 * @file ruplaintextedit.cpp
 * Implements the text editor base class
 */

RuQPlainTextEdit::RuQPlainTextEdit(QWidget *parent) :
    QPlainTextEdit(parent)
{
    QSettings settings("SASM Project", "SASM");
    QPalette palette = this->palette();
    palette.setColor(QPalette::Base, settings.value("backgroundcolor", palette.color(QPalette::Base)).value<QColor>());
    palette.setColor(QPalette::Text, settings.value("fontcolor", palette.color(QPalette::Text)).value<QColor>());
    setPalette(palette);

    setDebugDisabled();

    commentAction = new QAction(tr("Comment"), this);
    commentAction->setShortcut(QString("Ctrl+Shift+Q"));
    connect(commentAction, SIGNAL(triggered()), this, SLOT(commentSelectedCode()));

    uncommentAction = new QAction(tr("Remove comment"), this);
    uncommentAction->setShortcut(QString("Ctrl+Shift+A"));
    connect(uncommentAction, SIGNAL(triggered()), this, SLOT(uncommentSelectedCode()));

    undoAction = new QAction(tr("Undo"), this);
    undoAction->setShortcut(QKeySequence::Undo);
    connect(undoAction, SIGNAL(triggered()), this, SLOT(undo()));

    redoAction = new QAction(tr("Redo"), this);
    redoAction->setShortcut(QKeySequence::Redo);
    connect(redoAction, SIGNAL(triggered()), this, SLOT(redo()));

    cutAction = new QAction(tr("Cut"), this);
    cutAction->setShortcut(QKeySequence::Cut);
    connect(cutAction, SIGNAL(triggered()), this, SLOT(cut()));

    copyAction = new QAction(tr("Copy"), this);
    copyAction->setShortcut(QKeySequence::Copy);
    connect(copyAction, SIGNAL(triggered()), this, SLOT(copy()));

    pasteAction = new QAction(tr("Paste"), this);
    pasteAction->setShortcut(QKeySequence::Paste);
    connect(pasteAction, SIGNAL(triggered()), this, SLOT(paste()));

    deleteAction = new QAction(tr("Delete"), this);
    deleteAction->setShortcut(QKeySequence::Delete);
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(deleteSelected()));

    selectAllAction = new QAction(tr("Select all"), this);
    selectAllAction->setShortcut(QKeySequence::SelectAll);
    connect(selectAllAction, SIGNAL(triggered()), this, SLOT(selectAll()));

    addWatchAction = new QAction(this);
    connect(addWatchAction, SIGNAL(triggered()), this, SLOT(addWatch()));

    undoAction->setEnabled(false);
    redoAction->setEnabled(false);
    connect(this, SIGNAL(undoAvailable(bool)), undoAction, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(redoAvailable(bool)), redoAction, SLOT(setEnabled(bool)));
}

QMenu * RuQPlainTextEdit::createMenu()
{
    QMenu *menu = new QMenu;
    QTextCursor textCursor = this->textCursor();

    //! if nothing selected
    if (textCursor.selectionEnd() - textCursor.selectionStart() <= 0) {
        commentAction->setEnabled(false);
        uncommentAction->setEnabled(false);
        cutAction->setEnabled(false);
        copyAction->setEnabled(false);
        deleteAction->setEnabled(false);
    } else {
        commentAction->setEnabled(true);
        uncommentAction->setEnabled(true);
        cutAction->setEnabled(true);
        copyAction->setEnabled(true);
        deleteAction->setEnabled(true);
    }

    if (isReadOnly()) {
        cutAction->setVisible(false);
        pasteAction->setVisible(false);
        deleteAction->setVisible(false);
    } else {
        cutAction->setVisible(true);
        pasteAction->setVisible(true);
        deleteAction->setVisible(true);
    }

    QString variableName = variableOnCurrentLine().name;
    if (variableName.isEmpty()) {
        addWatchAction->setVisible(false);
    } else {
        addWatchAction->setVisible(true);
        addWatchAction->setText(tr("Watch '%1'").arg(variableName));
        if (!debugEnabled) {
            addWatchAction->setEnabled(false);
        } else {
            addWatchAction->setEnabled(true);
        }
    }

    menu->addAction(addWatchAction);
    menu->addSeparator();
    menu->addAction(commentAction);
    menu->addAction(uncommentAction);
    menu->addSeparator();
    menu->addAction(undoAction);
    menu->addAction(redoAction);
    menu->addSeparator();
    menu->addAction(cutAction);
    menu->addAction(copyAction);
    menu->addAction(pasteAction);
    menu->addAction(deleteAction);
    menu->addSeparator();
    menu->addAction(selectAllAction);

    return menu;
}

void RuQPlainTextEdit::contextMenuEvent(QContextMenuEvent *e)
{
    QMouseEvent leftClick(QEvent::MouseButtonPress, e->pos(), Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
    mousePressEvent(&leftClick);
    contextMenu = createMenu();
    contextMenu->exec(e->globalPos());
    if (!contextMenu.isNull())
        delete contextMenu;
}

void RuQPlainTextEdit::deleteSelected()
{
    QTextCursor textCursor = this->textCursor();
    textCursor.removeSelectedText();
}

void RuQPlainTextEdit::commentSelectedCode()
{
    QTextCursor cursor = textCursor();
    QString selected = cursor.selectedText().replace(QString(QChar::ParagraphSeparator),
                                                    QString(QChar::ParagraphSeparator) + QString(';'));
    selected.insert(0, QChar(';'));
    cursor.removeSelectedText();
    cursor.insertText(selected);
}

void RuQPlainTextEdit::uncommentSelectedCode()
{
    QTextCursor cursor = textCursor();
    QString selected = cursor.selectedText().replace(QString(QChar::ParagraphSeparator) + QString(';'),
                                                     QString(QChar::ParagraphSeparator));
    if (selected[0] == QChar(';'))
        selected.remove(0, 1);
    cursor.removeSelectedText();
    cursor.insertText(selected);
}

RuQPlainTextEdit::Watch RuQPlainTextEdit::variableOnCurrentLine()
{
    QTextBlock block = document()->findBlock(textCursor().position());
    QString text = block.text();
    QStringList memoryPatterns;
    memoryPatterns << "\\bresb\\b" << "\\bresw\\b" << "\\bresd\\b" <<
                      "\\bresq\\b" << "\\brest\\b" << "\\breso\\b" <<
                      "\\bresy\\b" << "\\bddq\\b" << "\\bresdq\\b" <<
                      "\\bdb\\b" << "\\bdw\\b" << "\\bdd\\b" <<
                      "\\bdq\\b" << "\\bdt\\b" << "\\bdo\\b" <<
                      "\\bdy\\b" << "\\bequ\\b";
    bool isVariableLine = false;
    foreach (const QString &pattern, memoryPatterns)
        if (text.indexOf(QRegExp(pattern)) != -1) {
            isVariableLine = true;
            break;
        }
    QString variableName;
    int variableSize = 0;
    int arraySize = 0;
    if (isVariableLine) {
        QTextStream line(&text);
        line.skipWhiteSpace();
        line >> variableName;
        line.skipWhiteSpace();
        QString temp = 0;
        line >> temp;
        bool isArray = false;
        if (temp.indexOf("res") != -1)
            isArray = true;
        temp = temp.right(1);
        if (temp == "q")
            variableSize = 3;
        if (temp == "b")
            variableSize = 2;
        if (temp == "w")
            variableSize = 1;
        line.skipWhiteSpace();
        if (isArray) {
            line >> arraySize;
        }
    }
    Watch w;
    w.name = variableName;
    w.type = 0;
    w.size = variableSize;
    if (arraySize == 1)
        arraySize = 0;
    w.arraySize = arraySize;
    w.address = false;
    return w;
}

void RuQPlainTextEdit::addWatch()
{
    emit addWatchSignal(variableOnCurrentLine());
}

void RuQPlainTextEdit::setDebugDisabled()
{
    debugEnabled = false;
}

void RuQPlainTextEdit::setDebugEnabled()
{
    debugEnabled = true;
}

RuQPlainTextEdit::~RuQPlainTextEdit()
{
    delete commentAction;
    delete uncommentAction;
    delete undoAction;
    delete redoAction;
    delete cutAction;
    delete copyAction;
    delete pasteAction;
    delete deleteAction;
    delete selectAllAction;
    delete addWatchAction;
}
