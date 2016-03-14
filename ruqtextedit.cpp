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

#include "ruqtextedit.h"

/**
 * @file ruqtextedit.cpp
 * UNKNOWN
 */

RuQTextEdit::RuQTextEdit(QWidget *parent) :
    QTextEdit(parent)
{
    QSettings settings("SASM Project", "SASM");
    QPalette palette = this->palette();
    palette.setColor(QPalette::Base, settings.value("backgroundcolor", palette.color(QPalette::Base)).value<QColor>());
    palette.setColor(QPalette::Text, settings.value("fontcolor", palette.color(QPalette::Text)).value<QColor>());
    setPalette(palette);

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

    clearAction = new QAction(tr("Clear"), this);
    connect(clearAction, SIGNAL(triggered()), this, SLOT(clear()));

    undoAction->setEnabled(false);
    redoAction->setEnabled(false);
    connect(this, SIGNAL(undoAvailable(bool)), undoAction, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(redoAvailable(bool)), redoAction, SLOT(setEnabled(bool)));
}

void RuQTextEdit::contextMenuEvent(QContextMenuEvent *e)
{
    contextMenu = new QMenu;
    QTextCursor textCursor = this->textCursor();

    //!if nothing selected
    if (textCursor.selectionEnd() - textCursor.selectionStart() <= 0) {
        cutAction->setEnabled(false);
        copyAction->setEnabled(false);
        deleteAction->setEnabled(false);
    } else {
        cutAction->setEnabled(true);
        copyAction->setEnabled(true);
        deleteAction->setEnabled(true);
    }

    if (isReadOnly()) {
        undoAction->setVisible(false);
        redoAction->setVisible(false);
        cutAction->setVisible(false);
        pasteAction->setVisible(false);
        deleteAction->setVisible(false);
        clearAction->setVisible(true);
    } else {
        undoAction->setVisible(true);
        redoAction->setVisible(true);
        cutAction->setVisible(true);
        pasteAction->setVisible(true);
        deleteAction->setVisible(true);
        clearAction->setVisible(false);
    }

    contextMenu->addAction(undoAction);
    contextMenu->addAction(redoAction);
    contextMenu->addSeparator();
    contextMenu->addAction(cutAction);
    contextMenu->addAction(copyAction);
    contextMenu->addAction(pasteAction);
    contextMenu->addAction(deleteAction);
    contextMenu->addSeparator();
    contextMenu->addAction(selectAllAction);
    contextMenu->addAction(clearAction);

    contextMenu->exec(e->globalPos());
    if (!contextMenu.isNull())
        delete contextMenu;
}

void RuQTextEdit::deleteSelected()
{
    QTextCursor textCursor = this->textCursor();
    textCursor.removeSelectedText();
}

RuQTextEdit::~RuQTextEdit()
{
    delete undoAction;
    delete redoAction;
    delete cutAction;
    delete copyAction;
    delete pasteAction;
    delete deleteAction;
    delete selectAllAction;
    delete clearAction;
}
