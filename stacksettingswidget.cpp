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

#include "stacksettingswidget.h"

/*! \brief  This defines the layout such as spacing and items of the stack
 *
 *
 * First, the parent widget is passed to the class. Next, the form items are added
 * and spaced appropriately. The form items are then further defined and then connected.
*/
StackSettingsWidget::StackSettingsWidget(QWidget *parent) :
    QWidget(parent)
{
    layout = new QHBoxLayout(this);
    typeComboBox = new QComboBox;
    sizeComboBox = new QComboBox;
    signCheckbox = new QCheckBox(tr("Signed/Unsigned"));
    layout->addWidget(typeComboBox);
    layout->addWidget(sizeComboBox);
    //layout->addSpacing(1);
    layout->addWidget(signCheckbox);

    QStringList comboBoxList;
    comboBoxList << tr("Hex") << QString("Dec") << tr("Bin");
    typeComboBox->insertItems(0, comboBoxList);

    QStringList sizeBoxList;
    sizeBoxList << QString("8") << QString("16") << QString("32") << QString("64");
    sizeComboBox->insertItems(0, sizeBoxList);

    signCheckbox->setChecked(false);

    layout->setSpacing(0);
    layout->setMargin(0);

    connect(typeComboBox, SIGNAL(currentIndexChanged(int)), this, SIGNAL(stacksettingsChanged()));
    connect(sizeComboBox, SIGNAL(currentIndexChanged(int)), this, SIGNAL(stacksettingsChanged()));
    connect(signCheckbox, SIGNAL(stateChanged(int)), this, SIGNAL(stacksettingsChanged()));

    setLayout(layout);
}

StackSettingsWidget::~StackSettingsWidget()
{
    delete typeComboBox;
    delete sizeComboBox;
    delete signCheckbox;
    delete layout;
}
