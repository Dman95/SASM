/****************************************************************************
** SASM - simple IDE for NASM development
** Copyright (C) 2013 Dmitriy Manushin
** Contact: site: https://sites.google.com/site/simpleasmproject/
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

#include "commanddebugwindow.h"

CommandDebugWindow::CommandDebugWindow(QWidget *parent) :
    QDialog(parent)
{
    QSizePolicy size;
    size.setHorizontalPolicy(QSizePolicy::Fixed);
    runButton = new QPushButton;
    runButton->setText(tr("Run"));
    runButton->setSizePolicy(size);
    runButton->setDefault(true);

    size.setHorizontalPolicy(QSizePolicy::Expanding);
    size.setVerticalPolicy(QSizePolicy::Fixed);

    commandName = new QLineEdit;
    commandName->setMinimumHeight(25);
    commandName->setMaximumHeight(25);
    commandName->setSizePolicy(size);

    addLayout = new QHBoxLayout;
    addLayout->addWidget(commandName);
    addLayout->addWidget(runButton);

    this->setLayout(addLayout);

    connect(runButton, SIGNAL(clicked()), this, SLOT(runClick()));
}

void CommandDebugWindow::runClick()
{
    emit runCommand(commandName->text());
    this->close();
}

CommandDebugWindow::~CommandDebugWindow()
{
    delete runButton;
    delete commandName;
    delete addLayout;
}
