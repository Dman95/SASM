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

#include "debuganycommandwidget.h"

/**
 * @file debuganycommandwidget.cpp
 * Impliments the debugger's stepping features
 */

DebugAnyCommandWidget::DebugAnyCommandWidget(QWidget *parent) :
    QWidget(parent)
{
    currentCommandPos = -1;

    anyCommandLabel = new QLabel(tr("GDB command:"));
    command = new QLineEdit;
    performButton = new QPushButton(tr("Perform"));
    printCheckBox = new QCheckBox(tr("Print"));
    printCheckBox->setChecked(false);

    layout = new QHBoxLayout;
    layout->addWidget(anyCommandLabel);
    layout->addWidget(command);
    layout->addWidget(printCheckBox);
    layout->addWidget(performButton);

    setLayout(layout);

    connect(performButton, SIGNAL(clicked()), this, SLOT(processCommand()));
}

void DebugAnyCommandWidget::showPreviousCommand()
{
    if (currentCommandPos > 0)
        currentCommandPos--;
    if (currentCommandPos >= 0 && currentCommandPos < commands.size())
        command->setText(commands[currentCommandPos]);
}

void DebugAnyCommandWidget::showNextCommand()
{
    if (currentCommandPos < commands.size() - 1)
        currentCommandPos++;
    if (currentCommandPos >= 0 && currentCommandPos < commands.size())
        command->setText(commands[currentCommandPos]);
}

void DebugAnyCommandWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
        processCommand();
    if (event->key() == Qt::Key_Up)
        showPreviousCommand();
    if (event->key() == Qt::Key_Down)
        showNextCommand();
    QWidget::keyPressEvent(event);
}

void DebugAnyCommandWidget::processCommand()
{
    if (! command->text().isEmpty()) {
        if (commands.isEmpty() || command->text() != commands.last())
            commands.append(command->text());
        currentCommandPos = commands.size();
        emit performCommand(command->text(), printCheckBox->isChecked());
        command->clear();
    } else {
        if (!commands.isEmpty())
            emit performCommand(commands.last(), printCheckBox->isChecked());
    }
}

void DebugAnyCommandWidget::setFocusOnLineEdit()
{
    command->setFocus();
}

int DebugAnyCommandWidget::height()
{
    return command->sizeHint().height();
}

DebugAnyCommandWidget::~DebugAnyCommandWidget()
{
    delete printCheckBox;
    delete anyCommandLabel;
    delete command;
    delete performButton;
    delete layout;
}
