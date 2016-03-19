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

#ifndef RUQPLAINTEXTEDIT_H
#define RUQPLAINTEXTEDIT_H

#include <QPlainTextEdit>
#include <QAction>
#include <QMenu>
#include <QContextMenuEvent>
#include <QTextBlock>
#include <QTextStream>
#include <QPointer>
#include <QSettings>

/**
 * @file ruqplaintextedit.h
 * Text editor base class definition file.
 */


/*! \brief This defines the base class which the text editor is derived from.
 *
 * The class contains methods that are used in the editor. These range from simple
 * copying and pasting to enabling/disabling the debugger.
 */

class RuQPlainTextEdit : public QPlainTextEdit
{
    Q_OBJECT

public:
    //! The class constructor creates the editor, given a specified parent QWidget object.
    RuQPlainTextEdit(QWidget *parent = 0);
    ~RuQPlainTextEdit();

    //! Creates a menu
    QMenu *createMenu();

    //! Defines a structure to keep track of a watched variable.
    struct Watch {
        QString name;
        int type;
        int size;
        int arraySize;
        bool address;
    };

protected:
    void contextMenuEvent(QContextMenuEvent *e);

private:
    RuQPlainTextEdit::Watch variableOnCurrentLine();

    QPointer<QMenu> contextMenu;
    //! Creates a comment
    QAction *commentAction;
    //! Removes a comment
    QAction *uncommentAction;
    //! Undo the last action
    QAction *undoAction;
    //! Do the previous action again.
    QAction *redoAction;
    //! Cut a selected text string.
    QAction *cutAction;
    //! Copy a selected text string.
    QAction *copyAction;
    //! Paste the clipboard contents.
    QAction *pasteAction;
    //! Delete a selected text string.
    QAction *deleteAction;
    //! Select all of the text.
    QAction *selectAllAction;
    //! Add a watch on a variable.
    QAction *addWatchAction;
    //! Used to keep track if the debugger is enabled.
    bool debugEnabled;

public slots:
    //! Method that comments the user selected code.
    void commentSelectedCode();
    //! Method for uncommenting a previously commented code block.
    void uncommentSelectedCode();

    void deleteSelected();
    //! Method for adding a variable watch.
    void addWatch();
    //! Used to enable the debugger.
    void setDebugEnabled();
    //! Used to disable the debugger.
    void setDebugDisabled();

signals:
    //! Signal which emited when watch variable action is selected.
    void addWatchSignal(RuQPlainTextEdit::Watch variable);
};

#endif // RUQPLAINTEXTEDIT_H
