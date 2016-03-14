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

#include "watchsettinswidget.h"
/**
 * @file watchsettingswidget.cpp
 *
 */

/**
 * @file watchsettinswidget.cpp
 * Implimentation of the WatchSettingsWidget
 */

/*! \brief  This defines the layout such as spacing and items of the variable watch
 * area.
 *
 *
 *First, the parent widget is passed to the class. Next, the form items are added
 * and spaced appropriately. The form items are then further defined and then connected.
*/
WatchSettinsWidget::WatchSettinsWidget(QWidget *parent) :
    QWidget(parent)
{
    layout = new QHBoxLayout(this);
    typeComboBox = new QComboBox;
    sizeComboBox = new QComboBox;
    arraySizeEdit = new QLineEdit;
    addressCheckbox = new QCheckBox(tr("Address"));
    layout->addSpacing(3);
    layout->addWidget(typeComboBox);
    layout->addWidget(sizeComboBox);
    layout->addWidget(arraySizeEdit);
    layout->addSpacing(5);
    layout->addWidget(addressCheckbox);

    arraySizeEdit->setPlaceholderText(tr("Array size"));

    QStringList comboBoxList;
    comboBoxList << tr("Smart") << tr("Hex") << tr("Bin") << tr("Char") <<
                    tr("Int") << tr("UInt") << tr("Float");
    typeComboBox->insertItems(0, comboBoxList);

    QStringList sizeBoxList;
    sizeBoxList << tr("d") << tr("w") << tr("b") << tr("q");
    sizeComboBox->insertItems(0, sizeBoxList);

    addressCheckbox->setChecked(false);

    layout->setSpacing(0);
    layout->setMargin(0);

    connect(typeComboBox, SIGNAL(currentIndexChanged(int)), this, SIGNAL(settingsChanged()));
    connect(sizeComboBox, SIGNAL(currentIndexChanged(int)), this, SIGNAL(settingsChanged()));
    connect(arraySizeEdit, SIGNAL(textChanged(QString)), this, SIGNAL(settingsChanged()));
    connect(addressCheckbox, SIGNAL(stateChanged(int)), this, SIGNAL(settingsChanged()));

    setLayout(layout);
}

int WatchSettinsWidget::sumSize()
{
    return typeComboBox->sizeHint().width() + sizeComboBox->sizeHint().width()
            + arraySizeEdit->fontMetrics().width(arraySizeEdit->placeholderText())
            + addressCheckbox->sizeHint().width() + 50;
}

QSize WatchSettinsWidget::sizeHint() const
{
    return QSize(typeComboBox->sizeHint().width() + sizeComboBox->sizeHint().width()
            + arraySizeEdit->fontMetrics().width(arraySizeEdit->placeholderText())
            + addressCheckbox->sizeHint().width() + 50, 0);
}

WatchSettinsWidget::~WatchSettinsWidget()
{
    delete arraySizeEdit;
    delete typeComboBox;
    delete sizeComboBox;
    delete addressCheckbox;
    delete layout;
}
