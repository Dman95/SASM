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

/**
 * Def: SASM_VERSION
 *
 * @def SASM_VERSION
 *
 * @brief A macro that defines sasm version.
 *
 * @date 07:59 04 February 2016
 */

#define SASM_VERSION "3.3.0"

/**
 * Class: MainWindow
 *
 * @class MainWindow
 *
 * @brief The application's main form.
 *
 * @date 07:59 04 February 2016
 */

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	explicit
	MainWindow(const QStringList& args, QWidget* parent = nullptr);

	/**
	 * Destructor: ~MainWindow
	 *
	 * @fn MainWindow::~MainWindow();
	 *
	 * @brief Destructor.
	 *
	 * @date 08:21 04 February 2016
	 */

	~MainWindow();

	/**
	 * Method: initUi
	 *
	 * @fn void MainWindow::initUi();
	 *
	 * @brief Initializes the user interface.
	 *
	 * @date 08:21 04 February 2016
	 */

	void
	initUi();

	/**
	 * Method: createMenus
	 *
	 * @fn void MainWindow::createMenus();
	 *
	 * @brief Creates the menus.
	 *
	 * @date 08:22 04 February 2016
	 */

	void
	createMenus();

	/**
	 * Method: createActions
	 *
	 * @fn void MainWindow::createActions();
	 *
	 * @brief Creates the actions.
	 *
	 * @date 08:22 04 February 2016
	 */

	void
	createActions();

	/**
	 * Method: createButtons
	 *
	 * @fn void MainWindow::createButtons();
	 *
	 * @brief Creates the buttons.
	 *
	 * @date 08:22 04 February 2016
	 */

	void
	createButtons();

	/**
	 * Method: createToolBars
	 *
	 * @fn void MainWindow::createToolBars();
	 *
	 * @brief Creates tool bars.
	 *
	 * @date 08:22 04 February 2016
	 */

	void
	createToolBars();

	/**
	 * Method: writeSettings
	 *
	 * @fn void MainWindow::writeSettings();
	 *
	 * @brief Writes the settings.
	 *
	 * @date 08:22 04 February 2016
	 */

	void
	writeSettings();

	/**
	 * Method: setupEditor
	 *
	 * @fn void MainWindow::setupEditor(int i);
	 *
	 * @brief Sets up the editor.
	 *
	 * @date 08:22 04 February 2016
	 *
	 * @param i Zero-based index of the.
	 */

	void
	setupEditor(int i);

	/**
	 * Method: okToContinue
	 *
	 * @fn bool MainWindow::okToContinue(int index = -1);
	 *
	 * @brief Ok to continue.
	 *
	 * @date 08:22 04 February 2016
	 *
	 * @param index Zero-based index of the.
	 *
	 * @return true if it succeeds, false if it fails.
	 */

	bool
	okToContinue(int index = -1);

	/**
	 * Method: setCurrentTabName
	 *
	 * @fn void MainWindow::setCurrentTabName(const QString& filePath, int index = -1);
	 *
	 * @brief Sets current tab name.
	 *
	 * @date 08:22 04 February 2016
	 *
	 * @param filePath Full pathname of the file.
	 * @param index    Zero-based index of the.
	 */

	void
	setCurrentTabName(const QString& filePath, int index = -1);

	/**
	 * Method: removeDirRecuresively
	 *
	 * @fn bool MainWindow::removeDirRecuresively(const QString& dirName);
	 *
	 * @brief Removes the dir recursively described by dirName.
	 *
	 * @date 08:22 04 February 2016
	 *
	 * @param dirName Pathname of the directory.
	 *
	 * @return true if it succeeds, false if it fails.
	 */

	bool
	removeDirRecuresively(const QString& dirName);

private:
	/** @brief ui. */
	GetStartedWidget* getStartedWidget;
	/** @brief The main widget. */
	QStackedWidget* mainWidget;
	/** @brief The splitter. */
	QSplitter* splitter;
	/** @brief The work layout. */
	QVBoxLayout* workLayout;
	/** @brief The work widget. */
	QWidget* workWidget;
	/** @brief The compiler out. */
	RuQTextEdit* compilerOut;
	/** @brief The tabs. */
	QTabWidget* tabs;

	/** @brief menus and actions. */
	QMenu* fileMenu;
	/** @brief The edit menu. */
	QMenu* editMenu;
	/** @brief The debug menu. */
	QMenu* debugMenu;
	/** @brief The build menu. */
	QMenu* buildMenu;
	/** @brief The settings menu. */
	QMenu* settingsMenu;
	/** @brief The help menu. */
	QMenu* helpMenu;
	/** @brief The new action. */
	QAction* newAction;
	/** @brief The open action. */
	QAction* openAction;
	/** @brief The close action. */
	QAction* closeAction;
	/** @brief The save action. */
	QAction* saveAction;
	/** @brief The save as action. */
	QAction* saveAsAction;
	/** @brief The save executable action. */
	QAction* saveExeAction;
	/** @brief The exit action. */
	QAction* exitAction;
	/** @brief The find action. */
	QAction* findAction;
	/** @brief The comment action. */
	QAction* commentAction;
	/** @brief The uncomment action. */
	QAction* uncommentAction;
	/** @brief The undo action. */
	QAction* undoAction;
	/** @brief The redo action. */
	QAction* redoAction;
	/** @brief The cut action. */
	QAction* cutAction;
	/** @brief The copy action. */
	QAction* copyAction;
	/** @brief The paste action. */
	QAction* pasteAction;
	/** @brief The delete action. */
	QAction* deleteAction;
	/** @brief The select all action. */
	QAction* selectAllAction;
	/** @brief The put tab action. */
	QAction* putTabAction;
	/** @brief The delete tab action. */
	QAction* deleteTabAction;
	/** @brief The build action. */
	QAction* buildAction;
	/** @brief The run action. */
	QAction* runAction;
	/** @brief The run executable action. */
	QAction* runExeAction;
	/** @brief The stop action. */
	QAction* stopAction;
	/** @brief The debug action. */
	QAction* debugAction;
	/** @brief The debug next action. */
	QAction* debugNextAction;
	/** @brief The debug next ni action. */
	QAction* debugNextNiAction;
	/** @brief The debug toggle breakpoint action. */
	QAction* debugToggleBreakpointAction;
	/** @brief The debug show registers action. */
	QAction* debugShowRegistersAction;
	/** @brief The debug show memory action. */
	QAction* debugShowMemoryAction;
	/** @brief The settings action. */
	QAction* settingsAction;
	/** @brief The help action. */
	QAction* helpAction;
	/** @brief The about action. */
	QAction* aboutAction;

	/** @brief toolbars. */
	QToolBar* fileToolBar;
	/** @brief The edit tool bar. */
	QToolBar* editToolBar;
	/** @brief The build tool bar. */
	QToolBar* buildToolBar;
	/** @brief The debug tool bar. */
	QToolBar* debugToolBar;

	/** @brief builder and debugger and all that concern to them. */
	QProcess* runProcess;
	/** @brief The previous code editor. */
	CodeEditor* prevCodeEditor;
	/** @brief The timer. */
	QTimer* timer;
	/** @brief The program execution time. */
	QTime programExecutionTime;
	/** @brief The debugger. */
	Debugger* debugger;
	/** @brief true if program is builded. */
	bool programIsBuilded;
	/** @brief The registers window. */
	QPointer<DebugTableWidget> registersWindow;
	/** @brief The registers dock. */
	QDockWidget* registersDock;
	/** @brief The memory window. */
	QPointer<DebugTableWidget> memoryWindow;
	/** @brief The memory dock. */
	QDockWidget* memoryDock;
	/** @brief The watches. */
	QList<RuQPlainTextEdit::Watch> watches;
	/** @brief The debug any command widget. */
	DebugAnyCommandWidget* debugAnyCommandWidget;
	/** @brief true if program stopped. */
	bool programStopped;
	/** @brief Zero-based index of the output. */
	int outputIndex;
	/** @brief The assembler. */
	Assembler* assembler;
	/** @brief true if debugger was started. */
	bool debuggerWasStarted;
	/** @brief The debug key. */
	QString debugKey;

	/** @brief highlighters. */
	Highlighter* highlighter;
	/** @brief The settings highlighter. */
	Highlighter* settingsHighlighter;

	/** @brief search. */
	QPointer<FindDialog> findDialog;
	/** @brief The previous create struct. */
	Qt::CaseSensitivity prevCs;

	/** @brief settings and help. */
	QPointer<QWidget> settingsWindow;
	/** @brief The settings user interface. */
	Ui::SettingsWindow settingsUi;
	/** @brief The start text. */
	QString startText;
	/** @brief The settings start text editor. */
	CodeEditor* settingsStartTextEditor;
	/** @brief save and open. */
	QString saveOpenDirectory;
	/** @brief The help. */
	QPointer<QTextBrowser> help;
	/** @brief The color signal mapper. */
	QSignalMapper* colorSignalMapper;
	/** @brief The fonts signal mapper. */
	QSignalMapper* fontsSignalMapper;
	/** @brief The color buttons. */
	QList<QPushButton *> colorButtons;
	/** @brief according to colorButtons. */
	QList<QColor> defaultColors;
	/** @brief The colors map. */
	QMap<QString, QColor> colorsMap;
	/** @brief The font check boxes. */
	QList<QCheckBox *> fontCheckBoxes;
	/** @brief Options for controlling the operation. */
	QSettings settings;
	/** @brief The backup assembler. */
	QString backupAssembler;
	/** @brief The backup mode. */
	QString backupMode;
	/** @brief Options for controlling the backup assembler. */
	QString backupAssemblerOptions;
	/** @brief Options for controlling the backup linker. */
	QString backupLinkerOptions;
	/** @brief true to backup disable linking. */
	bool backupDisableLinking;
	/** @brief Full pathname of the backup assembler file. */
	QString backupAssemblerPath;
	/** @brief The backup start text. */
	QString backupStartText;
	/** @brief Full pathname of the backup linker file. */
	QString backupLinkerPath;

	/** @brief about close. */
	bool closeFromCloseAll;
	void
	closeEvent(QCloseEvent* e) override;

public slots:

	/**
	 * Method: newFile
	 *
	 * @fn void MainWindow::newFile();
	 *
	 * @brief actions and menus.
	 *
	 * @date 08:05 04 February 2016
	 */

	void
	newFile();

	/**
	 * Method: openFile
	 *
	 * @fn void MainWindow::openFile();
	 *
	 * @brief Opens the file.
	 *
	 * @date 08:06 04 February 2016
	 */

	void
	openFile();

	/**
	 * Method: closeFile
	 *
	 * @fn void MainWindow::closeFile();
	 *
	 * @brief Closes the file.
	 *
	 * @date 08:06 04 February 2016
	 */

	void
	closeFile();

	/**
	 * Method: saveFile
	 *
	 * @fn bool MainWindow::saveFile(int index = -1);
	 *
	 * @brief Saves a file.
	 *
	 * @date 08:06 04 February 2016
	 *
	 * @param index Zero-based index of the.
	 *
	 * @return true if it succeeds, false if it fails.
	 */

	bool
	saveFile(int index = -1);

	/**
	 * Method: saveAsFile
	 *
	 * @fn bool MainWindow::saveAsFile(int index = -1);
	 *
	 * @brief Saves as file.
	 *
	 * @date 08:06 04 February 2016
	 *
	 * @param index Zero-based index of the.
	 *
	 * @return true if it succeeds, false if it fails.
	 */

	bool
	saveAsFile(int index = -1);

	/**
	 * Method: saveExe
	 *
	 * @fn void MainWindow::saveExe();
	 *
	 * @brief Saves the executable.
	 *
	 * @date 08:06 04 February 2016
	 */

	void
	saveExe();

	/**
	 * Method: closeApp
	 *
	 * @fn bool MainWindow::closeApp();
	 *
	 * @brief Closes the application.
	 *
	 * @date 08:06 04 February 2016
	 *
	 * @return true if it succeeds, false if it fails.
	 */

	bool
	closeApp();

	/**
	 * Method: refreshEditMenu
	 *
	 * @fn void MainWindow::refreshEditMenu();
	 *
	 * @brief Refresh edit menu.
	 *
	 * @date 08:07 04 February 2016
	 */

	void
	refreshEditMenu();

	/**
	 * Method: changeCurrentSavedState
	 *
	 * @fn void MainWindow::changeCurrentSavedState(bool changed);
	 *
	 * @brief Change current saved state.
	 *
	 * @date 08:07 04 February 2016
	 *
	 * @param changed true if the data is changed.
	 */

	void
	changeCurrentSavedState(bool changed);

	/**
	 * Method: openFile
	 *
	 * @fn void MainWindow::openFile(QString path)
	 *
	 * @brief custom.
	 *
	 * @date 08:07 04 February 2016
	 *
	 * @param path Full pathname of the file.
	 */

	void
	openFile(QString path) {
		if (path.isEmpty())
			return;

		newFile();
		/** @brief The current tab. */
		Tab* curTab = (Tab *) tabs->currentWidget();
		curTab->loadCodeFromFile(path);
		setCurrentTabName(path);
		//        connect(curTab, SIGNAL(fileOpened(QString)), this, SLOT(openFile(QString)));
	}

	/**
	 * Method: otherInstanceDataReceived
	 *
	 * @fn void MainWindow::otherInstanceDataReceived(QByteArray data)
	 *
	 * @brief Other instance data received.
	 *
	 * @date 08:09 04 February 2016
	 *
	 * @param data The data.
	 */

	void
	otherInstanceDataReceived(QByteArray data) {
		this->showNormal();
		this->raise();
		this->activateWindow();

		/** @brief The arguments. */
		QList<QByteArray> arguments = data.split(0x00);
		for (int i = 1; i < arguments.size() - 1; i++)
			openFile(QString(arguments[i]));
	}

	/**
	 * Method: buildProgram
	 *
	 * @fn void MainWindow::buildProgram(bool debugMode = false);
	 *
	 * @brief build.
	 *
	 * @date 08:09 04 February 2016
	 *
	 * @param debugMode true to enable debug mode, false to disable it.
	 */

	void
	buildProgram(bool debugMode = false);

	/**
	 * Method: runProgram
	 *
	 * @fn void MainWindow::runProgram();
	 *
	 * @brief Executes the program operation.
	 *
	 * @date 08:09 04 February 2016
	 */

	void
	runProgram();

	/**
	 * Method: runExeProgram
	 *
	 * @fn void MainWindow::runExeProgram();
	 *
	 * @brief Executes the executable program operation.
	 *
	 * @date 08:09 04 February 2016
	 */

	void
	runExeProgram();

	/**
	 * Method: stopProgram
	 *
	 * @fn void MainWindow::stopProgram();
	 *
	 * @brief Stops a program.
	 *
	 * @date 08:09 04 February 2016
	 */

	void
	stopProgram();

	/**
	 * Method: testStopOfProgram
	 *
	 * @fn void MainWindow::testStopOfProgram();
	 *
	 * @brief Tests stop of program.
	 *
	 * @date 08:09 04 February 2016
	 */

	void
	testStopOfProgram();

	/**
	 * Method: setProgramBuildedFlagToFalse
	 *
	 * @fn void MainWindow::setProgramBuildedFlagToFalse();
	 *
	 * @brief Sets program builded flag to false.
	 *
	 * @date 08:09 04 February 2016
	 */

	void
	setProgramBuildedFlagToFalse();

	/**
	 * Method: changeCurrentTab
	 *
	 * @fn void MainWindow::changeCurrentTab(int index);
	 *
	 * @brief Change current tab.
	 *
	 * @date 08:09 04 February 2016
	 *
	 * @param index Zero-based index of the.
	 */

	void
	changeCurrentTab(int index);

	/**
	 * Method: printLog
	 *
	 * @fn void MainWindow::printLog(const QString& message, const QColor& color);
	 *
	 * @brief Print log.
	 *
	 * @date 08:09 04 February 2016
	 *
	 * @param message The message.
	 * @param color   The color.
	 */

	void
	printLog(const QString& message, const QColor& color);

	/**
	 * Method: printLogWithTime
	 *
	 * @fn void MainWindow::printLogWithTime(const QString& message, const QColor& color);
	 *
	 * @brief Print log with time.
	 *
	 * @date 08:09 04 February 2016
	 *
	 * @param message The message.
	 * @param color   The color.
	 */

	void
	printLogWithTime(const QString& message, const QColor& color);

	/**
	 * Method: startCountProgramTime
	 *
	 * @fn void MainWindow::startCountProgramTime();
	 *
	 * @brief Starts count program time.
	 *
	 * @date 08:09 04 February 2016
	 */

	void
	startCountProgramTime();

	/**
	 * Method: debug
	 *
	 * @fn void MainWindow::debug();
	 *
	 * @brief debug.
	 *
	 * @date 08:10 04 February 2016
	 */

	void
	debug();

	/**
	 * Method: enableDebugActions
	 *
	 * @fn void MainWindow::enableDebugActions();
	 *
	 * @brief Enables the debug actions.
	 *
	 * @date 08:10 04 February 2016
	 */

	void
	enableDebugActions();

	/**
	 * Method: disableDebugActions
	 *
	 * @fn void MainWindow::disableDebugActions(bool start = false);
	 *
	 * @brief Disables the debug actions.
	 *
	 * @date 08:10 04 February 2016
	 *
	 * @param start true to start.
	 */

	void
	disableDebugActions(bool start = false);

	/**
	 * Method: debugNext
	 *
	 * @fn void MainWindow::debugNext();
	 *
	 * @brief Debug next.
	 *
	 * @date 08:10 04 February 2016
	 */

	void
	debugNext();

	/**
	 * Method: debugNextNi
	 *
	 * @fn void MainWindow::debugNextNi();
	 *
	 * @brief Debug next ni.
	 *
	 * @date 08:10 04 February 2016
	 */

	void
	debugNextNi();

	/**
	 * Method: debugExit
	 *
	 * @fn void MainWindow::debugExit();
	 *
	 * @brief Debug exit.
	 *
	 * @date 08:10 04 February 2016
	 */

	void
	debugExit();

	/**
	 * Method: debugToggleBreakpoint
	 *
	 * @fn void MainWindow::debugToggleBreakpoint();
	 *
	 * @brief Debug toggle breakpoint.
	 *
	 * @date 08:10 04 February 2016
	 */

	void
	debugToggleBreakpoint();

	/**
	 * Method: debugShowRegisters
	 *
	 * @fn void MainWindow::debugShowRegisters();
	 *
	 * @brief Debug show registers.
	 *
	 * @date 08:10 04 February 2016
	 */

	void
	debugShowRegisters();

	/**
	 * Method: debugShowMemory
	 *
	 * @fn void MainWindow::debugShowMemory();
	 *
	 * @brief Debug show memory.
	 *
	 * @date 08:10 04 February 2016
	 */

	void
	debugShowMemory();

	/**
	 * Method: debugRunCommand
	 *
	 * @fn void MainWindow::debugRunCommand(QString command, bool print);
	 *
	 * @brief Debug run command.
	 *
	 * @date 08:10 04 February 2016
	 *
	 * @param command The command.
	 * @param print   true to print.
	 */

	void
	debugRunCommand(QString command, bool print);

	/**
	 * Method: saveWatches
	 *
	 * @fn void MainWindow::saveWatches(DebugTableWidget* table);
	 *
	 * @brief Saves the watches.
	 *
	 * @date 08:10 04 February 2016
	 *
	 * @param [in,out] table If non-null, the table.
	 */

	void
	saveWatches(DebugTableWidget* table);

	/**
	 * Method: setShowRegistersToUnchecked
	 *
	 * @fn void MainWindow::setShowRegistersToUnchecked();
	 *
	 * @brief Sets show registers to unchecked.
	 *
	 * @date 08:10 04 February 2016
	 */

	void
	setShowRegistersToUnchecked();

	/**
	 * Method: setShowMemoryToUnchecked
	 *
	 * @fn void MainWindow::setShowMemoryToUnchecked();
	 *
	 * @brief Sets show memory to unchecked.
	 *
	 * @date 08:10 04 February 2016
	 */

	void
	setShowMemoryToUnchecked();

	/**
	 * Method: setShowMemoryToChecked
	 *
	 * @fn void MainWindow::setShowMemoryToChecked(const RuQPlainTextEdit::Watch& variable);
	 *
	 * @brief Sets show memory to checked.
	 *
	 * @date 08:10 04 February 2016
	 *
	 * @param variable The variable.
	 */

	void
	setShowMemoryToChecked(const RuQPlainTextEdit::Watch& variable);

	/**
	 * Method: showAnyCommandWidget
	 *
	 * @fn void MainWindow::showAnyCommandWidget();
	 *
	 * @brief Shows any command widget.
	 *
	 * @date 08:10 04 February 2016
	 */

	void
	showAnyCommandWidget();

	/**
	 * Method: closeAnyCommandWidget
	 *
	 * @fn void MainWindow::closeAnyCommandWidget();
	 *
	 * @brief Closes any command widget.
	 *
	 * @date 08:10 04 February 2016
	 */

	void
	closeAnyCommandWidget();

	/**
	 * Method: printOutput
	 *
	 * @fn void MainWindow::printOutput(QString msg, int index = -1);
	 *
	 * @brief Print output.
	 *
	 * @date 08:10 04 February 2016
	 *
	 * @param msg   The message.
	 * @param index Zero-based index of the.
	 */

	void
	printOutput(QString msg, int index = -1);

	/**
	 * Method: getOutput
	 *
	 * @fn void MainWindow::getOutput();
	 *
	 * @brief Gets the output.
	 *
	 * @date 08:11 04 February 2016
	 */

	void
	getOutput();

	/**
	 * Method: changeDebugActionToStart
	 *
	 * @fn void MainWindow::changeDebugActionToStart();
	 *
	 * @brief Change debug action to start.
	 *
	 * @date 08:11 04 February 2016
	 */

	void
	changeDebugActionToStart();

	/**
	 * Method: find
	 *
	 * @fn void MainWindow::find();
	 *
	 * @brief Search.
	 *
	 * @date 08:11 04 February 2016
	 */

	void
	find();

	/**
	 * Method: findNext
	 *
	 * @fn void MainWindow::findNext(const QString& pattern, Qt::CaseSensitivity cs, bool all, bool replace, const QString& replaceText = 0);
	 *
	 * @brief Finds the next item.
	 *
	 * @date 08:11 04 February 2016
	 *
	 * @param pattern	  Specifies the pattern.
	 * @param cs		  The create struct.
	 * @param all		  true to all.
	 * @param replace	  true to replace.
	 * @param replaceText The replace text.
	 */

	void
	findNext(const QString& pattern, Qt::CaseSensitivity cs, bool all, bool replace,
	              const QString& replaceText = 0);

	/**
	 * Method: restorePrevSession
	 *
	 * @fn void MainWindow::restorePrevSession(bool notNotify = false);
	 *
	 * @brief Settings.
	 *
	 * @date 08:11 04 February 2016
	 *
	 * @param notNotify true to not notify.
	 */

	void
	restorePrevSession(bool notNotify = false);

	/**
	 * Method: openSettings
	 *
	 * @fn void MainWindow::openSettings();
	 *
	 * @brief Opens the settings.
	 *
	 * @date 08:11 04 February 2016
	 */

	void
	openSettings();

	/**
	 * Method: changeMode
	 *
	 * @fn void MainWindow::changeMode(bool x86);
	 *
	 * @brief Change mode.
	 *
	 * @date 08:11 04 February 2016
	 *
	 * @param x86 true to 86.
	 */

	void
	changeMode(bool x86);

	/**
	 * Method: changeAssembler
	 *
	 * @fn void MainWindow::changeAssembler();
	 *
	 * @brief Change assembler.
	 *
	 * @date 08:11 04 February 2016
	 */

	void
	changeAssembler();

	/**
	 * Method: changeStartText
	 *
	 * @fn void MainWindow::changeStartText();
	 *
	 * @brief Change start text.
	 *
	 * @date 08:12 04 February 2016
	 */

	void
	changeStartText();

	/**
	 * Method: saveSettings
	 *
	 * @fn void MainWindow::saveSettings();
	 *
	 * @brief Saves the settings.
	 *
	 * @date 08:12 04 February 2016
	 */

	void
	saveSettings();

	/**
	 * Method: exitSettings
	 *
	 * @fn void MainWindow::exitSettings();
	 *
	 * @brief Exit settings.
	 *
	 * @date 08:12 04 February 2016
	 */

	void
	exitSettings();

	/**
	 * Method: changeActionsState
	 *
	 * @fn void MainWindow::changeActionsState(int widgetIndex);
	 *
	 * @brief Change actions state.
	 *
	 * @date 08:12 04 February 2016
	 *
	 * @param widgetIndex Zero-based index of the widget.
	 */

	void
	changeActionsState(int widgetIndex);

	/**
	 * Method: resetAllSettings
	 *
	 * @fn void MainWindow::resetAllSettings();
	 *
	 * @brief Resets all settings.
	 *
	 * @date 08:12 04 February 2016
	 */

	void
	resetAllSettings();

	/**
	 * Method: pickColor
	 *
	 * @fn void MainWindow::pickColor(QWidget* colorButton, bool init = false);
	 *
	 * @brief Picks the color.
	 *
	 * @date 08:12 04 February 2016
	 *
	 * @param [in,out] colorButton If non-null, the color button.
	 * @param init				   true to initialize.
	 */

	void
	pickColor(QWidget* colorButton, bool init = false);

	/**
	 * Method: changeHighlightingFont
	 *
	 * @fn void MainWindow::changeHighlightingFont(QWidget* box, bool init = false);
	 *
	 * @brief Change highlighting font.
	 *
	 * @date 08:12 04 February 2016
	 *
	 * @param [in,out] box If non-null, the box.
	 * @param init		   true to initialize.
	 */

	void
	changeHighlightingFont(QWidget* box, bool init = false);

	/**
	 * Method: changeHighlightingLineMode
	 *
	 * @fn void MainWindow::changeHighlightingLineMode(bool mode);
	 *
	 * @brief Change highlighting line mode.
	 *
	 * @date 08:12 04 February 2016
	 *
	 * @param mode true to mode.
	 */

	void
	changeHighlightingLineMode(bool mode);

	/**
	 * Method: recreateHighlighter
	 *
	 * @fn void MainWindow::recreateHighlighter();
	 *
	 * @brief Recreate highlighter.
	 *
	 * @date 08:12 04 February 2016
	 */

	void
	recreateHighlighter();

	/**
	 * Method: recreateAssembler
	 *
	 * @fn void MainWindow::recreateAssembler(bool start = false);
	 *
	 * @brief Recreate assembler.
	 *
	 * @date 08:12 04 February 2016
	 *
	 * @param start true to start.
	 */

	void
	recreateAssembler(bool start = false);

	/**
	 * Method: initAssemblerSettings
	 *
	 * @fn void MainWindow::initAssemblerSettings(bool firstOpening);
	 *
	 * @brief Initializes the assembler settings.
	 *
	 * @date 08:12 04 February 2016
	 *
	 * @param firstOpening true to first opening.
	 */

	void
	initAssemblerSettings(bool firstOpening);

	/**
	 * Method: backupSettings
	 *
	 * @fn void MainWindow::backupSettings();
	 *
	 * @brief Backup settings.
	 *
	 * @date 08:12 04 February 2016
	 */

	void
	backupSettings();

	/**
	 * Method: restoreSettingsAndExit
	 *
	 * @fn void MainWindow::restoreSettingsAndExit();
	 *
	 * @brief Restore settings and exit.
	 *
	 * @date 08:12 04 February 2016
	 */

	void
	restoreSettingsAndExit();

	/**
	 * Method: printMasmInfo
	 *
	 * @fn void MainWindow::printMasmInfo();
	 *
	 * @brief Print masm information.
	 *
	 * @date 08:12 04 February 2016
	 */

	void
	printMasmInfo();

	/**
	 * Method: enableOrDisableLinkingEdit
	 *
	 * @fn void MainWindow::enableOrDisableLinkingEdit(int disableLinkingCheckboxState);
	 *
	 * @brief Enables the or disable linking edit.
	 *
	 * @date 08:12 04 February 2016
	 *
	 * @param disableLinkingCheckboxState State of the disable linking checkbox.
	 */

	void
	enableOrDisableLinkingEdit(int disableLinkingCheckboxState);

	/**
	 * Method: deleteTab
	 *
	 * @fn bool MainWindow::deleteTab(int index, bool saveFileName = false);
	 *
	 * @brief closing.
	 *
	 * @date 08:12 04 February 2016
	 *
	 * @param index		   Zero-based index of the.
	 * @param saveFileName true to save file name.
	 *
	 * @return true if it succeeds, false if it fails.
	 */

	bool
	deleteTab(int index, bool saveFileName = false);

	/**
	 * Method: closeAllChildWindows
	 *
	 * @fn void MainWindow::closeAllChildWindows();
	 *
	 * @brief Closes all child windows.
	 *
	 * @date 08:12 04 February 2016
	 */

	void
	closeAllChildWindows();

	/**
	 * Method: openHelp
	 *
	 * @fn void MainWindow::openHelp();
	 *
	 * @brief other windows.
	 *
	 * @date 08:12 04 February 2016
	 */

	void
	openHelp();

	/**
	 * Method: openAbout
	 *
	 * @fn void MainWindow::openAbout();
	 *
	 * @brief Opens the about.
	 *
	 * @date 08:12 04 February 2016
	 */

	void
	openAbout();

protected:

	/**
	 * Method: dragEnterEvent
	 *
	 * @fn void MainWindow::dragEnterEvent(QDragEnterEvent* event);
	 *
	 * @brief Drag enter event.
	 *
	 * @date 08:13 04 February 2016
	 *
	 * @param [in,out] event If non-null, the event.
	 */

	void
	dragEnterEvent(QDragEnterEvent* event) override;

	/**
	 * Method: dropEvent
	 *
	 * @fn void MainWindow::dropEvent(QDropEvent* event);
	 *
	 * @brief Drop event.
	 *
	 * @date 08:13 04 February 2016
	 *
	 * @param [in,out] event If non-null, the event.
	 */

	void
	dropEvent(QDropEvent* event) override;
};

#endif // MAINWINDOW_H


