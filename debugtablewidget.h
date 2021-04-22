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
#include "watchsettingswidget.h"
#include "stacksettingswidget.h"
#include "ruqplaintextedit.h"

/**
 * @file debugtablewidget.h
 * Defines the debugging window (memory or registers table).
 */

enum DebugTableWidgetType {registersTable, memoryTable, stackTable};


/**
 * ! \brief This class represents memory or registers table.
 *
 * This class contains the methods and variables relevant to memory and register windows under the debugger.
 * Methods include adding variable watches, modifying variable contents, and adding registers.
 * */

class DebugTableWidget : public QTableWidget
{
    Q_OBJECT

public:
    explicit DebugTableWidget(int rows, int columns, DebugTableWidgetType widgetType, QWidget *parent = 0);
    ~DebugTableWidget();
    bool isEmpty();
    void initializeMemoryWindow(const QList<RuQPlainTextEdit::Watch> &watches);
    static QByteArray memoryHeaderState;
    static QByteArray registerWindowState;
    static QByteArray stackWindowState;
    static bool geometryMemorySaved;
    static bool geometryRegistersSaved;
    static bool geometryStackSaved;

protected:
    void closeEvent(QCloseEvent *);
    void mousePressEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);

signals:
    void closeSignal();
    void debugShowMemory();

public slots:
    void deleteVariable();
    void addVariable(const QString &variableName, int rowNumber = -1);
    void addVariable(const RuQPlainTextEdit::Watch &variable, int rowNumber = -1);
    void changeVariableValue(const QString &value, int rowNumber, bool isValid);
    void addRegister(const QString &name, const QString &hexValue, const QString &decValue, int rowNumber);
    void addStack(const QString &hexValue, const QString &address);
    void changeMemoryWindow(int row, int column);
    void setValuesFromDebugger(QList<Debugger::memoryInfo> watches);
    void setValuesFromDebugger(QList<Debugger::registersInfo> registers);
    void setValuesFromDebugger(QList<Debugger::stackInfo> stacks);

private:
    int contextMenuLineNumber;
    DebugTableWidgetType type;
    bool empty;
    bool firstTime;
};

#endif // DEBUGTABLEWIDGET_H
