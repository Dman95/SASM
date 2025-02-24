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

#include "finddialog.h"

/**
 * @file finddialog.cpp
 * Implements the find text widget.
 */

FindDialog::FindDialog(QWidget *parent)
    : QWidget(parent)
{
    searchLabel = new QLabel(tr("Find what:"));
    searchEdit = new QLineEdit;
    searchLabel->setBuddy(searchEdit);

    replaceLabel = new QLabel(tr("Replace with:"));
    replaceEdit = new QLineEdit;
    replaceLabel->setBuddy(replaceEdit);

    caseCheckBox = new QCheckBox(tr("Match case"));

    findButton = new QPushButton(tr("Find next"));
    findButton->setDefault(true);
    findButton->setEnabled(false);

    findAllButton = new QPushButton(tr("Find all"));
    findAllButton->setAutoDefault(true);
    findAllButton->setEnabled(false);

    replaceButton = new QPushButton(tr("Replace"));
    replaceButton->setAutoDefault(true);
    replaceButton->setEnabled(false);

    replaceAllButton = new QPushButton(tr("Replace all"));
    replaceAllButton->setAutoDefault(true);
    replaceAllButton->setEnabled(false);

    closeButton = new QPushButton(tr("Close"));
    closeButton->setAutoDefault(true);

    connect(searchEdit, SIGNAL(textChanged(const QString &)),
            this, SLOT(enableFindButton(const QString &)));
    connect(searchEdit, SIGNAL(returnPressed()),
            findButton, SLOT(click()));

    connect(replaceEdit, SIGNAL(returnPressed()),
            replaceButton, SLOT(click()));

    connect(findButton, SIGNAL(clicked()),
            this, SLOT(findClicked()));

    connect(findAllButton, SIGNAL(clicked()),
            this, SLOT(findAllClicked()));

    connect(replaceButton, SIGNAL(clicked()),
            this, SLOT(replaceClicked()));

    connect(replaceAllButton, SIGNAL(clicked()),
            this, SLOT(replaceAllClicked()));

    connect(closeButton, SIGNAL(clicked()),
            this, SLOT(close()));

    replaceLayout = new QHBoxLayout;
    replaceLayout->addWidget(replaceLabel);
    replaceLayout->addWidget(replaceEdit);

    searchLayout = new QHBoxLayout;
    searchLayout->addWidget(searchLabel);
    searchSpacer = new QSpacerItem(
                replaceLabel->fontMetrics().horizontalAdvance(tr("Replace with:")) -
                searchLabel->fontMetrics().horizontalAdvance(tr("Find what:")),
                0, QSizePolicy::Fixed, QSizePolicy::Fixed);
    searchLayout->addSpacerItem(searchSpacer);
    searchLayout->addWidget(searchEdit);

    replaceButtonLayout = new QHBoxLayout;
    replaceButtonLayout->addWidget(replaceButton);
    replaceButtonLayout->addWidget(replaceAllButton);

    leftLayout = new QVBoxLayout;
    leftLayout->addLayout(searchLayout);
    leftLayout->addWidget(caseCheckBox);
    leftLayout->addSpacing(20);
    leftLayout->addLayout(replaceLayout);
    leftLayout->addLayout(replaceButtonLayout);

    rightLayout = new QVBoxLayout;
    rightLayout->addWidget(findButton);
    rightLayout->addWidget(findAllButton);
    rightLayout->addWidget(closeButton);
    rightLayout->addStretch();

    mainLayout = new QHBoxLayout;
    mainLayout->addLayout(leftLayout);
    mainLayout->addLayout(rightLayout);
    setLayout(mainLayout);

    setWindowFlags(Qt::Dialog);
    setWindowTitle(tr("Find and replace"));
    setFixedHeight(sizeHint().height());
}

void FindDialog::enableFindButton(const QString &text)
{
    findButton->setEnabled(!text.isEmpty());
    findAllButton->setEnabled(!text.isEmpty());
    replaceButton->setEnabled(!text.isEmpty());
    replaceAllButton->setEnabled(!text.isEmpty());
}

void FindDialog::findClicked()
{
    QString text = searchEdit->text();
    Qt::CaseSensitivity cs =
            caseCheckBox->isChecked() ? Qt::CaseSensitive
                                      : Qt::CaseInsensitive;

    emit findNext(text, cs, false, false);
}

void FindDialog::findAllClicked()
{
    QString text = searchEdit->text();
    Qt::CaseSensitivity cs =
            caseCheckBox->isChecked() ? Qt::CaseSensitive
                                      : Qt::CaseInsensitive;

    emit findNext(text, cs, true, false);
}

void FindDialog::replaceClicked()
{
    QString text = searchEdit->text();
    Qt::CaseSensitivity cs =
            caseCheckBox->isChecked() ? Qt::CaseSensitive
                                      : Qt::CaseInsensitive;

    emit findNext(text, cs, false, true, replaceEdit->text());
}

void FindDialog::replaceAllClicked()
{
    QString text = searchEdit->text();
    Qt::CaseSensitivity cs =
            caseCheckBox->isChecked() ? Qt::CaseSensitive
                                      : Qt::CaseInsensitive;

    emit findNext(text, cs, true, true, replaceEdit->text());
}

bool FindDialog::close()
{
    emit findNext(QString(), Qt::CaseSensitive, true, false);
    return QWidget::close();
}

void FindDialog::setSearchText(const QString &text)
{
    searchEdit->setText(text);
}

void FindDialog::keyPressEvent(QKeyEvent *e)
{
    switch(e->key())
    {
    case Qt::Key_Escape:
        close();
        break;
    default:
        QWidget::keyPressEvent(e);
    }
}

void FindDialog::showEvent(QShowEvent *e)
{
    searchEdit->setFocus();
    e->accept();
}

void FindDialog::closeEvent(QCloseEvent *e)
{
    close();
    e->accept();
}

FindDialog::~FindDialog()
{
    delete searchLabel;
    delete replaceLabel;
    delete searchEdit;
    delete replaceEdit;
    delete caseCheckBox;
    delete findButton;
    delete findAllButton;
    delete replaceButton;
    delete replaceAllButton;
    delete closeButton;
    delete replaceLayout;
    delete searchLayout;
    delete replaceButtonLayout;
    delete leftLayout;
    delete rightLayout;
    delete mainLayout;
}
