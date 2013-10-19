#include "debugtablewidget.h"

QByteArray DebugTableWidget::memoryHeaderState;
int DebugTableWidget::memoryX;
int DebugTableWidget::memoryY;
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
    setWindowFlags(Qt::Tool);

    if (type == memoryTable) {
        setSelectionMode(QAbstractItemView::SingleSelection);
        QStringList header;
        header << tr("Variable") << tr("Value") << tr("Type");
        setHorizontalHeaderLabels(header);
        setWindowTitle(tr("Memory"));
        horizontalHeader()->resizeSection(1,
            QFontMetrics(horizontalHeader()->font()).width(tr("Wrong variable or address: \"varName\"") + 10));
        horizontalHeader()->resizeSection(0,
            qMax(QFontMetrics(horizontalHeader()->font()).width(tr("Add variable...")) + 15,
                 QFontMetrics(horizontalHeader()->font()).width(tr("Variable")) + 15));
        adjust(0);
    }

    if (type == registersTable) {
        setEditTriggers(QAbstractItemView::NoEditTriggers);
        setSelectionMode(QAbstractItemView::NoSelection);
        QStringList header;
        header << tr("Register") << tr("Hex") << tr("Integer");
        setHorizontalHeaderLabels(header);
        setWindowTitle(tr("Registers"));
        horizontalHeader()->resizeSection(2,
            QFontMetrics(horizontalHeader()->font()).width("<function.sasmMacroE_0.L>") + 10);
        horizontalHeader()->resizeSection(1,
            QFontMetrics(horizontalHeader()->font()).width("0x99999999") + 10);
        horizontalHeader()->resizeSection(0,
            QFontMetrics(horizontalHeader()->font()).width("Register") + 15);
        adjust(0);
        if (geometryRegistersSaved)
            restoreGeometry(registerWindowState);
        else {
            int tableWidth = frameGeometry().width() + 2;
            move(QDesktopWidget().availableGeometry().width() - tableWidth - 50, 175);
        }
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
        if (geometryMemorySaved) {
            move(memoryX, memoryY);
        } else {
            int tableWidth = frameGeometry().width() + 2;
            move(QDesktopWidget().availableGeometry().width() - tableWidth - 50, 80);
        }
        connect(this, SIGNAL(cellChanged(int,int)), this, SLOT(changeMemoryWindow(int,int)), Qt::QueuedConnection);
        show();
        activateWindow();
    }
}

void DebugTableWidget::setValuesFromDebugger(QList<Debugger::memoryInfo> *watches){ //watches
    for (int i = 0; i < rowCount() - 1; i++) {
        changeVariableValue((*watches)[i].value, i, (*watches)[i].isValid);
    }
    show();
    activateWindow();
}

void DebugTableWidget::setValuesFromDebugger(Debugger::registersInfo *registers) //registers
{
    for (int i = 0; i < 16; i++)
        addRegister(registers[i].name, registers[i].hexValue, registers[i].decValue, i);
    show();
    activateWindow();
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
    }
}

void DebugTableWidget::deleteVariable()
{
    if (contextMenuLineNumber >= rowCount() - 1) //last line
        return;
    int height = rowHeight(contextMenuLineNumber);
    this->removeRow(contextMenuLineNumber);
    adjust(height);
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
        if (firstTime) {
            horizontalHeader()->resizeSection(2, settings->sumSize());
            firstTime = false;
        }
        setItem(rowNumber, 0, name);
        setItem(rowNumber, 1, value);
        setCellWidget(rowNumber, 2, settings);
        item(rowNumber, 1)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable);
        adjust(rowHeight(rowNumber));
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
        if (firstTime) {
            horizontalHeader()->resizeSection(2, settings->sumSize());
            firstTime = false;
        }
        setItem(rowNumber, 0, name);
        setItem(rowNumber, 1, value);
        setCellWidget(rowNumber, 2, settings);
        item(rowNumber, 1)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable);
        adjust(rowHeight(rowNumber));
    }
}

void DebugTableWidget::addRegister(const QString &name, const QString &hexValue, const QString &decValue, int rowNumber)
{
    empty = false;
    if (type == registersTable) {
        if (item(2, rowNumber)) {
            item(rowNumber, 0)->setText(name);
            item(rowNumber, 1)->setText(hexValue);
            item(rowNumber, 2)->setText(decValue);
        } else {
            QTableWidgetItem *nameItem = new QTableWidgetItem(name);
            QTableWidgetItem *hexValueItem = new QTableWidgetItem(hexValue);
            QTableWidgetItem *decValueItem = new QTableWidgetItem(decValue);
            setItem(rowNumber, 0, nameItem);
            setItem(rowNumber, 1, hexValueItem);
            setItem(rowNumber, 2, decValueItem);
        }
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
        contextMenuLineNumber = this->itemAt(event->pos())->row();
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

void DebugTableWidget::adjust(int shift)
{
    int tableHeight, tableWidth;
    tableHeight = horizontalHeader()->height() + verticalHeader()->length() + 2;
    tableWidth = horizontalHeader()->length() + 2;
    QPoint globalPosition = QPoint(frameGeometry().x(), geometry().y());
    resize(tableWidth, tableHeight);
    //for correct redrawing
    scroll(1, 0);
    scroll(-1, 0);
    move(globalPosition.x(), globalPosition.y() - shift); //shift after insertion or removing
}

DebugTableWidget::~DebugTableWidget()
{
    if (type == memoryTable) {
        memoryX = x();
        memoryY = y();
        memoryHeaderState = horizontalHeader()->saveState();
        geometryMemorySaved = true;
    }
    if (type == registersTable) {
        registerWindowState = saveGeometry();
        geometryRegistersSaved = true;
    }
}
