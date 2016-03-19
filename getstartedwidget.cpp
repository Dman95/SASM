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

#include "getstartedwidget.h"
/**

 * @file getstartedwidget.cpp
 * Implements the opening window.
 */

GetStartedWidget::GetStartedWidget(QWidget *parent) :
    QWidget(parent)
{
    layout = new QVBoxLayout;

    //header
    labelLayout = new QHBoxLayout;

    imgLabel = new QLabel;
    imgLabel->setPixmap(QPixmap(":images/smallIcon.png"));
    imgLabel->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    labelLayout->addWidget(imgLabel);
    labelLayout->addSpacing(20);

    welcomeLabel = new QLabel;
    QFont font("Arial", 24);
    welcomeLabel->setFont(font);
    welcomeLabel->setText(tr("Welcome to the SASM!"));
    welcomeLabel->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    labelLayout->addWidget(welcomeLabel);

    leftSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Fixed);
    labelLayout->addSpacerItem(leftSpacer);

    layout->addLayout(labelLayout);


    //buttons
    spacer = new QSpacerItem(0, 50, QSizePolicy::Fixed, QSizePolicy::Fixed);
    layout->addSpacerItem(spacer);

    newButton = new QCommandLinkButton(tr("Create new project"));
    newButton->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
    layout->addWidget(newButton);
    layout->addSpacing(10);

    openButton = new QCommandLinkButton(tr("Open project"));
    openButton->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
    layout->addWidget(openButton);
    layout->addSpacing(10);

    prevSessionButton = new QCommandLinkButton(tr("Restore last session"));
    prevSessionButton->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
    layout->addWidget(prevSessionButton);
    layout->addSpacing(10);

    bottomSpacer = new QSpacerItem(10, 100, QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addSpacerItem(bottomSpacer);


    setLayout(layout);
}

GetStartedWidget::~GetStartedWidget()
{
    delete welcomeLabel;
    delete imgLabel;
    delete labelLayout;
    delete newButton;
    delete openButton;
    delete prevSessionButton;
    delete layout;
}
