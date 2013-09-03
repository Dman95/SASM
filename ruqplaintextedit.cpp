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

#include "ruqplaintextedit.h"

RuQPlainTextEdit::RuQPlainTextEdit(QWidget *parent) :
    QPlainTextEdit(parent)
{
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

    undoAction->setEnabled(false);
    redoAction->setEnabled(false);
    connect(this, SIGNAL(undoAvailable(bool)), undoAction, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(redoAvailable(bool)), redoAction, SLOT(setEnabled(bool)));
}

QMenu * RuQPlainTextEdit::createMenu()
{
    QMenu *menu = new QMenu;
    QTextCursor textCursor = this->textCursor();

    commentAction->setEnabled(true);
    uncommentAction->setEnabled(true);
    cutAction->setEnabled(true);
    copyAction->setEnabled(true);
    deleteAction->setEnabled(true);
    //if nothing selected
    if (textCursor.selectionEnd() - textCursor.selectionStart() <= 0) {
        commentAction->setEnabled(false);
        uncommentAction->setEnabled(false);
        cutAction->setEnabled(false);
        copyAction->setEnabled(false);
        deleteAction->setEnabled(false);
    }

    cutAction->setVisible(true);
    copyAction->setVisible(true);
    if (this->isReadOnly()) {
        cutAction->setVisible(false);
        pasteAction->setVisible(false);
        deleteAction->setVisible(false);
    }

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
    contextMenu = createMenu();
    contextMenu->exec(e->globalPos());
    delete contextMenu;
}

void RuQPlainTextEdit::deleteSelected()
{
    QTextCursor textCursor = this->textCursor();
    textCursor.removeSelectedText();
}

void RuQPlainTextEdit::commentSelectedCode()
{
    QTextCursor cursor = this->textCursor();
    QString selected = cursor.selectedText().replace(QString(QChar::ParagraphSeparator),
                                                    QString(QChar::ParagraphSeparator) + QString(';'));
    selected.insert(0, QChar(';'));
    cursor.removeSelectedText();
    cursor.insertText(selected);
}

void RuQPlainTextEdit::uncommentSelectedCode()
{
    QTextCursor cursor = this->textCursor();
    QString selected = cursor.selectedText().replace(QString(QChar::ParagraphSeparator) + QString(';'),
                                                     QString(QChar::ParagraphSeparator));
    if (selected[0] == QChar(';'))
        selected.remove(0, 1);
    cursor.removeSelectedText();
    cursor.insertText(selected);
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
}
