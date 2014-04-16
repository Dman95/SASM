#include "debugtablewidget.h"

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
        header << tr("Register") << tr("Hex") << tr("Integer");
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
        QString zeroes;
        zeroes.fill('0', 10 - hexValue.length());
        if (item(rowNumber, 2)) {
            item(rowNumber, 0)->setText(name);
            item(rowNumber, 1)->setText("0x" + zeroes + hexValue.right(hexValue.length() - 2));
            item(rowNumber, 2)->setText(decValue);
        } else {
            QTableWidgetItem *nameItem = new QTableWidgetItem(name);
            QTableWidgetItem *hexValueItem = new QTableWidgetItem("0x" + zeroes + hexValue.right(hexValue.length() - 2));
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
