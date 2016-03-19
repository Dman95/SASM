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

#include "mainwindow.h"

/**
 * @file mainwindow.cpp
 * Code for all of the windows and features.
 */

MainWindow::MainWindow(const QStringList &args, QWidget *parent)
    : QMainWindow(parent), settings("SASM Project", "SASM")
{
    setWindowTitle("SASM");
    setWindowIcon(QIcon(":images/mainIcon.png"));

    QObject::connect(
                QApplication::instance(),
                SIGNAL(otherInstanceDataReceived(QByteArray)),
                this,
                SLOT(otherInstanceDataReceived(QByteArray))
    );

    //! Set save and open directory
    saveOpenDirectory = settings.value("saveopendirectory", QString(Common::applicationDataPath() + "/Projects")).toString();

    //! Initial variables
    programIsBuilded = false;
    prevCodeEditor = 0;
    findDialog = 0;
    settingsWindow = 0;
    closeFromCloseAll = false;
    settingsStartTextEditor = 0;
    help = 0;
    registersWindow = 0;
    memoryWindow = 0;
    debugger = 0;
    programStopped = true;
    highlighter = 0;
    tabs = 0;
    memoryDock = 0;
    registersDock = 0;

    //! Initialize assembler
    assembler = 0;
    recreateAssembler(true);

    //! Set code field start text
    startText = settings.value("starttext", QString()).toString();
    if (startText.isEmpty()) {
        settings.setValue("starttext", assembler->getStartText());
        startText = assembler->getStartText();
    }

    timer = new QTimer;
    timer->setInterval(100);
    timer->stop();
    connect(timer, SIGNAL(timeout()), this, SLOT(testStopOfProgram()));
    runProcess = new QProcess();

    initUi();
    createActions();
    createMenus();
    createToolBars();
    refreshEditMenu();
    setAcceptDrops(true);

    //! Restore log splitter state
    splitter->restoreState(settings.value("logsplitterstate").toByteArray());

    //! Open documents from command line
    for (int i = 1; i < args.size(); i++)
        openFile(args[i]);
}

void MainWindow::enableOrDisableLinkingEdit(int disableLinkingCheckboxState)
{
    bool enabled = (disableLinkingCheckboxState == Qt::Unchecked);
    settingsUi.linkingOptionsEdit->setEnabled(enabled);
    settingsUi.linkerPathEdit->setEnabled(enabled);
}

void MainWindow::initUi()
{
    //! Resize
    settings.beginGroup("MainWindow");
    resize(settings.value("size", QSize(1024, 650)).toSize()); //default 1024x650
    move(settings.value("pos", QPoint(200, 200)).toPoint());
    //! Set maximized
    setWindowState(windowState() | (Qt::WindowState) settings.value("maximized", (int) Qt::WindowNoState).toInt());
    settings.endGroup();

    //! Get Started window
    getStartedWidget = new GetStartedWidget;
    connect(getStartedWidget->newButton, SIGNAL(clicked()), this, SLOT(newFile()));
    connect(getStartedWidget->openButton, SIGNAL(clicked()), this, SLOT(openFile()));
    connect(getStartedWidget->prevSessionButton, SIGNAL(clicked()), this, SLOT(restorePrevSession()));

    //! Create form
    splitter = new QSplitter;
    splitter->setOrientation(Qt::Vertical);
    workLayout = new QVBoxLayout;
    workLayout->addWidget(splitter);
    workLayout->setMargin(0);
    workLayout->setSpacing(0);
    workWidget = new QWidget;
    workWidget->setLayout(workLayout);

    //! Stacked widget
    mainWidget = new QStackedWidget;
    mainWidget->addWidget(getStartedWidget);
    mainWidget->addWidget(workWidget);
    mainWidget->setCurrentIndex(0); //get started
    setCentralWidget(mainWidget);

    //! Create highlighter
    highlighter = new Highlighter(assembler);

    //! Create tabs
    tabs = new QTabWidget;
    connect(tabs, SIGNAL(tabCloseRequested(int)), this, SLOT(deleteTab(int)));
    connect(tabs, SIGNAL(currentChanged(int)), this, SLOT(changeCurrentTab(int)));
    tabs->setTabsClosable(true);
    tabs->setMovable(true);

    //! Create compiler field
    compilerOut = new RuQTextEdit;
    compilerOut->setReadOnly(true);
    QFont compilerOutFont;
    compilerOutFont.setPointSize(settings.value("fontsize", 12).toInt());
    compilerOut->setFont(compilerOutFont);
    compilerOut->setText(tr("Build log:") + '\n');

    //! Create gdb command widget
    debugAnyCommandWidget = new DebugAnyCommandWidget;

    //! Add widgets on splitter
    splitter->addWidget(tabs);
    splitter->addWidget(compilerOut);
    workLayout->addWidget(debugAnyCommandWidget);
    int compilerOutSize = 100;
    debugAnyCommandWidget->close();
    debugAnyCommandWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    splitter->setSizes(QList<int>() << splitter->size().height() - compilerOutSize << compilerOutSize);
    //! Restore previous session if it in settings
    if (settings.value("startwindow", 0).toInt() == 1)
        restorePrevSession(true);
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("File"));
    fileMenu->addAction(newAction);
    fileMenu->addSeparator();
    fileMenu->addAction(openAction);
    fileMenu->addAction(closeAction);
    fileMenu->addSeparator();
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);
    fileMenu->addAction(saveExeAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);
    editMenu = menuBar()->addMenu(tr("Edit"));
    editMenu->addAction(findAction);
    editMenu->addSeparator();
    editMenu->addAction(undoAction);
    editMenu->addAction(redoAction);
    editMenu->addSeparator();
    editMenu->addAction(cutAction);
    editMenu->addAction(copyAction);
    editMenu->addAction(pasteAction);
    editMenu->addAction(deleteAction);
    editMenu->addSeparator();
    editMenu->addAction(selectAllAction);
    editMenu->addSeparator();
    editMenu->addAction(commentAction);
    editMenu->addAction(uncommentAction);
    editMenu->addSeparator();
    editMenu->addAction(putTabAction);
    editMenu->addAction(deleteTabAction);
    connect(editMenu, SIGNAL(aboutToShow()), this, SLOT(refreshEditMenu()));
    connect(tabs, SIGNAL(currentChanged(int)), this, SLOT(refreshEditMenu()));
    buildMenu = menuBar()->addMenu(tr("Build"));
    buildMenu->addAction(buildAction);
    buildMenu->addSeparator();
    buildMenu->addAction(runAction);
    #ifdef Q_OS_WIN32
        buildMenu->addAction(runExeAction);
    #endif
    buildMenu->addSeparator();
    buildMenu->addAction(stopAction);
    debugMenu = menuBar()->addMenu(tr("Debug"));
    debugMenu->addAction(debugAction);
    debugMenu->addSeparator();
    debugMenu->addAction(debugNextNiAction);
    debugMenu->addAction(debugNextAction);
    debugMenu->addSeparator();
    debugMenu->addAction(debugToggleBreakpointAction);
    debugMenu->addAction(debugShowRegistersAction);
    debugMenu->addAction(debugShowMemoryAction);
    debugMenu->addSeparator();
    debugMenu->addAction(stopAction);
    settingsMenu = menuBar()->addMenu(tr("Settings"));
    settingsMenu->addAction(settingsAction);
    helpMenu = menuBar()->addMenu(tr("Help"));
    helpMenu->addAction(helpAction);
    helpMenu->addAction(aboutAction);
}

void MainWindow::createActions()
{
    QSettings keySettings(Common::applicationDataPath() + "/keys.ini", QSettings::IniFormat);

    newAction = new QAction(QIcon(":/images/new.png"), tr("New"), this);
    QString key = keySettings.value("new", "default").toString();
    QKeySequence stdKey(QKeySequence::New);
    if (key == "default")
        key = stdKey.toString();
    newAction->setShortcut(key);
    connect(newAction, SIGNAL(triggered()), this, SLOT(newFile()));

    openAction = new QAction(QIcon(":/images/open.png"), tr("Open"), this);
    key = keySettings.value("open", "default").toString();
    stdKey = QKeySequence(QKeySequence::Open);
    if (key == "default")
        key = stdKey.toString();
    openAction->setShortcut(key);
    connect(openAction, SIGNAL(triggered()), this, SLOT(openFile()));

    closeAction = new QAction(tr("Close file"), this);
    key = keySettings.value("close", "default").toString();
    stdKey = QKeySequence(QKeySequence::Close);
    if (key == "default")
        key = stdKey.toString();
    closeAction->setShortcut(key);
    connect(closeAction, SIGNAL(triggered()), this, SLOT(closeFile()));

    
    saveAction = new QAction(QIcon(":/images/save.png"), tr("Save"), this);
    key = keySettings.value("save", "default").toString();
    stdKey = QKeySequence(QKeySequence::Save);
    if (key == "default")
        key = stdKey.toString();
    saveAction->setShortcut(key);
    connect(saveAction, SIGNAL(triggered()), this, SLOT(saveFile()));

    saveAsAction = new QAction(tr("Save as"), this);
    key = keySettings.value("saveAs", "default").toString();
    stdKey = QKeySequence(QKeySequence::SaveAs);
    if (key == "default")
        key = stdKey.toString();
    saveAsAction->setShortcut(key);
    connect(saveAsAction, SIGNAL(triggered()), this, SLOT(saveAsFile()));

    saveExeAction = new QAction(tr("Save .exe"), this);
    key = keySettings.value("save_exe", "default").toString();
    if (key == "default")
        key = "Ctrl+Shift+E";
    saveExeAction->setShortcut(QKeySequence(key));
    connect(saveExeAction, SIGNAL(triggered()), this, SLOT(saveExe()));

    exitAction = new QAction(tr("Exit"), this);
    key = keySettings.value("exit", "default").toString();
    stdKey = QKeySequence(QString("Ctrl+Q"));
    if (key == "default")
        key = stdKey.toString();
    exitAction->setShortcut(key);
    connect(exitAction, SIGNAL(triggered()), this, SLOT(closeApp()));

    findAction = new QAction(tr("Find and replace"), this);
    key = keySettings.value("find", "default").toString();
    stdKey = QKeySequence(QKeySequence::Find);
    if (key == "default")
        key = stdKey.toString();
    findAction->setShortcut(key);
    connect(findAction, SIGNAL(triggered()), this, SLOT(find()));


    undoAction = new QAction(QIcon(":/images/undo.png"), tr("Undo"), this);
    key = keySettings.value("undo", "default").toString();
    stdKey = QKeySequence(QKeySequence::Undo);
    if (key == "default")
        key = stdKey.toString();
    undoAction->setShortcut(key);

    redoAction = new QAction(QIcon(":/images/redo.png"), tr("Redo"), this);
    key = keySettings.value("redo", "default").toString();
    stdKey = QKeySequence(QKeySequence::Redo);
    if (key == "default")
        key = stdKey.toString();
    redoAction->setShortcut(key);

    cutAction = new QAction(QIcon(":/images/cut.png"), tr("Cut"), this);
    key = keySettings.value("cut", "default").toString();
    stdKey = QKeySequence(QKeySequence::Cut);
    if (key == "default")
        key = stdKey.toString();
    cutAction->setShortcut(key);

    copyAction = new QAction(QIcon(":/images/copy.png"), tr("Copy"), this);
    key = keySettings.value("copy", "default").toString();
    stdKey = QKeySequence(QKeySequence::Copy);
    if (key == "default")
        key = stdKey.toString();
    copyAction->setShortcut(key);

    pasteAction = new QAction(QIcon(":/images/paste.png"), tr("Paste"), this);
    key = keySettings.value("paste", "default").toString();
    stdKey = QKeySequence(QKeySequence::Paste);
    if (key == "default")
        key = stdKey.toString();
    pasteAction->setShortcut(key);

    deleteAction = new QAction(tr("Delete"), this);
    key = keySettings.value("delete", "default").toString();
    stdKey = QKeySequence(QKeySequence::Delete);
    if (key == "default")
        key = stdKey.toString();
    deleteAction->setShortcut(key);

    selectAllAction = new QAction(tr("Select all"), this);
    key = keySettings.value("selectAll", "default").toString();
    stdKey = QKeySequence(QKeySequence::SelectAll);
    if (key == "default")
        key = stdKey.toString();
    selectAllAction->setShortcut(key);

    commentAction = new QAction(tr("Comment"), this);
    key = keySettings.value("comment", "default").toString();
    stdKey = QKeySequence(QString("Shift+Ctrl+A"));
    if (key == "default")
        key = stdKey.toString();
    commentAction->setShortcut(key);

    uncommentAction = new QAction(tr("Remove comment"), this);
    key = keySettings.value("uncomment", "default").toString();
    stdKey = QKeySequence(QString("Shift+Ctrl+S"));
    if (key == "default")
        key = stdKey.toString();
    uncommentAction->setShortcut(key);

    putTabAction = new QAction(tr("Indent"), this);
    key = keySettings.value("putTab", "default").toString();
    stdKey = QKeySequence(QString("Tab"));
    if (key == "default")
        key = stdKey.toString();
    putTabAction->setShortcut(key);

    deleteTabAction = new QAction(tr("Remove indent"), this);
    key = keySettings.value("deleteTab", "default").toString();
    stdKey = QKeySequence(QString("Shift+Tab"));
    if (key == "default")
        key = stdKey.toString();
    deleteTabAction->setShortcut(key);
    //! Action in edit menu connects in refreshEditMenu function!


    buildAction = new QAction(QIcon(":/images/build.png"), tr("Build this"), this);
    key = keySettings.value("build", "default").toString();
    stdKey = QKeySequence(QString("Ctrl+F9"));
    if (key == "default")
        key = stdKey.toString();
    buildAction->setShortcut(key);
    connect(buildAction, SIGNAL(triggered()), this, SLOT(buildProgram()));

    runAction = new QAction(QIcon(":/images/run.png"), tr("Build and run"), this);
    key = keySettings.value("run", "default").toString();
    stdKey = QKeySequence(QString("F9"));
    if (key == "default")
        key = stdKey.toString();
    runAction->setShortcut(key);
    connect(runAction, SIGNAL(triggered()), this, SLOT(runProgram()));

    runExeAction = new QAction(tr("Run in new window"), this);
    connect(runExeAction, SIGNAL(triggered()), this, SLOT(runExeProgram()));

    stopAction = new QAction(QIcon(":/images/stop.png"), tr("Stop"), this);
    connect(stopAction, SIGNAL(triggered()), this, SLOT(stopProgram()));
    //! Enable in runProgram(), disable in stop
    stopAction->setEnabled(false);

    debugAction = new QAction(QIcon(":/images/debug.png"), tr("Debug"), this);
    key = keySettings.value("debug", "default").toString();
    stdKey = QKeySequence(QString("F5"));
    if (key == "default")
        key = stdKey.toString();
    debugAction->setShortcut(key);
    debugKey = key;
    connect(debugAction, SIGNAL(triggered()), this, SLOT(debug()));

    debugNextNiAction = new QAction(QIcon(":/images/stepover.png"), tr("Step over"), this);
    key = keySettings.value("stepOver", "default").toString();
    stdKey = QKeySequence(QString("F10"));
    if (key == "default")
        key = stdKey.toString();
    debugNextNiAction->setShortcut(key);
    connect(debugNextNiAction, SIGNAL(triggered()), this, SLOT(debugNextNi()));

    debugNextAction = new QAction(QIcon(":/images/stepinto.png"), tr("Step into"), this);
    key = keySettings.value("stepInto", "default").toString();
    stdKey = QKeySequence(QString("F11"));
    if (key == "default")
        key = stdKey.toString();
    debugNextAction->setShortcut(key);
    connect(debugNextAction, SIGNAL(triggered()), this, SLOT(debugNext()));

    debugToggleBreakpointAction = new QAction(tr("Toggle breakpoint"), this);
    key = keySettings.value("breakpoint", "default").toString();
    stdKey = QKeySequence(QString("F8"));
    if (key == "default")
        key = stdKey.toString();
    debugToggleBreakpointAction->setShortcut(key);
    connect(debugToggleBreakpointAction, SIGNAL(triggered()), this, SLOT(debugToggleBreakpoint()));

    debugShowRegistersAction = new QAction(tr("Show registers"), this);
    key = keySettings.value("showRegisters", "default").toString();
    stdKey = QKeySequence(QString("Ctrl+R"));
    if (key == "default")
        key = stdKey.toString();
    debugShowRegistersAction->setShortcut(key);
    debugShowRegistersAction->setCheckable(true);
    connect(debugShowRegistersAction, SIGNAL(toggled(bool)), this, SLOT(debugShowRegisters()), Qt::QueuedConnection);

    debugShowMemoryAction = new QAction(tr("Show memory"), this);
    key = keySettings.value("showMemory", "default").toString();
    stdKey = QKeySequence(QString("Ctrl+M"));
    if (key == "default")
        key = stdKey.toString();
    debugShowMemoryAction->setShortcut(key);
    debugShowMemoryAction->setCheckable(true);
    connect(debugShowMemoryAction, SIGNAL(toggled(bool)), this, SLOT(debugShowMemory()), Qt::QueuedConnection);

    disableDebugActions(true);


    settingsAction = new QAction(tr("Settings"), this);
    connect(settingsAction, SIGNAL(triggered()), this, SLOT(openSettings()));

    connect(mainWidget, SIGNAL(currentChanged(int)), this, SLOT(changeActionsState(int)));
    //! Disable some actions if get started widget opened
    changeActionsState(mainWidget->currentIndex());

    helpAction = new QAction(tr("Help"), this);
    key = keySettings.value("help", "default").toString();
    stdKey = QKeySequence(QKeySequence::HelpContents);
    if (key == "default")
        key = stdKey.toString();
    helpAction->setShortcut(key);
    connect(helpAction, SIGNAL(triggered()), this, SLOT(openHelp()));

    aboutAction = new QAction(tr("About"), this);
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(openAbout()));
}

void MainWindow::createToolBars()
{
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(newAction);
    fileToolBar->addAction(openAction);
    fileToolBar->addAction(saveAction);
    fileToolBar->setObjectName("File");

    editToolBar = addToolBar(tr("Edit"));
    editToolBar->addAction(undoAction);
    editToolBar->addAction(redoAction);
    editToolBar->addAction(cutAction);
    editToolBar->addAction(copyAction);
    editToolBar->addAction(pasteAction);
    editToolBar->setObjectName("Edit");

    buildToolBar = addToolBar(tr("Build"));
    buildToolBar->addAction(buildAction);
    buildToolBar->addAction(runAction);
    buildToolBar->addAction(stopAction);
    buildToolBar->setObjectName("Build");

    debugToolBar = addToolBar(tr("Debug"));
    debugToolBar->addAction(debugAction);
    debugToolBar->addAction(debugNextNiAction);
    debugToolBar->addAction(debugNextAction);
    debugToolBar->addAction(stopAction);
    debugToolBar->setObjectName("Debug");

    restoreState(settings.value("windowstate").toByteArray());
}

void MainWindow::refreshEditMenu()
{
    CodeEditor *codeEditor = 0;

    //! Disconnect from previous
    if (prevCodeEditor) {
        disconnect(commentAction, SIGNAL(triggered()), prevCodeEditor, SLOT(commentSelectedCode()));
        disconnect(uncommentAction, SIGNAL(triggered()), prevCodeEditor, SLOT(uncommentSelectedCode()));
        disconnect(undoAction, SIGNAL(triggered()), prevCodeEditor, SLOT(undo()));
        disconnect(redoAction, SIGNAL(triggered()), prevCodeEditor, SLOT(redo()));
        disconnect(cutAction, SIGNAL(triggered()), prevCodeEditor, SLOT(cut()));
        disconnect(copyAction, SIGNAL(triggered()), prevCodeEditor, SLOT(copy()));
        disconnect(pasteAction, SIGNAL(triggered()), prevCodeEditor, SLOT(paste()));
        disconnect(deleteAction, SIGNAL(triggered()), prevCodeEditor, SLOT(deleteSelected()));
        disconnect(selectAllAction, SIGNAL(triggered()), prevCodeEditor, SLOT(selectAll()));
        disconnect(putTabAction, SIGNAL(triggered()), prevCodeEditor, SLOT(putTab()));
        disconnect(deleteTabAction, SIGNAL(triggered()), prevCodeEditor, SLOT(deleteTab()));

        disconnect(prevCodeEditor, SIGNAL(undoAvailable(bool)), undoAction, SLOT(setEnabled(bool)));
        disconnect(prevCodeEditor, SIGNAL(redoAvailable(bool)), redoAction, SLOT(setEnabled(bool)));
    }

    //! Connect to current
    if (tabs->currentIndex() != -1) {
        codeEditor = ((Tab *) tabs->currentWidget())->code;
        connect(commentAction, SIGNAL(triggered()), codeEditor, SLOT(commentSelectedCode()));
        connect(uncommentAction, SIGNAL(triggered()), codeEditor, SLOT(uncommentSelectedCode()));
        connect(undoAction, SIGNAL(triggered()), codeEditor, SLOT(undo()));
        connect(redoAction, SIGNAL(triggered()), codeEditor, SLOT(redo()));
        connect(cutAction, SIGNAL(triggered()), codeEditor, SLOT(cut()));
        connect(copyAction, SIGNAL(triggered()), codeEditor, SLOT(copy()));
        connect(pasteAction, SIGNAL(triggered()), codeEditor, SLOT(paste()));
        connect(deleteAction, SIGNAL(triggered()), codeEditor, SLOT(deleteSelected()));
        connect(selectAllAction, SIGNAL(triggered()), codeEditor, SLOT(selectAll()));
        connect(putTabAction, SIGNAL(triggered()), codeEditor, SLOT(putTab()));
        connect(deleteTabAction, SIGNAL(triggered()), codeEditor, SLOT(deleteTab()));

        connect(codeEditor, SIGNAL(undoAvailable(bool)), undoAction, SLOT(setEnabled(bool)));
        connect(codeEditor, SIGNAL(redoAvailable(bool)), redoAction, SLOT(setEnabled(bool)));
        connect(codeEditor, SIGNAL(copyAvailable(bool)), cutAction, SLOT(setEnabled(bool)));
        connect(codeEditor, SIGNAL(copyAvailable(bool)), copyAction, SLOT(setEnabled(bool)));
        connect(codeEditor, SIGNAL(copyAvailable(bool)), commentAction, SLOT(setEnabled(bool)));
        connect(codeEditor, SIGNAL(copyAvailable(bool)), uncommentAction, SLOT(setEnabled(bool)));
        connect(codeEditor, SIGNAL(copyAvailable(bool)), deleteAction, SLOT(setEnabled(bool)));
    }

    prevCodeEditor = codeEditor;
}

void MainWindow::newFile()
{
    if (tabs->count() > 0) {
        settings.setValue("tabgeometry", ((Tab *) tabs->currentWidget())->saveGeometry());
        settings.setValue("tabwindowstate", ((Tab *) tabs->currentWidget())->saveState());
    }
    //! Tabs
    mainWidget->setCurrentIndex(1);
    Tab *tab = new Tab;
    tab->code->setPlainText(startText);
    tabs->addTab(tab, tr("New"));
    tabs->setCurrentWidget(tab);
    connect(tab->code, SIGNAL(textChanged()), this, SLOT(setProgramBuildedFlagToFalse()));
    connect(tab->code, SIGNAL(modificationChanged(bool)), this, SLOT(changeCurrentSavedState(bool)));
    connect(tab->code, SIGNAL(fileOpened(QString)), this, SLOT(openFile(QString)));
}

void MainWindow::changeCurrentSavedState(bool changed)
{
    if (changed)
        tabs->setTabText(tabs->currentIndex(), '*' + tabs->tabText(tabs->currentIndex()));
    else {
        QString tabText = tabs->tabText(tabs->currentIndex());
        if (tabText[0] == '*')
            tabText.remove(0, 1);
        tabs->setTabText(tabs->currentIndex(), tabText);
    }
}

void MainWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open file"), saveOpenDirectory,
                                                    tr("Assembler source files (*.asm);;All files (*.*)"));
    if (fileName.isEmpty()) {
        return;
    }
    saveOpenDirectory = QFileInfo(fileName).absoluteDir().absolutePath();
    openFile(fileName);
}

void MainWindow::openFile(QString path)
{
    if (path.isEmpty()) {
        return;
    }

    newFile();
    Tab *curTab = (Tab *) tabs->currentWidget();
    curTab->loadCodeFromFile(path);
    setCurrentTabName(path);
    //connect(curTab, SIGNAL(fileOpened(QString)), this, SLOT(openFile(QString)));
}

void MainWindow::otherInstanceDataReceived(QByteArray data)
{
    this->showNormal();
    this->raise();
    this->activateWindow();

    QList<QByteArray> arguments = data.split(0x00);
    for (int i = 1; i < arguments.size() - 1; i++) {
        openFile(QString(arguments[i]));
    }
}

void MainWindow::setCurrentTabName(const QString &filePath, int index)
{
    //! Taking name of file (without path)
    QString caption;
    int i;
    if ((i = filePath.lastIndexOf('/')) == -1)
        i = filePath.lastIndexOf('\\');
    caption = filePath.mid(i + 1);
    if (index == -1)
        tabs->setTabText(tabs->currentIndex(), caption);
    else
        tabs->setTabText(index, caption);
}

void MainWindow::closeFile()
{
    deleteTab(tabs->currentIndex());
}

bool MainWindow::saveFile(int index)
{
    Tab *tab;
    if (index == -1)
        tab = (Tab *) tabs->currentWidget();
    else
        tab = (Tab *) tabs->widget(index);
    QString filePath = tab->getCurrentFilePath();
    if (filePath.isEmpty()) {
        return saveAsFile(index);
    } else {
        tab->saveCodeToFile(filePath, assembler);
        return true;
    }
}

bool MainWindow::saveAsFile(int index)
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save file"), saveOpenDirectory,
                                                    tr("Assembler source files (*.asm);;All files (*.*)"));
    if (fileName.isEmpty()) {
        return false;
    }
    saveOpenDirectory = QFileInfo(fileName).absoluteDir().absolutePath();
    Tab *tab;
    if (index == -1)
        tab = (Tab *) tabs->currentWidget();
    else
        tab = (Tab *) tabs->widget(index);
    tab->saveCodeToFile(fileName, assembler);
    setCurrentTabName(fileName, index);
    return true;
}

void MainWindow::saveExe()
{
    if (!programIsBuilded) {
        buildProgram();
    }
    if (!programIsBuilded) {
       return;
    }
    QString saveFileName = QFileDialog::getSaveFileName(this, tr("Save .exe file"), saveOpenDirectory,
                                                    tr("Execution files (*.exe);;All files (*.*)"));
    if (saveFileName.isEmpty())
        return;
    saveOpenDirectory = QFileInfo(saveFileName).absoluteDir().absolutePath();
    QFile exeFile(Common::pathInTemp("SASMprog.exe"));
    QFile::remove(saveFileName);
    exeFile.copy(saveFileName);
}

bool MainWindow::closeApp()
{
    int tabsCount = tabs->count();
    if (tabsCount > 0) {
        //! Consider all tabs
        tabs->setCurrentIndex(0);
        for (int i = 0; i < tabsCount; i++) {
            //! Left only saved tabs
            if (!deleteTab(tabs->currentIndex(), true))
                return false;
        }
    }
    writeSettings();
    closeFromCloseAll = true;
    close();
    return true;
}

void MainWindow::closeEvent(QCloseEvent *e)
{

    if (!closeFromCloseAll)
        if (closeApp()) {
            closeAllChildWindows();
            e->accept();
        } else
            e->ignore();
    else
        e->accept();
}

void MainWindow::closeAllChildWindows()
{
    if (findDialog) {
        findDialog->close();
        delete findDialog;
    }
    if (settingsWindow) {
        settingsWindow->close();
        delete settingsWindow;
    }
    if (help) {
        help->close();
        delete help;
    }
    if (registersWindow) {
        registersWindow->close();
        delete registersWindow;
        registersWindow = 0;
    }
    if (memoryWindow) {
        memoryWindow->close();
        delete memoryWindow;
        memoryWindow = 0;
    }
}

bool MainWindow::deleteTab(int index, bool saveFileName)
{
    settings.setValue("tabgeometry", ((Tab *) tabs->widget(index))->saveGeometry());
    settings.setValue("tabwindowstate", ((Tab *) tabs->widget(index))->saveState());
    if (okToContinue(index)) {
        if (debugger && index == tabs->currentIndex())
            debugExit();
        if (tabs->count() == 1)
            //! Get started
            mainWidget->setCurrentIndex(0);
        Tab *tabForDeleting = (Tab *) tabs->widget(index);
        if (!saveFileName || tabForDeleting->getCurrentFilePath().isEmpty()) {
            tabs->removeTab(index);
            delete tabForDeleting;
        } else {
            //! If tab did not delete
            tabs->setCurrentIndex(tabs->currentIndex() + 1);
        }
        if (tabForDeleting->code == prevCodeEditor)
            prevCodeEditor = 0;
        return true;
    }
    return false;
}

bool MainWindow::okToContinue(int index)
{
    Tab *deletingTab;
    if (index == -1)
        deletingTab = (Tab *) tabs->currentWidget();
    else
        deletingTab = (Tab *) tabs->widget(index);
    if (deletingTab->code->document()->isModified()) {
        QMessageBox msgBox(this);
        QPushButton *yesButton = msgBox.addButton(tr("Yes"), QMessageBox::YesRole);
        msgBox.addButton(tr("No"), QMessageBox::NoRole);
        QPushButton *cancelButton = msgBox.addButton(tr("Cancel"), QMessageBox::RejectRole);
        msgBox.setWindowTitle(tr("Save changes?"));
        msgBox.setText(tr("Save changes?"));
        msgBox.exec();

        if (msgBox.clickedButton() == yesButton) {
            return saveFile(index);
        } else if (msgBox.clickedButton() == cancelButton) {
            return false;
        }
    }
    return true;
}

void MainWindow::changeCurrentTab(int index)
{
    //! Set highlighter
    if (tabs->count() == 0)
        return;
    Tab *currentTab = (Tab *) tabs->widget(index);
    highlighter->setDocument(currentTab->getCodeDocument());
}

void MainWindow::buildProgram(bool debugMode)
{
    programIsBuilded = false;

    using Common::applicationDataPath;
    printLogWithTime(tr("Build started...") + '\n', Qt::black);
    QCoreApplication::processEvents();

    if (! QFile::exists(applicationDataPath())) {
        printLogWithTime(tr("Error! Program directory does not exist. Please reinstall the program.") + '\n', Qt::red);
        QMessageBox::critical(0, tr("Error!"), tr("Directory NASM does not exist. Please reinstall the program."));
        return;
    }

    QString tempPath = Common::pathInTemp(QString());
    removeDirRecuresively(tempPath);
    QString path = Common::pathInTemp("program.asm");

    Tab *currentTab = (Tab *) tabs->currentWidget();
    currentTab->saveCodeToFile(path, assembler, false, debugMode && settings.value("insertdebugstring", true).toBool());

    if (debugMode) {
        //! Save input to file
        QString inputPath = Common::pathInTemp("input.txt");
        Tab *currentTab = (Tab *) tabs->currentWidget();
        currentTab->saveInputToFile(inputPath);
    }

    while (! QFile::exists(path)) {
    }

    QString stdioMacros = Common::pathInTemp("macro.o");

    //! Assembler
    QString assemblerPath = assembler->getAssemblerPath();
    if (settings.contains("assemblerpath"))
        assemblerPath = settings.value("assemblerpath").toString();
    #ifdef Q_OS_WIN32
        QString assemblerOptions = "-g -f win32 $SOURCE$ -l $LSTOUTPUT$ -o $PROGRAM.OBJ$";
    #else
        QString assemblerOptions = "-g -f elf32 $SOURCE$ -l $LSTOUTPUT$ -o $PROGRAM.OBJ$";
    #endif
    if (settings.contains("assemblyoptions"))
        assemblerOptions = settings.value("assemblyoptions").toString();
    QStringList assemblerArguments = assemblerOptions.split(QChar(' '));
    assemblerArguments.replaceInStrings("$SOURCE$", Common::pathInTemp("program.asm"));
    assemblerArguments.replaceInStrings("$LSTOUTPUT$", Common::pathInTemp("program.lst"));
    assemblerArguments.replaceInStrings("$PROGRAM.OBJ$",
        Common::pathInTemp(settings.value("objectfilename", "program.o").toString()));
    assemblerArguments.replaceInStrings("$PROGRAM$", Common::pathInTemp("SASMprog.exe"));
    assemblerArguments.replaceInStrings("$MACRO.OBJ$", stdioMacros);
    QProcess assemblerProcess;
    QString assemblerOutput = Common::pathInTemp("compilererror.txt");
    assemblerProcess.setStandardOutputFile(assemblerOutput);
    assemblerProcess.setStandardErrorFile(assemblerOutput, QIODevice::Append);
    assemblerProcess.setWorkingDirectory(applicationDataPath() + "/include");
    assemblerProcess.start(assemblerPath, assemblerArguments);
    assemblerProcess.waitForFinished();

    if (assemblerProcess.error() != QProcess::UnknownError) {
        printLogWithTime(tr("Unable to start assembler. Check your settings.") + '\n', Qt::red);
        return;
    }

    //! GCC
    QString linkerOptions = "$PROGRAM.OBJ$ $MACRO.OBJ$ -g -o $PROGRAM$ -m32";
    if (settings.contains("linkingoptions"))
        linkerOptions = settings.value("linkingoptions").toString();
    //! macro.c compilation/copying
    QFile macro;
    #ifdef Q_OS_WIN32
        QString linker = settings.value("linkerpath", applicationDataPath() + "/MinGW/bin/gcc.exe").toString();
        if (settings.value("mode", QString("x86")).toString() == "x86") {
            macro.setFileName(applicationDataPath() + "/NASM/macro.o");
        } else {
            macro.setFileName(applicationDataPath() + "/NASM/macro64.o");
        }
        macro.copy(stdioMacros);
    #else
        QString gcc = "gcc";
        QString linker = settings.value("linkerpath", "gcc").toString();
        macro.setFileName(applicationDataPath() + "/NASM/macro.c");
        macro.copy(Common::pathInTemp("macro.c"));

        //! macro.c compilation
        QStringList gccMArguments;
        gccMArguments << "-x" << "c" << Common::pathInTemp("macro.c") << "-c" << "-g" << "-o" << stdioMacros;
        if (settings.value("mode", QString("x86")).toString() == "x86")
            gccMArguments << "-m32";
        else
            gccMArguments << "-m64";
        QProcess gccMProcess;
        gccMProcess.start(gcc, gccMArguments);
        gccMProcess.waitForFinished();
    #endif

    //! Final linking
    bool disableLinking = settings.value("disablelinking", false).toBool();
    QString linkerOutput;
    if (!disableLinking) {
        QStringList linkerArguments = linkerOptions.split(QChar(' '));
        linkerArguments.replaceInStrings("$PROGRAM.OBJ$",
            Common::pathInTemp(settings.value("objectfilename", "program.o").toString()));
        linkerArguments.replaceInStrings("$MACRO.OBJ$", stdioMacros);
        linkerArguments.replaceInStrings("$PROGRAM$", Common::pathInTemp("SASMprog.exe"));
        linkerArguments.replaceInStrings("$SOURCE$", Common::pathInTemp("program.asm"));
        linkerArguments.replaceInStrings("$LSTOUTPUT$", Common::pathInTemp("program.lst"));
        QProcess linkerProcess;
        linkerOutput = Common::pathInTemp("linkererror.txt");
        linkerProcess.setStandardOutputFile(linkerOutput);
        linkerProcess.setStandardErrorFile(linkerOutput, QIODevice::Append);
        linkerProcess.start(linker, linkerArguments);
        linkerProcess.waitForFinished();

        if (linkerProcess.error() != QProcess::UnknownError) {
            printLogWithTime(tr("Unable to start linker. Check your settings.") + '\n', Qt::red);
            return;
        }
    }

    QFile logFile;
    logFile.setFileName(assemblerOutput);
    logFile.open(QIODevice::ReadOnly);
    QTextStream log(&logFile);
    QString logText = log.readAll();
    if (settings.value("assembler", QString("NASM")).toString() == "FASM" && logText.count(QChar('\n')) == 2
            && logText.contains(QRegExp(" bytes\\.")))
        logText.clear();
    if (settings.value("assembler", QString("NASM")).toString() == "MASM") {
        if (logText.count(QChar('\n')) == 1 && logText.startsWith(" Assembling:"))
            logText.clear();
        if (logText.count(QChar('\n')) == 6 && logText.contains("ASCII build"))
            logText.clear();
    }
    logFile.close();

    bool builded;
    if (QFile::exists(Common::pathInTemp("SASMprog.exe")))
        builded = true;
    else
        builded = false;
    if (!builded) {
        printLogWithTime(tr("Warning! Errors have occurred in the build:") + '\n', Qt::red);

        //! Print errors
        printLog(logText, Qt::red);
        if (!disableLinking) {
            logFile.setFileName(linkerOutput);
            logFile.open(QIODevice::ReadOnly);
            QTextStream logLinker(&logFile);
            logText = logLinker.readAll();
            logFile.close();
            printLog(logText, Qt::red);
        }

        //! QMessageBox::critical(0, tr("Warning!"), tr("Errors have occurred in the build!"));
    } else {
        printLogWithTime(tr("Built successfully.") + '\n', Qt::darkGreen);
        //! print warnings
        printLog(logText, Qt::red);
        if (!disableLinking) {
            logFile.setFileName(linkerOutput);
            logFile.open(QIODevice::ReadOnly);
            QTextStream logLinker(&logFile);
            logText = logLinker.readAll();
            logFile.close();
            printLog(logText, Qt::red);
        }
        programIsBuilded = true;
    }
}

void MainWindow::runProgram()
{
    if (!programStopped) {
        QMessageBox::information(0, QString("SASM"), tr("The program is already running."));
        return;
    }
    outputIndex = tabs->currentIndex();
    if (!programIsBuilded) {
        buildProgram();
    }
    if (!programIsBuilded) {
       return;
    }

    stopAction->setEnabled(true);
    debugAction->setEnabled(false);
    buildAction->setEnabled(false);

    ((Tab *) tabs->currentWidget())->clearOutput();

    printLogWithTime(tr("The program is executing...") + '\n', Qt::black);
    QCoreApplication::processEvents();

    QString input = Common::pathInTemp("input.txt");
    Tab *currentTab = (Tab *) tabs->currentWidget();
    currentTab->saveInputToFile(input);

    QString program = Common::pathInTemp("SASMprog.exe");
    runProcess->setStandardInputFile(input);
    connect(runProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(getOutput()));
    programStopped = false;
    connect(runProcess, SIGNAL(started()), this, SLOT(startCountProgramTime()));
    connect(runProcess, SIGNAL(finished(int)), this, SLOT(testStopOfProgram()));
    runProcess->start(program, QStringList());

    timer->start(100);
    //Previous - connect(timer, SIGNAL(timeout()), this, SLOT(testStopOfProgram()));
}

void MainWindow::startCountProgramTime()
{
    programExecutionTime.start();
}

void MainWindow::getOutput()
{
    QString out(runProcess->readAllStandardOutput());
    printOutput(out, outputIndex);
}

void MainWindow::testStopOfProgram()
{
    QCoreApplication::processEvents();
    if (runProcess->state() == QProcess::NotRunning) {
        stopAction->setEnabled(false);
        debugAction->setEnabled(true);
        buildAction->setEnabled(true);
        if (!programStopped) {
            if (runProcess->exitStatus() == QProcess::NormalExit)
                printLogWithTime(tr("The program finished normally. Execution time: %1 s")
                                 .arg(programExecutionTime.elapsed() / 1000.0)
                                 + '\n', Qt::darkGreen);
            else
                printLogWithTime(tr("The program crashed! Execution time: %1 s").
                                 arg(programExecutionTime.elapsed() / 1000.0)
                                 + '\n', Qt::red);
            programStopped = true;
        }
        timer->stop();
    }
}

void MainWindow::runExeProgram()
{
    if (!programIsBuilded) {
        buildProgram();
    }
    if (!programIsBuilded) {
       return;
    }

    QString program = Common::pathInTemp("SASMprog.exe");
    runProcess->startDetached(program, QStringList());
}

void MainWindow::stopProgram()
{
    if (timer->isActive()) {
        programStopped = true;
        timer->stop();

        stopAction->setEnabled(false);

        runProcess->kill();

        printLogWithTime(tr("The program stopped.") + '\n', Qt::darkGreen);
    } else {
        QMessageBox::information(0, QString("SASM"), tr("The program is not running."));
    }
}

void MainWindow::printLogWithTime(const QString &message, const QColor &color)
{
    QTime curTime = QTime::currentTime();
    QString timeString = curTime.toString("[HH:mm:ss] ");
    printLog(timeString + message, color);
}

void MainWindow::printLog(const QString &message, const QColor &color)
{
    QTextCursor cursor = QTextCursor(compilerOut->document());
    cursor.movePosition(QTextCursor::End);
    compilerOut->setTextCursor(cursor);
    compilerOut->setTextColor(color);
    compilerOut->insertPlainText(message);
    cursor.movePosition(QTextCursor::End);
    compilerOut->setTextCursor(cursor);
}

void MainWindow::setProgramBuildedFlagToFalse()
{
    programIsBuilded = false;
}

void MainWindow::printOutput(QString msg, int index)
{
    Tab *tab = 0;
    if (index != -1) {
        tab = (Tab *) tabs->widget(index);
    } else
        tab = (Tab *) tabs->currentWidget();
    tab->appendOutput(msg);
}

void MainWindow::debug()
{
    //! Start debugger if true
    if (!debugger) {
        debuggerWasStarted = false;
        buildProgram(true);
        if (!programIsBuilded) {
            printLogWithTime(tr("Before debugging you need to build the program.") + '\n', Qt::red);
            return;
        }
        ((Tab *) tabs->currentWidget())->clearOutput();
        printLogWithTime(tr("Debugging started...") + '\n', Qt::darkGreen);
        QString path = Common::pathInTemp("SASMprog.exe");
        CodeEditor *code = ((Tab *) tabs->currentWidget())->code;
        debugger = new Debugger(compilerOut, path, Common::pathInTemp(QString()), assembler);
        connect(debugger, SIGNAL(highlightLine(int)), code, SLOT(updateDebugLine(int)));
        connect(debugger, SIGNAL(finished()), this, SLOT(debugExit()), Qt::QueuedConnection);
        connect(debugger, SIGNAL(started()), this, SLOT(enableDebugActions()));
        connect(debugger, SIGNAL(started()), this, SLOT(showAnyCommandWidget()));
        connect(code, SIGNAL(breakpointsChanged(quint64,bool)), debugger, SLOT(changeBreakpoint(quint64,bool)));
        connect(code, SIGNAL(addWatchSignal(const RuQPlainTextEdit::Watch &)),
                   this, SLOT(setShowMemoryToChecked(RuQPlainTextEdit::Watch)));
        connect(debugger, SIGNAL(printLog(QString,QColor)), this, SLOT(printLog(QString,QColor)));
        connect(debugger, SIGNAL(printOutput(QString)), this, SLOT(printOutput(QString)));
        connect(debugger, SIGNAL(inMacro()), this, SLOT(debugNextNi()), Qt::QueuedConnection);
        connect(debugger, SIGNAL(wasStopped()), this, SLOT(changeDebugActionToStart()));
        connect(debugger, SIGNAL(needToContinue()), this, SLOT(debug()));
        code->setDebugEnabled();
    }
    //Pause or continue debugger
    else {
        debugAction->setEnabled(false);
        if (debugger->isStopped()) {
            debugAction->setText(tr("Pause"));
            debugAction->setIcon(QIcon(":/images/debugPause.png"));
            debugger->doInput(QString("c\n"), ni);
            debugAction->setShortcut(QString());
        } else {
            debugAction->setText(tr("Continue"));
            debugAction->setIcon(QIcon(":/images/continue.png"));
            debugAction->setShortcut(debugKey);
            debugger->pause();
        }
        debugAction->setEnabled(true);
    }
}

void MainWindow::changeDebugActionToStart()
{
    debugAction->setText(tr("Continue"));
    debugAction->setIcon(QIcon(":/images/continue.png"));
    debugAction->setShortcut(debugKey);
    if (!debuggerWasStarted)
        debuggerWasStarted = true;
    else {
        debugShowRegisters();
        debugShowMemory();
    }
}

void MainWindow::enableDebugActions()
{
    debugAction->setText(tr("Continue"));
    debugAction->setIcon(QIcon(":/images/continue.png"));

    //! Set all user's breakpoints
    CodeEditor *code = ((Tab *) tabs->currentWidget())->code;
    int lineNumber;
    foreach (lineNumber, *(code->getBreakpoints())) {
        debugger->changeBreakpoint(lineNumber, true);
    }

    //! Enable all actions
    debugNextAction->setEnabled(true);
    debugNextNiAction->setEnabled(true);
    debugShowRegistersAction->setEnabled(true);
    debugShowMemoryAction->setEnabled(true);
    stopAction->setEnabled(true);

    //! Change stopAction
    disconnect(stopAction, SIGNAL(triggered()), this, SLOT(stopProgram()));
    connect(stopAction, SIGNAL(triggered()), this, SLOT(debugExit()));

    //! Block run and build
    buildAction->setEnabled(false);
    runAction->setEnabled(false);

    //! Restore windows
    debugShowRegistersAction->setChecked(settings.value("debugregisters", false).toBool());
    debugShowMemoryAction->setChecked(settings.value("debugmemory", false).toBool());
}

void MainWindow::disableDebugActions(bool start)
{
    debugAction->setText(tr("Debug"));
    debugAction->setIcon(QIcon(":/images/debug.png"));

    debugNextAction->setEnabled(false);
    debugNextNiAction->setEnabled(false);
    debugShowRegistersAction->setEnabled(false);
    debugShowMemoryAction->setEnabled(false);
    stopAction->setEnabled(false);

    //! Change stopAction
    if (!start) {
        disconnect(stopAction, SIGNAL(triggered()), this, SLOT(debugExit()));
        connect(stopAction, SIGNAL(triggered()), this, SLOT(stopProgram()));
    }

    //!Enable run and build
    buildAction->setEnabled(true);
    runAction->setEnabled(true);
}

void MainWindow::debugNext()
{
    CodeEditor *code = ((Tab *) tabs->currentWidget())->code;
    if (code->isMacroOnCurrentDebugLine()) {
        debugNextNi();
    } else {
        debugNextAction->setEnabled(false);
        debugger->doInput(QString("si\n"), si);
        debugNextAction->setEnabled(true);
    }
}

void MainWindow::debugToggleBreakpoint()
{
    CodeEditor *code = ((Tab *) tabs->currentWidget())->code;
    code->setBreakpointOnCurrentLine();
}

void MainWindow::debugNextNi()
{
    debugNextNiAction->setEnabled(false);
    debugger->doInput(QString("ni\n"), ni);
    debugNextNiAction->setEnabled(true);
}

void MainWindow::debugShowMemory()
{
    if (debugShowMemoryAction->isChecked()) {
        if (!memoryWindow) {
            memoryDock = new QDockWidget(tr("Memory"), this);
            memoryDock->setAllowedAreas(Qt::AllDockWidgetAreas);

            //! Create table
            memoryWindow = new DebugTableWidget(0, 3, memoryTable, memoryDock);
            connect(memoryWindow, SIGNAL(closeSignal()), this, SLOT(setShowMemoryToUnchecked()));
            connect(memoryWindow, SIGNAL(debugShowMemory()), this, SLOT(debugShowMemory()));
            CodeEditor *code = ((Tab *) tabs->currentWidget())->code;
            connect(code, SIGNAL(addWatchSignal(const RuQPlainTextEdit::Watch &)),
                    memoryWindow, SLOT(addVariable(const RuQPlainTextEdit::Watch &)));
            connect(debugger, SIGNAL(printMemory(QList<Debugger::memoryInfo>)),
                    memoryWindow, SLOT(setValuesFromDebugger(QList<Debugger::memoryInfo>)));

            memoryDock->setWidget(memoryWindow);
            memoryDock->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
            addDockWidget(Qt::TopDockWidgetArea, memoryDock);
            memoryDock->setObjectName("memoryDock");

            restoreState(settings.value("debugstate").toByteArray());
            if (registersDock)
                registersDock->show();
            if (memoryDock)
                memoryDock->show();

            //! Fill table
            memoryWindow->initializeMemoryWindow(watches);
        }
        if (debugger->isStopped()) {
            debugger->setWatchesCount(memoryWindow->rowCount() - 1);
            for (int i = 0; i < memoryWindow->rowCount() - 1; i++) {
                if (memoryWindow->cellWidget(i, 2)) {
                    WatchSettingsWidget *settings = (WatchSettingsWidget *) memoryWindow->cellWidget(i, 2);

                    int arraySize = 0;

                    int size = settings->sizeComboBox->currentIndex();
                    QStringList sizeFormat;
                    sizeFormat << "int" << "short" << "char" << "long long";

                    bool ok;
                    arraySize = settings->arraySizeEdit->text().toInt(&ok);
                    if (!ok && sizeFormat[size] == "long long") {
                        arraySize = 1;
                        ok = true;
                    }
                    QString watchAsArray;
                    if (ok && arraySize > 0)
                        watchAsArray = "[" + QString::number(arraySize) + "]";

                    int type = settings->typeComboBox->currentIndex();
                    QStringList printFormat;
                    printFormat << "p" << "p/x" << "p/t" << "p/c" << "p/d" << "p/u" << "p/f";
                    //! If true, watch as variable
                    if (! settings->addressCheckbox->isChecked()) {
                        debugger->doInput(printFormat[type] + " (" + sizeFormat[size] + watchAsArray + ")" +
                                          memoryWindow->item(i, 0)->text() + "\n", infoMemory);
                    } else {
                        //! Watch as random address
                        debugger->doInput(printFormat[type] + " (" + sizeFormat[size] + watchAsArray + ")" +
                                          "*((" + sizeFormat[size] + "*) " + memoryWindow->item(i, 0)->text() + ")" +
                                          "\n", infoMemory);
                    }
                }
            }
            static SignalLocker locker;
            if (memoryWindow->rowCount() > 1 && locker.tryLock()) {
                connect(debugger, SIGNAL(printMemory(QList<Debugger::memoryInfo>)), &locker, SLOT(unlock()), Qt::UniqueConnection);
                connect(debugger, SIGNAL(finished()), &locker, SLOT(unlock()), Qt::UniqueConnection);
            }
        }
    } else
        if (memoryWindow) {
            saveWatches(memoryWindow);
            memoryWindow->close();
            memoryWindow->clear();
            delete memoryWindow;
            memoryWindow = 0;
            memoryDock->close();
            delete memoryDock;
            memoryDock = 0;
        }
}

void MainWindow::saveWatches(DebugTableWidget *table)
{
    watches.clear();
    for (int i = 0; i < table->rowCount() - 1; i++) {
        RuQPlainTextEdit::Watch watch;
        watch.name = table->item(i, 0)->text();
        WatchSettingsWidget *settings = (WatchSettingsWidget *) table->cellWidget(i, 2);
        watch.type = settings->typeComboBox->currentIndex();
        watch.size = settings->sizeComboBox->currentIndex();
        watch.address = settings->addressCheckbox->isChecked();
        bool ok;
        watch.arraySize = settings->arraySizeEdit->text().toInt(&ok);
        if (!ok)
            watch.arraySize = 0;
        watches.append(watch);
    }
}

void MainWindow::setShowMemoryToUnchecked()
{
    debugShowMemoryAction->setChecked(false);
}

void MainWindow::setShowMemoryToChecked(const RuQPlainTextEdit::Watch &variable)
{
    if (!debugShowMemoryAction->isChecked()) {
        watches.append(variable);
        debugShowMemoryAction->setChecked(true);
    }
}

void MainWindow::debugShowRegisters()
{
    if (debugShowRegistersAction->isChecked()) {
        if (!registersWindow) {
            registersDock = new QDockWidget(tr("Registers"), this);
            registersDock->setAllowedAreas(Qt::AllDockWidgetAreas);

            int regCount;
            if (settings.value("mode", QString("x86")).toString() == "x86") {
                regCount = 16;
            } else {
                regCount = 24;
            }
            if (settings.value("allregisters", false).toBool()) {
                regCount += 33;
            }
            registersWindow = new DebugTableWidget(regCount, 3, registersTable, registersDock);
            connect(registersWindow, SIGNAL(closeSignal()), this, SLOT(setShowRegistersToUnchecked()));
            connect(debugger, SIGNAL(printRegisters(QList<Debugger::registersInfo>)),
                    registersWindow, SLOT(setValuesFromDebugger(QList<Debugger::registersInfo>)));

            registersDock->setWidget(registersWindow);
            registersDock->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
            addDockWidget(Qt::RightDockWidgetArea, registersDock);
            registersDock->setObjectName("registersDock");

            restoreState(settings.value("debugstate").toByteArray());
            if (registersDock)
                registersDock->show();
            if (memoryDock)
                memoryDock->show();
        }
        static SignalLocker locker;
        if (debugger->isStopped() && locker.tryLock()) {
            connect(debugger, SIGNAL(printRegisters(QList<Debugger::registersInfo>)), &locker, SLOT(unlock()), Qt::UniqueConnection);
            connect(debugger, SIGNAL(finished()), &locker, SLOT(unlock()), Qt::UniqueConnection);
            if (settings.value("allregisters", false).toBool())
                debugger->doInput(QString("info all-registers\n"), infoRegisters);
            else
                debugger->doInput(QString("info registers\n"), infoRegisters);
        }
    } else
        if (registersWindow) {
            registersWindow->close();
            registersWindow->clear();
            delete registersWindow;
            registersWindow = 0;
            registersDock->close();
            delete registersDock;
            registersDock = 0;
        }
}

void MainWindow::setShowRegistersToUnchecked()
{
    debugShowRegistersAction->setChecked(false);
}

void MainWindow::debugExit()
{
    settings.setValue("debugregisters", debugShowRegistersAction->isChecked());
    settings.setValue("debugmemory", debugShowMemoryAction->isChecked());
    settings.setValue("debugstate", saveState());
    CodeEditor *code = ((Tab *) tabs->currentWidget())->code;
    disconnect(code, SIGNAL(addWatchSignal(const RuQPlainTextEdit::Watch &)),
               this, SLOT(setShowMemoryToChecked(RuQPlainTextEdit::Watch)));
    disconnect(debugger, SIGNAL(printLog(QString,QColor)), this, SLOT(printLog(QString,QColor)));
    disconnect(debugger, SIGNAL(printOutput(QString)), this, SLOT(printOutput(QString)));
    code->setDebugDisabled();
     //! Many actions performed here - deleting of highlighting too
    delete debugger;
    debugger = 0;
    closeAnyCommandWidget();
    debugShowRegistersAction->setChecked(false);
    debugShowMemoryAction->setChecked(false);
    printLogWithTime(tr("Debugging finished.") + '\n', Qt::darkGreen);
    disableDebugActions();
}

void MainWindow::showAnyCommandWidget()
{
    debugAnyCommandWidget->show();
    debugAnyCommandWidget->setFocusOnLineEdit();
    connect(debugAnyCommandWidget, SIGNAL(performCommand(QString,bool)),
            this, SLOT(debugRunCommand(QString,bool)));
}

void MainWindow::closeAnyCommandWidget()
{
    disconnect(debugAnyCommandWidget, SIGNAL(performCommand(QString,bool)),
            this, SLOT(debugRunCommand(QString,bool)));
    debugAnyCommandWidget->close();
}

void MainWindow::debugRunCommand(QString command, bool print)
{
    static SignalLocker locker;
    if (debugger->isStopped() && locker.tryLock()) {
        connect(debugger, SIGNAL(printLog(QString,QColor)), &locker, SLOT(unlock()), Qt::UniqueConnection);
        connect(debugger, SIGNAL(finished()), &locker, SLOT(unlock()), Qt::UniqueConnection);
        printLog("> " + command + "\n", QColor(32, 71, 247));
        if (print)
            debugger->doInput("p " + command + "\n", anyAction);
        else
            debugger->doInput(command + "\n", anyAction);
    }
}

void MainWindow::find()
{
    if (!findDialog) {
        findDialog = new FindDialog(this);
        connect(findDialog, SIGNAL(findNext(QString,Qt::CaseSensitivity,bool,bool,QString)),
                this, SLOT(findNext(QString,Qt::CaseSensitivity,bool,bool,QString)));
    }
    findDialog->show();
    findDialog->activateWindow();
}

void MainWindow::findNext(const QString &pattern, Qt::CaseSensitivity cs, bool all,
                          bool replace, const QString &replaceText)
{
    //! Clear all highlights and disable highlighting of current line
    for (int i = 0; i < tabs->count(); i++) {
        CodeEditor *code = ((Tab *) tabs->widget(i))->code;
        disconnect(code, SIGNAL(cursorPositionChanged()), code, SLOT(highlightCurrentLine()));
        code->setExtraSelections(QList<QTextEdit::ExtraSelection>());
    }
     //! Restore highlight
    if (pattern.isEmpty()) {

        for (int i = 0; i < tabs->count(); i++) {
            CodeEditor *code = ((Tab *) tabs->widget(i))->code;
            connect(code, SIGNAL(cursorPositionChanged()), code, SLOT(highlightCurrentLine()));
            code->highlightCurrentLine();
            if (code->debugMode)
                code->highlightDebugLine(code->currentDebugLine);
        }
    } else {
         //If true, find all
        if (all) {
            for (int i = 0; i < tabs->count(); i++) {
                QTextEdit::ExtraSelection selection;
                QList<QTextEdit::ExtraSelection> extraSelections;
                selection.format.setBackground(QBrush(Qt::green));
                QTextDocument *document = ((Tab *) tabs->widget(i))->getCodeDocument();
                QTextCursor newCursor = QTextCursor(document);
                CodeEditor *code = ((Tab *) tabs->widget(i))->code;

                while (!newCursor.isNull() && !newCursor.atEnd()) {
                    if (cs == Qt::CaseSensitive)
                        newCursor = document->find(pattern, newCursor, QTextDocument::FindCaseSensitively);
                    else
                        newCursor = document->find(pattern, newCursor, 0);
                    //! Replace mode
                    if (replace && i == tabs->currentIndex()) {
                        newCursor.removeSelectedText();
                        newCursor.insertText(replaceText);
                    }
                    if (!newCursor.isNull()) {
                        selection.cursor = newCursor;
                        extraSelections.append(selection);
                    }
                }

                //! Highlight all
                code->setExtraSelections(extraSelections);
            }
        }
       //!Find next only
        else {
            QTextEdit::ExtraSelection selection;
            QList<QTextEdit::ExtraSelection> extraSelections;
            selection.format.setBackground(QBrush(Qt::green));
            QTextDocument *document = ((Tab *) tabs->currentWidget())->getCodeDocument();
            CodeEditor *code = ((Tab *) tabs->currentWidget())->code;
            static QTextCursor newCursor(document);
            //! if documents differ, cursor is ignored in QTextDocument::find()
            if (replace) {
                newCursor.removeSelectedText();
                newCursor.insertText(replaceText);
            }
            if (cs == Qt::CaseSensitive)
                newCursor = document->find(pattern, newCursor, QTextDocument::FindCaseSensitively);
            else
                newCursor = document->find(pattern, newCursor, 0);
             //! Continue from start
            if (newCursor.isNull()) {
                if (cs == Qt::CaseSensitive)
                    newCursor = document->find(pattern, newCursor, QTextDocument::FindCaseSensitively);
                else
                    newCursor = document->find(pattern, newCursor, 0);
            }
            if (!newCursor.isNull()) {
                selection.cursor = newCursor;
                extraSelections.append(selection);
                QTextCursor cursor = newCursor;
                cursor.clearSelection();
                code->setTextCursor(cursor);
            }

            code->setExtraSelections(extraSelections);
        }
    }
}

void MainWindow::restorePrevSession(bool notNotify)
{
    int count = settings.value("tabscount", 0).toInt();
    if (count == 0 && !notNotify) {
        QMessageBox::information(0, QString("SASM"),
                                 tr("In the previous session was not open any of the saved tabs!"));
        return;
    }
    settings.beginGroup("Tabs");
    for (int i = 0; i < count; i++) {
        QString fileName = settings.value(QString::number(i)).toString();
        newFile();
        Tab *curTab = (Tab *) tabs->currentWidget();
        curTab->loadCodeFromFile(fileName);
        setCurrentTabName(fileName);
    }
    settings.endGroup();
}

void MainWindow::writeSettings()
{
    //! GUI
    settings.beginGroup("MainWindow");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    unsigned int maxFlag = windowState() & Qt::WindowMaximized;
    settings.setValue("maximized", maxFlag);
    settings.endGroup();
    settings.setValue("windowstate", saveState());

    //! Opened tabs
    int prevTabsCount = settings.value("tabscount", 0).toInt();
    //! Remove previous
    for (int i = 0; i < prevTabsCount; i++)
    {
        settings.remove(QString("Tabs/") + QString::number(i));
    }
    //! Create current
    settings.setValue("tabscount", tabs->count());
    settings.beginGroup("Tabs");
    for (int i = 0; i < tabs->count(); i++) {
        settings.setValue(QString::number(i), ((Tab *) tabs->widget(i))->getCurrentFilePath());
    }
    settings.endGroup();

    //! Save and open directory
    settings.setValue("saveopendirectory", saveOpenDirectory);

    //! Splitters state
    settings.setValue("logsplitterstate", splitter->saveState());
}

void MainWindow::openSettings()
{
    //! Initialize
    if (!settingsWindow) {
        settingsWindow = new QWidget(this, Qt::Window);
        settingsWindow->setWindowModality(Qt::WindowModal);
        settingsUi.setupUi(settingsWindow);
        connect(settingsUi.buttonBox->button(QDialogButtonBox::Ok),
                SIGNAL(clicked()), this, SLOT(exitSettings()));
        connect(settingsUi.buttonBox->button(QDialogButtonBox::Apply),
                SIGNAL(clicked()), this, SLOT(saveSettings()));
        connect(settingsUi.buttonBox->button(QDialogButtonBox::Cancel),
                SIGNAL(clicked()), this, SLOT(restoreSettingsAndExit()));
        connect(settingsUi.resetAllButton, SIGNAL(clicked()), this, SLOT(resetAllSettings()));
        connect(settingsUi.disableLinkingCheckbox, SIGNAL(stateChanged(int)),
                this, SLOT(enableOrDisableLinkingEdit(int)));

        //! Colors
        colorSignalMapper = new QSignalMapper(this);
        colorButtons <<
                        //! Foreground
                        settingsUi.keywordsColorButton << settingsUi.registersColorButton <<
                        settingsUi.numbersColorButton << settingsUi.memoryColorButton <<
                        settingsUi.labelsColorButton << settingsUi.commentsColorButton <<
                        settingsUi.systemColorButton << settingsUi.iomacroColorButton <<
                        settingsUi.quotationColorButton <<
                        //! Background
                        settingsUi.keywordsColorButton_2 << settingsUi.registersColorButton_2 <<
                        settingsUi.numbersColorButton_2 << settingsUi.memoryColorButton_2 <<
                        settingsUi.labelsColorButton_2 << settingsUi.commentsColorButton_2 <<
                        settingsUi.systemColorButton_2 << settingsUi.iomacroColorButton_2 <<
                        settingsUi.quotationColorButton_2 <<
                        //! Common
                        settingsUi.backgroundColorButton << settingsUi.lineNumberPanelColorButton <<
                        settingsUi.fontColorButton <<
                        settingsUi.currentLineColorButton << settingsUi.debugLineColorButton <<
                        settingsUi.lineNumberFontColorButton;

        defaultColors <<
                          //! According to colorButtons
                          QColor(Qt::blue) << QColor(153, 0, 204) <<
                          QColor(255, 122, 0) << QColor(0, 128, 255) <<
                          QColor(128, 0, 0) << QColor(Qt::darkGreen) <<
                          QColor(Qt::darkCyan) << QColor(Qt::blue) <<
                          QColor(128, 128, 128) <<
                          QPalette().color(QPalette::Base) << QPalette().color(QPalette::Base) <<
                          QPalette().color(QPalette::Base) << QPalette().color(QPalette::Base) <<
                          QPalette().color(QPalette::Base) << QPalette().color(QPalette::Base) <<
                          QPalette().color(QPalette::Base) << QPalette().color(QPalette::Base) <<
                          QPalette().color(QPalette::Base) <<
                          QPalette().color(QPalette::Base) << QPalette().color(QPalette::Window) <<
                          QPalette().color(QPalette::WindowText) <<
                          QColor(232, 232, 255) << QColor(235, 200, 40) <<
                          QColor(QPalette::WindowText);
        //! Add color to associative array
        for (int i = 0; i < colorButtons.size(); i++) {

            QString name = colorButtons[i]->objectName();
            name.remove("Button");
            name.replace("_2", "bg");
            name = name.toLower();
            colorsMap.insert(name, defaultColors[i]);

            connect(colorButtons[i], SIGNAL(clicked()), colorSignalMapper, SLOT(map()));
            colorSignalMapper->setMapping(colorButtons[i], colorButtons[i]);
        }
        connect(colorSignalMapper, SIGNAL(mapped(QWidget*)), this, SLOT(pickColor(QWidget*)));

        //! Fonts
        fontsSignalMapper = new QSignalMapper(this);
        fontCheckBoxes <<
                          //! Bold
                          settingsUi.keywordsBoldCheckBox << settingsUi.registersBoldCheckBox <<
                          settingsUi.numbersBoldCheckBox << settingsUi.memoryBoldCheckBox <<
                          settingsUi.labelsBoldCheckBox << settingsUi.commentsBoldCheckBox <<
                          settingsUi.systemBoldCheckBox << settingsUi.iomacroBoldCheckBox <<
                          settingsUi.quotationBoldCheckBox <<
                          //! Italic
                          settingsUi.keywordsItalicCheckBox << settingsUi.registersItalicCheckBox <<
                          settingsUi.numbersItalicCheckBox << settingsUi.memoryItalicCheckBox <<
                          settingsUi.labelsItalicCheckBox << settingsUi.commentsItalicCheckBox <<
                          settingsUi.systemItalicCheckBox << settingsUi.iomacroItalicCheckBox <<
                          settingsUi.quotationItalicCheckBox;
         //! Add font to associative array
        for (int i = 0; i < fontCheckBoxes.size(); i++) {

            QString name = fontCheckBoxes[i]->objectName();
            name.remove("CheckBox");
            name = name.toLower();

            connect(fontCheckBoxes[i], SIGNAL(clicked()), fontsSignalMapper, SLOT(map()));
            fontsSignalMapper->setMapping(fontCheckBoxes[i], fontCheckBoxes[i]);
        }
        connect(fontsSignalMapper, SIGNAL(mapped(QWidget*)), this, SLOT(changeHighlightingFont(QWidget*)));

        connect(settingsUi.currentLineCheckBox, SIGNAL(clicked(bool)), this, SLOT(changeHighlightingLineMode(bool)));

        backupSettings();
        initAssemblerSettings(true);
    } else {
        initAssemblerSettings(false);
    }

    //! Set settings
    //! Common
    settingsUi.startWindow->setCurrentIndex(settings.value("startwindow", 0).toInt());
    settingsUi.language->setCurrentIndex(settings.value("language", 0).toInt());
    settingsUi.fontComboBox->setCurrentFont(QFont(settings.value("fontfamily",
                                                                 QString("Courier New")).value<QString>()));
    settingsUi.fontSizeSpinBox->setValue(settings.value("fontsize", 12).toInt());
    if (settings.value("allregisters", false).toBool())
        settingsUi.registersYesRadioButton->setChecked(true);
    else
        settingsUi.registersNoRadioButton->setChecked(true);
    settingsUi.insertDebugStringCheckBox->setChecked(settings.value("insertdebugstring", true).toBool());

    //! Colors
    for (int i = 0; i < colorButtons.size(); i++) {
        pickColor((QWidget *) colorButtons[i], true);
    }
    //! Fonts
    for (int i = 0; i < fontCheckBoxes.size(); i++) {
        changeHighlightingFont((QWidget *) fontCheckBoxes[i], true);
    }

    settingsUi.currentLineCheckBox->setChecked(settings.value("currentlinemode", true).toBool());
    settingsWindow->show();
}

void MainWindow::initAssemblerSettings(bool firstOpening)
{
    if (firstOpening) {
        /*!
         *
         *  Assembler Dependent Options Begin
         *
         */
        connect(settingsUi.nasmRadioButton, SIGNAL(clicked()), this, SLOT(changeAssembler()));
        connect(settingsUi.gasRadioButton, SIGNAL(clicked()), this, SLOT(changeAssembler()));
        connect(settingsUi.fasmRadioButton, SIGNAL(clicked()), this, SLOT(changeAssembler()));
        #ifdef Q_OS_WIN32
            connect(settingsUi.masmRadioButton, SIGNAL(clicked()), this, SLOT(changeAssembler()));
            connect(settingsUi.masmRadioButton, SIGNAL(clicked()), this, SLOT(printMasmInfo()));
        //! Else it is Linux
        #else
            settingsUi.masmRadioButton->setEnabled(false);
        #endif

            /*!
             *
             *  Assembler Dependent Options End
             *
             */

        connect(settingsUi.x86RadioButton, SIGNAL(toggled(bool)), this, SLOT(changeMode(bool)));

        //! Start text editor
        settingsStartTextEditor = new CodeEditor(0, false);
        settingsHighlighter = new Highlighter(assembler);
        settingsHighlighter->setDocument(settingsStartTextEditor->document());
        QVBoxLayout *startTextLayout = new QVBoxLayout;
        startTextLayout->addWidget(settingsStartTextEditor);
        settingsUi.startTextWidget->setLayout(startTextLayout);
    }

    settingsStartTextEditor->setPlainText(settings.value("starttext").toString());

    //! Build options
    QString options = assembler->getAssemblerOptions();
    settingsUi.assemblyOptionsEdit->setText(settings.value("assemblyoptions", options).toString());

    QString linkerOptions = assembler->getLinkerOptions();
    settingsUi.linkingOptionsEdit->setText(settings.value("linkingoptions", linkerOptions).toString());

    settingsUi.objectFileNameEdit->setText(settings.value("objectfilename", "program.o").toString());

    settingsUi.disableLinkingCheckbox->setChecked(settings.value("disablelinking", false).toBool());

    QString assemblerPath = assembler->getAssemblerPath();
    settingsUi.assemblerPathEdit->setText(settings.value("assemblerpath", assemblerPath).toString());

    #ifdef Q_OS_WIN32
        settingsUi.linkerPathEdit->setText(settings.value("linkerpath", Common::applicationDataPath() + "/MinGW/bin/gcc.exe").toString());
    #else
        settingsUi.linkerPathEdit->setText(settings.value("linkerpath", "gcc").toString());
    #endif

    disconnect(settingsUi.x86RadioButton, SIGNAL(toggled(bool)), this, SLOT(changeMode(bool)));
    //! Mode
    if (assembler->isx86())
        settingsUi.x86RadioButton->setChecked(true);
    else
        settingsUi.x64RadioButton->setChecked(true);
    connect(settingsUi.x86RadioButton, SIGNAL(toggled(bool)), this, SLOT(changeMode(bool)));

    /*!
     *
     *  Assembler Dependent Options Begin
     *
     */

    QString assemblerName = settings.value("assembler", QString("NASM")).toString();
    if (assemblerName == "MASM") {
        settingsUi.x64RadioButton->setEnabled(false);
        printMasmInfo();
    }
    if (assemblerName == "NASM") {
        settingsUi.nasmRadioButton->setChecked(true);
    } else if (assemblerName == "GAS") {
        settingsUi.gasRadioButton->setChecked(true);
    } else if (assemblerName == "FASM") {
        settingsUi.fasmRadioButton->setChecked(true);
    } else if (assemblerName == "MASM") {
        settingsUi.masmRadioButton->setChecked(true);
    }
}

/*!
 *
 *  Assembler Dependent Options End
 *
 */

void MainWindow::printMasmInfo()
{
    settingsUi.infoLabel->setText(tr("Please set path to MASM assembler (ml.exe) and linker (link.exe) on your computer ") +
                                  tr("in fields \"Assembler path\" and \"Linker path\" above."));
}
/*!
 *
 *  Assembler Dependent Options Begin
 *
 */
void MainWindow::changeAssembler()
{
    if (settingsUi.nasmRadioButton->isChecked())
        settings.setValue("assembler", QString("NASM"));
    else if (settingsUi.gasRadioButton->isChecked()) {
        settings.setValue("assembler", QString("GAS"));
    } else if (settingsUi.fasmRadioButton->isChecked()) {
        settings.setValue("assembler", QString("FASM"));
    } else if (settingsUi.masmRadioButton->isChecked()) {
        settings.setValue("assembler", QString("MASM"));
    }
    recreateAssembler();
}

/*!
 *
 *  Assembler Dependent Options End
 *
 */

void MainWindow::changeMode(bool x86)
{
    if (x86)
        settings.setValue("mode", QString("x86"));
    else
        settings.setValue("mode", QString("x64"));
    recreateAssembler();
}

void MainWindow::recreateAssembler(bool start)
{
    if (assembler) {
        delete assembler;
        assembler = 0;
    }
    bool x86 = true;
    if (settings.value("mode", QString("x86")).toString() != "x86")
        x86 = false;
    /*!
     *
     *  Assembler Dependent Options Begin
     *
     */

    QString assemblerName = settings.value("assembler", QString("NASM")).toString();
    if (!start) {
        if (assemblerName == "MASM") {
            settingsUi.x64RadioButton->setEnabled(false);
            if (settingsUi.x64RadioButton->isChecked()) {
                settingsUi.x86RadioButton->setChecked(true);
                return;
            }
        } else {
            settingsUi.x64RadioButton->setEnabled(true);
            settingsUi.infoLabel->setText("");
        }
    }
    if (assemblerName == "NASM") {
        assembler = new NASM(x86);
    } else if (assemblerName == "GAS") {
        assembler = new GAS(x86);
    } else if (assemblerName == "FASM") {
        assembler = new FASM(x86);
    } else if (assemblerName == "MASM") {
        assembler = new MASM(x86);
    }

    /*!
     *
     *  Assembler Dependent Options End
     *
     */


    if (!start) {
        settingsUi.assemblyOptionsEdit->setText(assembler->getAssemblerOptions());
        settingsUi.linkingOptionsEdit->setText(assembler->getLinkerOptions());
        settingsUi.objectFileNameEdit->setText("program.o");
        settingsUi.disableLinkingCheckbox->setChecked(false);
        settingsUi.assemblerPathEdit->setText(assembler->getAssemblerPath());
        settingsUi.linkerPathEdit->setText(assembler->getLinkerPath());
        settings.setValue("assemblyoptions", assembler->getAssemblerOptions());
        settings.setValue("linkingoptions", assembler->getLinkerOptions());
        settings.setValue("objectfilename", "program.o");
        settings.setValue("disablelinking", false);
        settings.setValue("assemblerpath", assembler->getAssemblerPath());
        settings.setValue("linkerpath", assembler->getLinkerPath());
        changeStartText();
        recreateHighlighter();
    }
}

void MainWindow::changeStartText()
{
    settingsStartTextEditor->setPlainText(assembler->getStartText());
    settings.setValue("starttext", assembler->getStartText());
}

void MainWindow::backupSettings()
{
    backupAssembler = settings.value("assembler", QString("NASM")).toString();
    backupMode = settings.value("mode", QString("x86")).toString();
    backupAssemblerOptions = settings.value("assemblyoptions", assembler->getAssemblerOptions()).toString();
    backupAssemblerPath = settings.value("assemblerpath", assembler->getAssemblerPath()).toString();
    backupLinkerOptions = settings.value("linkingoptions", assembler->getLinkerOptions()).toString();
    backupObjectFileName = settings.value("objectfilename", "program.o").toString();
    backupDisableLinking = settings.value("disablelinking", false).toBool();
    backupStartText = settings.value("starttext", assembler->getStartText()).toString();
    backupLinkerPath = settings.value("linkerpath", assembler->getLinkerPath()).toString();
}

void MainWindow::restoreSettingsAndExit()
{
    settings.setValue("assembler", backupAssembler);
    settings.setValue("mode", backupMode);
    recreateAssembler();
    settings.setValue("assemblyoptions", backupAssemblerOptions);
    settings.setValue("assemblerpath", backupAssemblerPath);
    settings.setValue("linkingoptions", backupLinkerOptions);
    settings.setValue("objectfilename", backupObjectFileName);
    settings.setValue("disablelinking", backupDisableLinking);
    settings.setValue("starttext", backupStartText);
    settings.setValue("linkerpath", backupLinkerPath);
    settingsWindow->close();
}

void MainWindow::recreateHighlighter()
{
    if (highlighter) {
        delete highlighter;
        highlighter = new Highlighter(assembler);
    }
    if (tabs && tabs->count() > 0) {
        Tab *currentTab = (Tab *) tabs->currentWidget();
        highlighter->setDocument(currentTab->getCodeDocument());
    }
    delete settingsHighlighter;
    settingsHighlighter = new Highlighter(assembler);
    settingsHighlighter->setDocument(settingsStartTextEditor->document());
}

void MainWindow::saveSettings()
{
    settings.setValue("startwindow", settingsUi.startWindow->currentIndex());
    settings.setValue("language", settingsUi.language->currentIndex());
    startText = settingsStartTextEditor->toPlainText();
    settings.setValue("allregisters", settingsUi.registersYesRadioButton->isChecked());
    settings.setValue("insertdebugstring", settingsUi.insertDebugStringCheckBox->isChecked());

    //! Change fonts
    settings.setValue("fontfamily", settingsUi.fontComboBox->currentFont().family());
    settings.setValue("fontsize", settingsUi.fontSizeSpinBox->value());
    for (int i = 0; i < tabs->count(); i++) {
        Tab *tab = (Tab *) tabs->widget(i);
        tab->setFonts();
    }
    QFont compilerOutFont;
    compilerOutFont.setPointSize(settings.value("fontsize", 12).toInt());
    compilerOut->setFont(compilerOutFont);

    settings.setValue("assemblyoptions", settingsUi.assemblyOptionsEdit->text());
    settings.setValue("linkingoptions", settingsUi.linkingOptionsEdit->text());
    settings.setValue("objectfilename", settingsUi.objectFileNameEdit->text());
    settings.setValue("disablelinking", settingsUi.disableLinkingCheckbox->isChecked());
    settings.setValue("assemblerpath", settingsUi.assemblerPathEdit->text());
    settings.setValue("linkerpath", settingsUi.linkerPathEdit->text());
    settings.setValue("starttext", settingsStartTextEditor->document()->toPlainText());

    backupSettings();
}

void MainWindow::pickColor(QWidget *button, bool init)
{
    QPushButton *colorButton = (QPushButton *) button;
    QString name = colorButton->objectName();
    name.remove("Button");
    name.replace("_2", "bg");
    name = name.toLower();
    if (!init) {
        QColor color = QColorDialog::getColor(settings.value(name, colorsMap[name]).value<QColor>());
        if (color.isValid()) {
            settings.setValue(name, color);
            if (name == "backgroundcolor") {
                int r, g, b;
                color.getRgb(&r, &g, &b);
                for (int i = 9; i < 18; i++) {
                    QString buttonName = colorButtons[i]->objectName();
                    buttonName.remove("Button");
                    buttonName.replace("_2", "bg");
                    buttonName = buttonName.toLower();
                    settings.setValue(buttonName, color);
                    colorButtons[i]->setStyleSheet(QString("background-color: rgb(%1, %2, %3)").arg(r).arg(g).arg(b));
                }
            }
            recreateHighlighter();
        }
    }
    int r, g, b;
    QColor color = settings.value(name, colorsMap[name]).value<QColor>();
    color.getRgb(&r, &g, &b);
    colorButton->setStyleSheet(QString("background-color: rgb(%1, %2, %3)").arg(r).arg(g).arg(b));
}

void MainWindow::changeHighlightingFont(QWidget *box, bool init)
{
    QCheckBox *checkBox = (QCheckBox *) box;
    QString name = checkBox->objectName();
    name.remove("CheckBox");
    name = name.toLower();
    if (init) {
        bool defaultState = (name == "keywordsbold") ? true : false;
        checkBox->setChecked(settings.value(name, defaultState).toBool());
    } else {
        settings.setValue(name, checkBox->isChecked());
        recreateHighlighter();
    }
}

void MainWindow::changeHighlightingLineMode(bool mode)
{
    settings.setValue("currentlinemode", mode);
}

void MainWindow::exitSettings()
{
    saveSettings();
    settingsWindow->close();
}

void MainWindow::resetAllSettings()
{
    //! Ask before execution
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.addButton(tr("Yes"), QMessageBox::YesRole);
    QPushButton *cancelButton = msgBox.addButton(tr("Cancel"), QMessageBox::RejectRole);
    msgBox.setWindowTitle(tr("Warning!"));
    msgBox.setText(tr("All settings will be deleted and all unsaved data will be lost. Are you sure?"));
    msgBox.exec();
    if (msgBox.clickedButton() == cancelButton) {
        return;
    }

    settings.remove("MainWindow/size");
    settings.remove("MainWindow/pos");

    int prevTabsCount = settings.value("tabscount", 0).toInt();
    //! Remove previous
    for (int i = 0; i < prevTabsCount; i++)
        settings.remove(QString("Tabs/") + QString::number(i));

    settings.clear();

    closeFromCloseAll = true;
    close();
}

void MainWindow::changeActionsState(int widgetIndex)
{
    //! Get started window
    if (widgetIndex == 0) {
        closeAction->setEnabled(false);
        saveAction->setEnabled(false);
        saveAsAction->setEnabled(false);
        saveExeAction->setEnabled(false);
        findAction->setEnabled(false);
        commentAction->setEnabled(false);
        uncommentAction->setEnabled(false);
        undoAction->setEnabled(false);
        redoAction->setEnabled(false);
        cutAction->setEnabled(false);
        copyAction->setEnabled(false);
        pasteAction->setEnabled(false);
        deleteAction->setEnabled(false);
        selectAllAction->setEnabled(false);
        putTabAction->setEnabled(false);
        deleteTabAction->setEnabled(false);
        buildAction->setEnabled(false);
        runAction->setEnabled(false);
        runExeAction->setEnabled(false);
        stopAction->setEnabled(false);
        debugAction->setEnabled(false);
        debugNextAction->setEnabled(false);
        debugNextNiAction->setEnabled(false);
        debugShowRegistersAction->setEnabled(false);
        debugToggleBreakpointAction->setEnabled(false);
        debugShowMemoryAction->setEnabled(false);
    } else {
        closeAction->setEnabled(true);
        saveAction->setEnabled(true);
        saveAsAction->setEnabled(true);
        saveExeAction->setEnabled(true);
        findAction->setEnabled(true);
        pasteAction->setEnabled(true);
        selectAllAction->setEnabled(true);
        putTabAction->setEnabled(true);
        deleteTabAction->setEnabled(true);
        buildAction->setEnabled(true);
        runAction->setEnabled(true);
        runExeAction->setEnabled(true);
        debugAction->setEnabled(true);
        debugToggleBreakpointAction->setEnabled(true);
    }
}

void MainWindow::openHelp()
{
    help = new QTextBrowser;
    help->setAttribute(Qt::WA_DeleteOnClose);
    QFile helpFile;
    //! language 0=Russian
    if (settings.value("language", 0).toInt() == 0) {
        helpFile.setFileName(":help/help.html");
    } else {
        //! English language
        helpFile.setFileName(":help/helpENG.html");
    }

    helpFile.open(QFile::ReadOnly);
    QTextStream helpText(&helpFile);
    helpText.setCodec("utf-8");
    help->setHtml(helpText.readAll());
    helpFile.close();
    help->setWindowState(Qt::WindowMaximized);
    help->setOpenExternalLinks(true);
    help->setWindowTitle(tr("Help"));
    help->setWindowIcon(QIcon(":images/mainIcon.png"));
    help->show();
}

void MainWindow::openAbout()
{
    QMessageBox::about(this, tr("About SASM"),
                       tr("SASM (SimpleASM) ") + SASM_VERSION + " - " +
                       tr("simple Open Source IDE for NASM, MASM, GAS and FASM assembler languages.") + '\n' +
                       tr("Licensed under the GNU GPL v3.0") + '\n' +
                       tr("Based on the Qt.") + '\n' +
                       tr("Copyright © 2013 Dmitriy Manushin") + '\n' +
                       tr("Development and idea - Dmitriy Manushin") + '\n' +
                       tr("Icon and advices - Alick Gaybullaev") + '\n' + '\n' +
                       tr("Wishes and error messages are sent to the e-mail: Dman1095@gmail.com") + '\n' +
                       tr("More information on the site: http://dman95.github.io/SASM/") + '\n' + '\n' +
                       tr("Donate:") + '\n' +
                       tr("PayPal - Dman1095@gmail.com") + '\n' +
                       tr("WMZ - Z282016332582") + '\n' +
                       tr("WMR - R331674303467")
                       );
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    foreach (const QUrl &url, event->mimeData()->urls())
        openFile(url.toLocalFile());
}

bool MainWindow::removeDirRecuresively(const QString &dirName){
    bool result = true;
    QDir dir(dirName);

    if (dir.exists(dirName)) {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System |
                                                    QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
            if (info.isDir()) {
                result = removeDirRecuresively(info.absoluteFilePath());
            }
            else {
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result) {
                return result;
            }
        }
        result = dir.rmdir(dirName);
    }
    return result;
}

MainWindow::~MainWindow()
{
    //! Delete all temporary files
    removeDirRecuresively(Common::pathInTemp(QString()));
}
