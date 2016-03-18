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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QTextBrowser>
#include <QTimer>
#include <QMenuBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QTime>
#include <QPointer>
#include <QColorDialog>
#include <QSignalMapper>
#include <QMap>
#include <QSplitter>
#include <QToolBar>
#include <QMutex>
#include <QDragEnterEvent>
#include <QMimeData>
#include "tab.h"
#include "highlighter.h"
#include "debugger.h"
#include "commanddebugwindow.h"
#include "finddialog.h"
#include "ruqtextedit.h"
#include "getstartedwidget.h"
#include "ui_settings.h"
#include "debugtablewidget.h"
#include "debuganycommandwidget.h"
#include "assembler.h"
#include "nasm.h"
#include "gas.h"
#include "common.h"
#include "fasm.h"
#include "signallocker.h"
#include "masm.h"

#define SASM_VERSION "3.4.0"

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(const QStringList & args, QWidget *parent = 0);
    ~MainWindow();

    void initUi();
    void createMenus();
    void createActions();
    void createButtons();
    void createToolBars();
    void writeSettings();
    void setupEditor(int i);
    bool okToContinue(int index = -1);
    void setCurrentTabName(const QString &filePath, int index = -1);
    bool removeDirRecuresively(const QString &dirName);

private:
    //ui
    GetStartedWidget *getStartedWidget;
    QStackedWidget *mainWidget;
    QSplitter *splitter;
    QVBoxLayout *workLayout;
    QWidget *workWidget;
    RuQTextEdit *compilerOut;
    QTabWidget *tabs;

    //menus and actions
    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *debugMenu;
    QMenu *buildMenu;
    QMenu *settingsMenu;
    QMenu *helpMenu;
    QAction *newAction;
    QAction *openAction;
    QAction *closeAction;
    QAction *saveAction;
    QAction *saveAsAction;
    QAction *saveExeAction;
    QAction *exitAction;
    QAction *findAction;
    QAction *commentAction;
    QAction *uncommentAction;
    QAction *undoAction;
    QAction *redoAction;
    QAction *cutAction;
    QAction *copyAction;
    QAction *pasteAction;
    QAction *deleteAction;
    QAction *selectAllAction;
    QAction *putTabAction;
    QAction *deleteTabAction;
    QAction *buildAction;
    QAction *runAction;
    QAction *runExeAction;
    QAction *stopAction;
    QAction *debugAction;
    QAction *debugNextAction;
    QAction *debugNextNiAction;
    QAction *debugToggleBreakpointAction;
    QAction *debugShowRegistersAction;
    QAction *debugShowMemoryAction;
    QAction *settingsAction;
    QAction *helpAction;
    QAction *aboutAction;

    //toolbars
    QToolBar *fileToolBar;
    QToolBar *editToolBar;
    QToolBar *buildToolBar;
    QToolBar *debugToolBar;

    //builder and debugger and all that concern to them
    QProcess *runProcess;
    CodeEditor *prevCodeEditor;
    QTimer *timer;
    QTime programExecutionTime;
    Debugger *debugger;
    bool programIsBuilded;
    QPointer<DebugTableWidget> registersWindow;
    QDockWidget *registersDock;
    QPointer<DebugTableWidget> memoryWindow;
    QDockWidget *memoryDock;
    QList<RuQPlainTextEdit::Watch> watches;
    DebugAnyCommandWidget *debugAnyCommandWidget;
    bool programStopped;
    int outputIndex;
    Assembler *assembler;
    bool debuggerWasStarted;
    QString debugKey;

    //highlighters
    Highlighter *highlighter;
    Highlighter *settingsHighlighter;

    //search
    QPointer<FindDialog> findDialog;
    Qt::CaseSensitivity prevCs;

    //settings and help
    QPointer<QWidget> settingsWindow;
    Ui::SettingsWindow settingsUi;
    QString startText;
    CodeEditor *settingsStartTextEditor;
    QString saveOpenDirectory; //save and open
    QPointer<QTextBrowser> help;
    QSignalMapper *colorSignalMapper;
    QSignalMapper *fontsSignalMapper;
    QList<QPushButton *> colorButtons;
    QList<QColor> defaultColors; //according to colorButtons
    QMap<QString, QColor> colorsMap;
    QList<QCheckBox *> fontCheckBoxes;
    QSettings settings;
    QString backupAssembler;
    QString backupMode;
    QString backupAssemblerOptions;
    QString backupLinkerOptions;
    QString backupObjectFileName;
    bool backupDisableLinking;
    QString backupAssemblerPath;
    QString backupStartText;
    QString backupLinkerPath;

    //about close
    bool closeFromCloseAll;
    void closeEvent(QCloseEvent *e);

public slots:
    //actions and menus
    void newFile();
    void openFile();
    void closeFile();
    bool saveFile(int index = -1);
    bool saveAsFile(int index = -1);
    void saveExe();
    bool closeApp();
    void refreshEditMenu();
    void changeCurrentSavedState(bool changed);

    //custom
    void openFile(QString path)
    {
        if (path.isEmpty())
            return;

        newFile();
        Tab *curTab = (Tab *) tabs->currentWidget();
        curTab->loadCodeFromFile(path);
        setCurrentTabName(path);
//        connect(curTab, SIGNAL(fileOpened(QString)), this, SLOT(openFile(QString)));
    }

    void otherInstanceDataReceived(QByteArray data)
    {
        this->showNormal();
        this->raise();
        this->activateWindow();

        QList<QByteArray> arguments = data.split(0x00);
        for (int i = 1; i < arguments.size() - 1; i++)
            openFile(QString(arguments[i]));
    }

    //build
    void buildProgram(bool debugMode = false);
    void runProgram();
    void runExeProgram();
    void stopProgram();
    void testStopOfProgram();
    void setProgramBuildedFlagToFalse();
    void changeCurrentTab(int index);
    void printLog(const QString & message, const QColor &color);
    void printLogWithTime(const QString & message, const QColor &color);
    void startCountProgramTime();

    //debug
    void debug();
    void enableDebugActions();
    void disableDebugActions(bool start = false);
    void debugNext();
    void debugNextNi();
    void debugExit();
    void debugToggleBreakpoint();
    void debugShowRegisters();
    void debugShowMemory();
    void debugRunCommand(QString command, bool print);
    void saveWatches(DebugTableWidget *table);
    void setShowRegistersToUnchecked();
    void setShowMemoryToUnchecked();
    void setShowMemoryToChecked(const RuQPlainTextEdit::Watch &variable);
    void showAnyCommandWidget();
    void closeAnyCommandWidget();
    void printOutput(QString msg, int index = -1);
    void getOutput();
    void changeDebugActionToStart();

    //search
    void find();
    void findNext(const QString &pattern, Qt::CaseSensitivity cs, bool all, bool replace,
                  const QString &replaceText = 0);

    //settings
    void restorePrevSession(bool notNotify = false);
    void openSettings();
    void changeMode(bool x86);
    void changeAssembler();
    void changeStartText();
    void saveSettings();
    void exitSettings();
    void changeActionsState(int widgetIndex);
    void resetAllSettings();
    void pickColor(QWidget *colorButton, bool init = false);
    void changeHighlightingFont(QWidget *box, bool init = false);
    void changeHighlightingLineMode(bool mode);
    void recreateHighlighter();
    void recreateAssembler(bool start = false);
    void initAssemblerSettings(bool firstOpening);
    void backupSettings();
    void restoreSettingsAndExit();
    void printMasmInfo();
    void enableOrDisableLinkingEdit(int disableLinkingCheckboxState);

    //closing
    bool deleteTab(int index, bool saveFileName = false);
    void closeAllChildWindows();

    //other windows
    void openHelp();
    void openAbout();

protected:
    void dragEnterEvent(QDragEnterEvent *event);

    void dropEvent(QDropEvent *event);
};

#endif // MAINWINDOW_H
