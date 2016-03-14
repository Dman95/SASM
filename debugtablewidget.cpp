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

#include "debugtablewidget.h"

/**
 * @file debugtablewidget.cpp
 * Impliments the debuging memory window
 */

QByteArray DebugTableWidget::memoryHeaderState;
bool DebugTableWidget::geometryRegistersSaved;
bool DebugTableWidget::geometryMemorySaved;
QByteArray DebugTableWidget::registerWindowState;

DebugTableWidget::DebugTableWidget(int rows, int columns, DebugTableWidgetType widgetType, QWidget *parent) :
    QTableWidget(rows, columns, parent)
{
    empty = true;
    type = widgetType;
    firstTime = true;
    verticalHeader()->hide();

    if (type == memoryTable) {
        setSelectionMode(QAbstractItemView::NoSelection);
        QStringList header;
        header << tr("Variable or expression") << tr("Value") << tr("Type");
        setHorizontalHeaderLabels(header);
        setWindowTitle(tr("Memory"));
        horizontalHeader()->setHighlightSections(false);
        resizeColumnsToContents();
    }

    if (type == registersTable) {
        setSelectionMode(QAbstractItemView::NoSelection);
        QStringList header;
        header << tr("Register") << tr("Hex") << tr("Info");
        setHorizontalHeaderLabels(header);
        setWindowTitle(tr("Registers"));
        resizeColumnsToContents();
        if (geometryRegistersSaved)
            restoreGeometry(registerWindowState);
    }
}

void DebugTableWidget::initializeMemoryWindow(const QList<RuQPlainTextEdit::Watch> &watches)
{
    if (type == memoryTable) {
        horizontalHeader()->restoreState(memoryHeaderState);
        int i;
        for (i = 0; i < watches.size(); i++)
            addVariable(watches[i], i);
        addVariable(tr("Add variable..."), i);
        connect(this, SIGNAL(cellChanged(int,int)), this, SLOT(changeMemoryWindow(int,int)), Qt::QueuedConnection);
    }
}

void DebugTableWidget::setValuesFromDebugger(QList<Debugger::memoryInfo> watches) //watches
{
    for (int i = 0; i < rowCount() - 1; i++) {
        changeVariableValue(watches[i].value, i, watches[i].isValid);
    }
    show();
}

void DebugTableWidget::setValuesFromDebugger(QList<Debugger::registersInfo> registers) //registers
{
    for (int i = 0; i < registers.size(); i++)
        addRegister(registers[i].name, registers[i].hexValue, registers[i].decValue, i);
    show();
    if (firstTime) {
        firstTime = false;
        activateWindow();
    }
}

void DebugTableWidget::changeMemoryWindow(int row, int column)
{
    disconnect(this, SIGNAL(cellChanged(int,int)), this, SLOT(changeMemoryWindow(int,int)));
    QString validText = item(row, 0)->text().remove("\\");
    item(row, 0)->setText(validText);
    if (row == rowCount() - 1 && column == 0)
        addVariable(tr("Add variable..."), rowCount());
    if (column == 0)
        emit debugShowMemory();
    connect(this, SIGNAL(cellChanged(int,int)), this, SLOT(changeMemoryWindow(int,int)), Qt::QueuedConnection);
}

void DebugTableWidget::changeVariableValue(const QString &value, int rowNumber, bool isValid)
{
    if (type == memoryTable) {
        if (isValid)
            item(rowNumber, 1)->setText(value);
        else
            item(rowNumber, 1)->setText(tr("Wrong variable or address: \"%1\"").arg(item(rowNumber, 0)->text()));
        resizeColumnsToContents();
    }
}

void DebugTableWidget::deleteVariable()
{
    if (contextMenuLineNumber >= rowCount() - 1) //last line
        return;
    removeRow(contextMenuLineNumber);
    resizeColumnsToContents();
}

void DebugTableWidget::addVariable(const QString &variableName, int rowNumber)
{
    empty = false;
    if (type == memoryTable) {
        if (rowNumber == -1)
            rowNumber = rowCount() - 1;
        insertRow(rowNumber);
        QTableWidgetItem *name = new QTableWidgetItem(variableName);
        QTableWidgetItem *value = new QTableWidgetItem;
        WatchSettinsWidget *settings = new WatchSettinsWidget;
        connect(settings, SIGNAL(settingsChanged()), this, SIGNAL(debugShowMemory()));
        setItem(rowNumber, 0, name);
        setItem(rowNumber, 1, value);
        setCellWidget(rowNumber, 2, settings);
        resizeColumnsToContents();
    }
}

void DebugTableWidget::addVariable(const RuQPlainTextEdit::Watch &variable, int rowNumber)
{
    empty = false;
    if (type == memoryTable) {
        if (rowNumber == -1)
            rowNumber = rowCount() - 1;
        insertRow(rowNumber);
        QTableWidgetItem *name = new QTableWidgetItem(variable.name);
        QTableWidgetItem *value = new QTableWidgetItem;
        WatchSettinsWidget *settings = new WatchSettinsWidget;
        settings->sizeComboBox->setCurrentIndex(variable.size);
        settings->typeComboBox->setCurrentIndex(variable.type);
        settings->addressCheckbox->setChecked(variable.address);
        if (variable.arraySize > 0)
            settings->arraySizeEdit->setText(QString::number(variable.arraySize));
        connect(settings, SIGNAL(settingsChanged()), this, SIGNAL(debugShowMemory()));
        setItem(rowNumber, 0, name);
        setItem(rowNumber, 1, value);
        setCellWidget(rowNumber, 2, settings);
        resizeColumnsToContents();
    }
}

void DebugTableWidget::addRegister(const QString &name, const QString &hexValue, const QString &decValue, int rowNumber)
{
    empty = false;
    if (type == registersTable) {
        if (item(rowNumber, 2)) {
            item(rowNumber, 0)->setText(name);
            if (hexValue.isEmpty())
                item(rowNumber, 1)->setText("");
            else
                item(rowNumber, 1)->setText(hexValue);
            item(rowNumber, 2)->setText(decValue);
        } else {
            QTableWidgetItem *nameItem = new QTableWidgetItem(name);
            QTableWidgetItem *hexValueItem;
            if (hexValue.isEmpty())
                hexValueItem = new QTableWidgetItem("");
            else
                hexValueItem = new QTableWidgetItem(hexValue);
            QTableWidgetItem *decValueItem = new QTableWidgetItem(decValue);
            QFont monoFont("Courier");
            monoFont.setStyleHint(QFont::Monospace);
            hexValueItem->setFont(monoFont);
            decValueItem->setFont(monoFont);
            setItem(rowNumber, 0, nameItem);
            setItem(rowNumber, 1, hexValueItem);
            setItem(rowNumber, 2, decValueItem);
        }
        resizeColumnsToContents();
    }
}

bool DebugTableWidget::isEmpty()
{
    return empty;
}

void DebugTableWidget::closeEvent(QCloseEvent *) {
    emit closeSignal();
}

void DebugTableWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton && type == memoryTable) {
        contextMenuLineNumber = itemAt(event->pos())->row();
        setCurrentCell(contextMenuLineNumber, 0);
        if (contextMenuLineNumber >= rowCount() - 1) //last line
            return;
        QMenu *menu = new QMenu;
        QAction *deleteAction = new QAction(tr("Delete watch"), this);
        connect(deleteAction, SIGNAL(triggered()), this, SLOT(deleteVariable()));
        menu->addAction(deleteAction);
        menu->exec(event->globalPos());
        delete deleteAction;
        delete menu;
    } else
        QTableWidget::mousePressEvent(event);
}

void DebugTableWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Delete) {
        contextMenuLineNumber = currentIndex().row();
        deleteVariable();
    } else
        QTableWidget::keyPressEvent(event);
}

DebugTableWidget::~DebugTableWidget()
{
    if (type == memoryTable) {
        memoryHeaderState = horizontalHeader()->saveState();
        geometryMemorySaved = true;
    }
    if (type == registersTable) {
        registerWindowState = saveGeometry();
        geometryRegistersSaved = true;
    }
}
