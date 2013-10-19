#ifndef DEBUGTABLEWIDGET_H
#define DEBUGTABLEWIDGET_H

#include <QTableWidget>
#include <QDesktopWidget>
#include <QHeaderView>
#include <QMouseEvent>
#include <QAction>
#include <QMenu>
#include <QByteArray>
#include "debugger.h"
#include "watchsettinswidget.h"
#include "ruqplaintextedit.h"

enum DebugTableWidgetType {registersTable, memoryTable};

class DebugTableWidget : public QTableWidget
{
    Q_OBJECT

public:
    explicit DebugTableWidget(int rows, int columns, DebugTableWidgetType widgetType, QWidget *parent = 0);
    ~DebugTableWidget();
    bool isEmpty();
    void initializeMemoryWindow(const QList<RuQPlainTextEdit::Watch> &watches);
    static QByteArray memoryHeaderState;
    static int memoryX, memoryY;
    static QByteArray registerWindowState;
    static bool geometryMemorySaved;
    static bool geometryRegistersSaved;

protected:
    void closeEvent(QCloseEvent *);
    void mousePressEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);

signals:
    void closeSignal();
    void debugShowMemory();

public slots:
    void adjust(int shift = 0); //own adjust size
    void deleteVariable();
    void addVariable(const QString &variableName, int rowNumber = -1);
    void addVariable(const RuQPlainTextEdit::Watch &variable, int rowNumber = -1);
    void changeVariableValue(const QString &value, int rowNumber, bool isValid);
    void addRegister(const QString &name, const QString &hexValue, const QString &decValue, int rowNumber);
    void changeMemoryWindow(int row, int column);
    void setValuesFromDebugger(QList<Debugger::memoryInfo> *watches);
    void setValuesFromDebugger(Debugger::registersInfo *registers);

private:
    int contextMenuLineNumber;
    DebugTableWidgetType type;
    bool empty;
    bool firstTime;
};

#endif // DEBUGTABLEWIDGET_H
