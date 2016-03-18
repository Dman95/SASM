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

#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <QWidget>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QCloseEvent>

/**
 * @file finddialog.h
 * The dialog box used to find text.
 */



/*! \brief This class represents the "find text" functionality
 *
 *
 * All of the methods including
 *  Find
 *  Find All
 *  Find&Replace
 * Are defined here.
 */

class FindDialog : public QWidget
{
    Q_OBJECT

public:
    FindDialog(QWidget *parent = 0);
    ~FindDialog();

    void closeEvent(QCloseEvent *e);

signals:
    void findNext(const QString &str, Qt::CaseSensitivity cs, bool all, bool replace,
                  const QString &replaceText = 0);

public slots:
    bool close();

private slots:
    void findClicked();
    void findAllClicked();
    void replaceClicked();
    void replaceAllClicked();
    void enableFindButton(const QString &text);

private:
    QLabel *searchLabel;
    QLabel *replaceLabel;
    QLineEdit *searchEdit;
    QLineEdit *replaceEdit;
    QCheckBox *caseCheckBox;
    QPushButton *findButton;
    QPushButton *findAllButton;
    QPushButton *replaceButton;
    QPushButton *replaceAllButton;
    QPushButton *closeButton;

    QHBoxLayout *replaceLayout;
    QHBoxLayout *searchLayout;
    QHBoxLayout *replaceButtonLayout;
    QVBoxLayout *leftLayout;
    QVBoxLayout *rightLayout;
    QHBoxLayout *mainLayout;
    QSpacerItem *searchSpacer;
};

#endif
