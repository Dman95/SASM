/****************************************************************************
** SASM - simple IDE for NASM development
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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->setWindowTitle("SASM");
    setWindowIcon(QIcon(":images/mainIcon.png"));

    //restore settings
    QSettings settings("SASM Project", "SASM");

    //set tab splitter state
    tabSplitterState = settings.value("tabsplitterstate").toByteArray();

    //set code field start text
    startText = settings.value("starttext", QString()).toString();
    if (startText.isEmpty()) {
        settings.setValue("starttext",
                       QString("%include \"io.inc\"\n\nsection .text\nglobal CMAIN\nCMAIN:\n    ;write your code here\n    xor eax, eax\n    ret"));
        startText = settings.value("starttext", QString()).toString();
    }

    //set save and open directory
    saveOpenDirectory = settings.value("saveopendirectory", QString(applicationDataPath() + "/Projects")).toString();

    //initial variables
    programIsBuilded = false;
    memoryDebugWindow = 0;
    memoryExamineWindow = 0;
    anyCommandDebugWindow = 0;
    prevCodeEditor = 0;
    findDialog = 0;
    settingsWindow = 0;
    closeFromCloseAll = false;
    settingsStartTextEditor = 0;
    help = 0;
    registersWindow = 0;

    timer = new QTimer;
    timer->setInterval(100);
    timer->stop();
    connect(timer, SIGNAL(timeout()), this, SLOT(testStopOfProgram()));
    runProcess = new QProcess();

    initUi();
    createActions();
    createMenus();
    refreshEditMenu();

    //restore log splitter state
    splitter->restoreState(settings.value("logsplitterstate").toByteArray());
}

void MainWindow::initUi()
{
    //Resize
    QSettings settings("SASM Project", "SASM");
    settings.beginGroup("MainWindow");
    resize(settings.value("size", QSize(1024, 650)).toSize()); //default 1024x650
    move(settings.value("pos", QPoint(200, 200)).toPoint());
    setWindowState(windowState() | (Qt::WindowState) settings.value("maximized", (int) Qt::WindowNoState).toInt()); //set maximized
    settings.endGroup();

    //Get Started window
    getStartedWidget = new GetStartedWidget;
    connect(getStartedWidget->newButton, SIGNAL(clicked()), this, SLOT(newFile()));
    connect(getStartedWidget->openButton, SIGNAL(clicked()), this, SLOT(openFile()));
    connect(getStartedWidget->prevSessionButton, SIGNAL(clicked()), this, SLOT(restorePrevSession()));

    //Create form
    splitter = new QSplitter;
    splitter->setOrientation(Qt::Vertical);

    //Stacked widget
    mainWidget = new QStackedWidget;
    mainWidget->addWidget(getStartedWidget);
    mainWidget->addWidget(splitter);
    mainWidget->setCurrentIndex(0); //get started
    setCentralWidget(mainWidget);

    //Create highlighter
    highlighter = new Highlighter();

    //Create tabs
    tabs = new QTabWidget;
    connect(tabs, SIGNAL(tabCloseRequested(int)), this, SLOT(deleteTab(int)));
    connect(tabs, SIGNAL(currentChanged(int)), this, SLOT(setHighlighter(int)));
    tabs->setTabShape(QTabWidget::Triangular);
    tabs->setTabsClosable(true);

    //Create compiler field
    compilerOut = new RuQTextEdit;
    compilerOut->setReadOnly(true);
    QFont compilerOutFont;
    compilerOutFont.setPointSize(11);
    compilerOut->setFont(compilerOutFont);
    compilerOut->setText(tr("Build log:") + '\n');

    //Add widgets on splitter
    splitter->addWidget(tabs);
    splitter->addWidget(compilerOut);
    int compilerOutSize = 100;
    splitter->setSizes( QList<int>() << splitter->size().height() - compilerOutSize << compilerOutSize );

    //Restore previous session if it in settings
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
    debugMenu->addAction(debugAnyAction);
    debugMenu->addSeparator();
    debugMenu->addAction(debugNextAction);
    debugMenu->addAction(debugNextNiAction);
    debugMenu->addSeparator();
    debugMenu->addAction(debugShowRegistersAction);
    debugMenu->addAction(debugMemoryDialogAction);
    debugMenu->addAction(debugExamineMemoryAction);
    debugMenu->addSeparator();
    debugMenu->addAction(debugExitAction);
    settingsMenu = menuBar()->addMenu(tr("Settings"));
    settingsMenu->addAction(settingsAction);
    helpMenu = menuBar()->addMenu(tr("Help"));
    helpMenu->addAction(helpAction);
    helpMenu->addAction(aboutAction);
}

void MainWindow::createActions()
{
    newAction = new QAction(tr("New"), this);
    newAction->setShortcut(QKeySequence::New);
    connect(newAction, SIGNAL(triggered()), this, SLOT(newFile()));

    openAction = new QAction(tr("Open"), this);
    openAction->setShortcut(QKeySequence::Open);
    connect(openAction, SIGNAL(triggered()), this, SLOT(openFile()));

    closeAction = new QAction(tr("Close file"), this);
    closeAction->setShortcut(QString("Ctrl+W"));
    connect(closeAction, SIGNAL(triggered()), this, SLOT(closeFile()));

    
    saveAction = new QAction(tr("Save"), this);
    saveAction->setShortcut(QKeySequence::Save);
    connect(saveAction, SIGNAL(triggered()), this, SLOT(saveFile()));

    saveAsAction = new QAction(tr("Save as"), this);
    saveAsAction->setShortcut(QKeySequence::SaveAs);
    connect(saveAsAction, SIGNAL(triggered()), this, SLOT(saveAsFile()));

    saveExeAction = new QAction(tr("Save .exe"), this);
    connect(saveExeAction, SIGNAL(triggered()), this, SLOT(saveExe()));

    exitAction = new QAction(tr("Exit"), this);
    exitAction->setShortcut(QString("Ctrl+Q"));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(closeApp()));

    findAction = new QAction(tr("Find and replace"), this);
    findAction->setShortcut(QString("Ctrl+F"));
    connect(findAction, SIGNAL(triggered()), this, SLOT(find()));


    undoAction = new QAction(tr("Undo"), this);
    undoAction->setShortcut(QKeySequence::Undo);

    redoAction = new QAction(tr("Redo"), this);
    redoAction->setShortcut(QKeySequence::Redo);

    cutAction = new QAction(tr("Cut"), this);
    cutAction->setShortcut(QKeySequence::Cut);

    copyAction = new QAction(tr("Copy"), this);
    copyAction->setShortcut(QKeySequence::Copy);

    pasteAction = new QAction(tr("Paste"), this);
    pasteAction->setShortcut(QKeySequence::Paste);

    deleteAction = new QAction(tr("Delete"), this);
    deleteAction->setShortcut(QKeySequence::Delete);

    selectAllAction = new QAction(tr("Select all"), this);
    selectAllAction->setShortcut(QKeySequence::SelectAll);

    commentAction = new QAction(tr("Comment"), this);
    commentAction->setShortcut(QString("Ctrl+Shift+Q"));

    uncommentAction = new QAction(tr("Remove comment"), this);
    uncommentAction->setShortcut(QString("Ctrl+Shift+A"));

    putTabAction = new QAction(tr("Indent"), this);
    putTabAction->setShortcut(QString("Tab"));

    deleteTabAction = new QAction(tr("Remove indent"), this);
    deleteTabAction->setShortcut(QString("Ctrl+Tab"));
    //Action in edit menu connects in refreshEditMenu function!


    buildAction = new QAction(tr("Build"), this);
    buildAction->setShortcut(QString("Ctrl+F9"));
    connect(buildAction, SIGNAL(triggered()), this, SLOT(buildProgram()));

    runAction = new QAction(tr("Run"), this);
    runAction->setShortcut(QString("F9"));
    connect(runAction, SIGNAL(triggered()), this, SLOT(runProgram()));

    runExeAction = new QAction(tr("Run in new window"), this);
    connect(runExeAction, SIGNAL(triggered()), this, SLOT(runExeProgram()));

    stopAction = new QAction(tr("Stop"), this);
    connect(stopAction, SIGNAL(triggered()), this, SLOT(stopProgram()));
    stopAction->setEnabled(false); //enable in runProgram(), disable in stop

    debugAction = new QAction(tr("Debug"), this);
    debugAction->setShortcut(QString("Ctrl+D"));
    connect(debugAction, SIGNAL(triggered()), this, SLOT(debug()));

    debugNextAction = new QAction(tr("Step into"), this);
    debugNextAction->setShortcut(QString("Shift+F7"));
    connect(debugNextAction, SIGNAL(triggered()), this, SLOT(debugNext()));

    debugNextNiAction = new QAction(tr("Step over"), this);
    debugNextNiAction->setShortcut(QString("F7"));
    connect(debugNextNiAction, SIGNAL(triggered()), this, SLOT(debugNextNi()));

    debugShowRegistersAction = new QAction(tr("Show registers"), this);
    debugShowRegistersAction->setShortcut(QString("Ctrl+R"));
    debugShowRegistersAction->setCheckable(true);
    connect(debugShowRegistersAction, SIGNAL(changed()), this, SLOT(debugShowRegisters()));

    debugMemoryDialogAction = new QAction(tr("Show memory"), this);
    debugMemoryDialogAction->setShortcut(QString("Ctrl+M"));
    connect(debugMemoryDialogAction, SIGNAL(triggered()), this, SLOT(debugOpenMemoryDialog()));

    debugExamineMemoryAction = new QAction(tr("Examine memory at address"), this);
    debugExamineMemoryAction->setShortcut(QString("Ctrl+P"));
    connect(debugExamineMemoryAction, SIGNAL(triggered()), this, SLOT(debugExamineMemory()));

    debugAnyAction = new QAction(tr("Gdb command"), this);
    debugAnyAction->setShortcut(QString("Ctrl+G"));
    connect(debugAnyAction, SIGNAL(triggered()), this, SLOT(debugAnyCommand()));

    debugExitAction = new QAction(tr("Exit"), this);
    debugExitAction->setShortcut(QString("Ctrl+Shift+D"));
    connect(debugExitAction, SIGNAL(triggered()), this, SLOT(debugExit()));

    disableDebugActions();


    settingsAction = new QAction(tr("Settings"), this);
    connect(settingsAction, SIGNAL(triggered()), this, SLOT(openSettings()));

    connect(mainWidget, SIGNAL(currentChanged(int)), this, SLOT(changeActionsState(int))); //disable some actions if get started widget opened
    changeActionsState(mainWidget->currentIndex());

    helpAction = new QAction(tr("Help"), this);
    helpAction->setShortcut(QKeySequence::HelpContents);
    connect(helpAction, SIGNAL(triggered()), this, SLOT(openHelp()));

    aboutAction = new QAction(tr("About"), this);
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(openAbout()));
}

void MainWindow::refreshEditMenu()
{
    CodeEditor *codeEditor = 0;

    //disconnect from previous
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

    //connect to current
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

        QTextCursor textCursor = codeEditor->textCursor();

        commentAction->setEnabled(true);
        uncommentAction->setEnabled(true);
        cutAction->setEnabled(true);
        copyAction->setEnabled(true);
        deleteAction->setEnabled(true);
        //if nothing selected
        if (textCursor.selectionEnd() - textCursor.selectionStart() <= 0) {
            commentAction->setEnabled(false);
            uncommentAction->setEnabled(false);
            cutAction->setEnabled(false);
            copyAction->setEnabled(false);
            deleteAction->setEnabled(false);
        }
    }

    prevCodeEditor = codeEditor;
}

void MainWindow::newFile()
{
    mainWidget->setCurrentIndex(1); //tabs
    Tab *tab = new Tab;
    tab->restoreState(tabSplitterState);
    connect(tab, SIGNAL(splitterMoved(int,int)), this, SLOT(saveTabSplitterState()));
    connect(tab, SIGNAL(buildClick()), this, SLOT(buildProgram()));
    connect(tab, SIGNAL(runClick()), this, SLOT(runProgram()));
    connect(tab, SIGNAL(stopClick()), this, SLOT(stopProgram()));
    connect(tab, SIGNAL(saveAsmClick()), this, SLOT(saveFile()));
    connect(tab, SIGNAL(saveExeClick()), this, SLOT(saveExe()));
    tab->code->setPlainText(startText);
    tabs->addTab(tab, tr("New"));
    tabs->setCurrentWidget(tab);
    connect(tab, SIGNAL(codeChanged()), this, SLOT(setProgramBuildedFlagToFalse()));
}

void MainWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open file"), saveOpenDirectory,
                                                    tr("Assembler source file (*.asm)\n"
                                                       "All files (*.*)"));
    saveOpenDirectory = fileName;
    if (fileName.isEmpty())
        return;
    newFile();
    Tab *curTab = (Tab *) tabs->currentWidget();
    curTab->loadCodeFromFile(fileName);
    setCurrentTabName(fileName);
}

void MainWindow::setCurrentTabName(const QString &filePath, int index)
{
    //Taking name of file (without path)
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
        tab->saveCodeToFile(filePath);
        return true;
    }
}

bool MainWindow::saveAsFile(int index)
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save file"), saveOpenDirectory,
                                                    tr("Assembler source file (*.asm)\n"
                                                       "All files (*.*)"));
    saveOpenDirectory = fileName;
    if (fileName.isEmpty())
        return false;
    Tab *tab;
    if (index == -1)
        tab = (Tab *) tabs->currentWidget();
    else
        tab = (Tab *) tabs->widget(index);
    tab->saveCodeToFile(fileName);
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
                                                    tr("Execution file (*.exe)\n"
                                                       "All files (*.*)"));
    saveOpenDirectory = saveFileName;
    if (saveFileName.isEmpty())
        return;
    QFile exeFile(pathInTemp("SASMprog.exe"));
    exeFile.copy(saveFileName);
}

bool MainWindow::closeApp()
{
    int tabsCount = tabs->count();
    if (tabsCount > 0) {
        //consider all tabs
        tabs->setCurrentIndex(0);
        for (int i = 0; i < tabsCount; i++) {
            if (!deleteTab(tabs->currentIndex(), true)) //left only saved tabs
                return false;
        }
    }
    writeSettings();
    closeFromCloseAll = true;
    this->close();
    return true;
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    closeAllChildWindows();
    if (!closeFromCloseAll)
        if (closeApp())
            e->accept();
        else
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
    if (memoryDebugWindow) {
        memoryDebugWindow->close();
        delete memoryDebugWindow;
    }
    if (memoryExamineWindow) {
        memoryExamineWindow->close();
        delete memoryExamineWindow;
    }
    if (anyCommandDebugWindow) {
        anyCommandDebugWindow->close();
        delete anyCommandDebugWindow;
    }
    if (help) {
        help->close();
        delete help;
    }
    if (registersWindow) {
        registersWindow->close();
        registersWindow->clear();
        delete registersWindow;
        registersWindow = 0;
    }
}

bool MainWindow::deleteTab(int index, bool saveFileName)
{
    if (okToContinue(index)) {
        if (tabs->count() == 1)
            mainWidget->setCurrentIndex(0); //get started
        Tab *tabForDeleting = (Tab *) tabs->widget(index);
        if (!saveFileName || tabForDeleting->getCurrentFilePath().isEmpty()) {
            tabs->removeTab(index);
            delete tabForDeleting;
        } else {
            //if tab did not delete
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
    if (deletingTab->isCodeModified()) {
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

void MainWindow::setHighlighter(int index)
{
    if (tabs->count() == 0)
        return;
    Tab *currentTab = (Tab *) tabs->widget(index);
    highlighter->setDocument(currentTab->getCodeDocument());
}

QString MainWindow::pathInTemp(QString path, bool forCygwin)
{
    QString temp = QDir::tempPath();
    QChar lastSymbol = temp[temp.length() - 1];
    if (lastSymbol == QChar('/') || lastSymbol == QChar('\\')) {
        temp.chop(1);
    }
    if (! QFile::exists(temp + "/SASM")) {
        QDir().mkpath(temp + "/SASM");
    }
    if (forCygwin) {
        #ifdef Q_OS_WIN32
            temp.remove(1, 1);
            temp = "/cygdrive/" + temp;
        #endif
    }
    return temp + "/SASM/" + path;
}

QString MainWindow::applicationDataPath()
{
    #ifdef Q_OS_WIN32
        QString appDir = QCoreApplication::applicationDirPath();
        if (! QFile::exists(appDir + "/NASM")) {
            appDir = QCoreApplication::applicationDirPath() + "/Windows";
        }
        if (! QFile::exists(appDir + "/NASM")) {
            appDir = QCoreApplication::applicationDirPath();
        }
        return appDir;
    #else
        QString path = QCoreApplication::applicationDirPath();
        QString appDir = path.left(path.length() - 4) + QString("/share/sasm"); //replace /bin with /share/sasm
        if (! QFile::exists(appDir)) {
            appDir = QCoreApplication::applicationDirPath() + "/share/sasm";
        }
        if (! QFile::exists(appDir)) {
            appDir = QCoreApplication::applicationDirPath() + "/Linux/share/sasm";
        }
        if (! QFile::exists(appDir)) {
            appDir = QCoreApplication::applicationDirPath();
        }
        return appDir;
    #endif
}

void MainWindow::buildProgram(bool debugMode)
{
    printLogWithTime(tr("Build started...") + '\n', Qt::black);
    QCoreApplication::processEvents();

    if (! QFile::exists(applicationDataPath() + "/NASM")) {
        printLogWithTime(tr("Error! Directory NASM does not exist. Please reinstall the program.") + '\n', Qt::red);
        QMessageBox::critical(0, tr("Error!"), tr("Directory NASM does not exist. Please reinstall the program."));
        return;
    }

    QString path = pathInTemp("program.asm");
    QFile::remove(path);
    QString exePath = pathInTemp("SASMprog.exe");
    QFile::remove(exePath);

    while (QFile::exists(path)) {
    }

    Tab *currentTab = (Tab *) tabs->currentWidget();
    ioIncIncluded = currentTab->isIoIncIncluded();
    currentTab->saveCodeToFile(path, false, debugMode);

    if (debugMode) {
        //save input to file
        QString inputPath = pathInTemp("input.txt");
        Tab *currentTab = (Tab *) tabs->currentWidget();
        currentTab->saveInputToFile(inputPath);
        //create output file
        QString outputPath = pathInTemp("output.txt");
        QFile output;
        output.setFileName(outputPath);
        output.open(QIODevice::WriteOnly | QIODevice::Text);
    }

    while (! QFile::exists(path)) {
    }

    //NASM
    #ifdef Q_OS_WIN32
        QString nasm = applicationDataPath() + "/NASM/nasm.exe";
        QString objFormat = "win32";
    #else
        QString nasm = "nasm";
        QString objFormat = "elf32";
    #endif
    QStringList nasmArguments;
    nasmArguments << "-g" << "-f" << objFormat << pathInTemp("program.asm", true) << "-l" <<
                     pathInTemp("program.lst", true) << "-o" << pathInTemp("program.o", true);
    QProcess nasmProcess;
    QString nasmOutput = pathInTemp("compilererror.txt");
    nasmProcess.setStandardOutputFile(nasmOutput);
    nasmProcess.setStandardErrorFile(nasmOutput, QIODevice::Append);
    nasmProcess.setWorkingDirectory(applicationDataPath());
    nasmProcess.start(nasm, nasmArguments);
    nasmProcess.waitForFinished();

    //GCC
    QString stdioMacros = pathInTemp("macro.o");
    QFile macro;
    #ifdef Q_OS_WIN32
        QString gcc = applicationDataPath() + "/NASM/MinGW/bin/gcc.exe";
        macro.setFileName(applicationDataPath() + "/NASM/macro.o");
        macro.copy(stdioMacros);
    #else
        QString gcc = "gcc";
        macro.setFileName(applicationDataPath() + "/NASM/macro.c");
        macro.copy(pathInTemp("macro.c"));

        //macro.c compilation
        QStringList gccMArguments;
        gccMArguments << "-x" << "c" << pathInTemp("macro.c") << "-c" << "-g" << "-o" << stdioMacros << "-m32";
        QProcess gccMProcess;
        gccMProcess.start(gcc, gccMArguments);
        gccMProcess.waitForFinished();
    #endif
    //final linking
    QStringList gccArguments;
    gccArguments << pathInTemp("program.o") << stdioMacros << "-g" << "-o" << pathInTemp("SASMprog.exe") << "-m32";
    QProcess gccProcess;
    QString gccOutput = pathInTemp("linkererror.txt");
    gccProcess.setStandardOutputFile(gccOutput);
    gccProcess.setStandardErrorFile(gccOutput, QIODevice::Append);
    gccProcess.start(gcc, gccArguments);
    gccProcess.waitForFinished();

    QFile logFile;
    logFile.setFileName(nasmOutput);
    logFile.open(QIODevice::ReadOnly);
    QTextStream log(&logFile);
    QString logText = log.readAll();
    if (!logText.isEmpty()) {
        printLogWithTime(tr("Warning! Errors have occurred in the build:") + '\n', Qt::red);
        printLog(logText, Qt::red);
        QMessageBox::critical(0, tr("Warning!"), tr("Errors have occurred in the build!"));
        programIsBuilded = false;
    } else {
        logFile.setFileName(gccOutput);
        logFile.open(QIODevice::ReadOnly);
        QTextStream logLinker(&logFile);
        logText = logLinker.readAll();

        if (!logText.isEmpty()) {
            printLogWithTime(tr("Warning! Errors have occurred in the build:") + '\n', Qt::red);
            printLog(logText, Qt::red);
            QMessageBox::critical(0, tr("Warning!"), tr("Errors have occurred in the build!"));
            programIsBuilded = false;
        } else {
            printLogWithTime(tr("Built successfully.") + '\n', Qt::darkGreen);
            programIsBuilded = true;
        }
    }
}

void MainWindow::runProgram()
{
    if (!programIsBuilded) {
        buildProgram();
    }
    if (!programIsBuilded) {
       return;
    }

    stopAction->setEnabled(true);

    ((Tab *) tabs->currentWidget())->clearOutput();

    printLogWithTime(tr("The program is executing...") + '\n', Qt::black);
    QCoreApplication::processEvents();

    QString output = pathInTemp("output.txt");
    QString input = pathInTemp("input.txt");
    Tab *currentTab = (Tab *) tabs->currentWidget();
    currentTab->saveInputToFile(input);

    QString program = pathInTemp("SASMprog.exe");
    runProcess->setStandardInputFile(input);
    runProcess->setStandardOutputFile(output);
    runProcess->start(program);

    timer->start(100);
    //Previous - connect(timer, SIGNAL(timeout()), this, SLOT(testStopOfProgram()));
}

void MainWindow::testStopOfProgram()
{
    QCoreApplication::processEvents();
    if (runProcess->state() == QProcess::NotRunning) {
        stopAction->setEnabled(false);
        Tab *currentTab = (Tab *) tabs->currentWidget();
        currentTab->loadOutputFromFile(pathInTemp("output.txt"));
        printLogWithTime(tr("The program finished.") + '\n', Qt::darkGreen);
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

    QString program = pathInTemp("SASMprog.exe");
    runProcess->startDetached(program);
}

void MainWindow::stopProgram()
{
    if (timer->isActive()) {
        timer->stop();

        stopAction->setEnabled(false);

        runProcess->kill();

        printLogWithTime(tr("The program stopped.") + '\n', Qt::darkGreen);
        QMessageBox::information(0, QString("SASM"), tr("The program stopped."));
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

void MainWindow::debug()
{
    buildProgram(true);
    if (!programIsBuilded) {
        printLogWithTime(tr("Before debugging you need to build the program.") + '\n', Qt::red);
        return;
    }
    ((Tab *) tabs->currentWidget())->clearOutput();
    printLogWithTime(tr("Debugging started...") + '\n', Qt::darkGreen);
    QString path = pathInTemp("SASMprog.exe");
    debugger = new Debugger(compilerOut, path, ioIncIncluded, pathInTemp(QString()));
    connect(debugger, SIGNAL(highlightLine(int)), this, SLOT(highlightDebugLine(int)));
    connect(debugger, SIGNAL(finished()), this, SLOT(debugExit()), Qt::QueuedConnection);
    connect(debugger, SIGNAL(started()), this, SLOT(enableDebugActions()));
    connect(debugger, SIGNAL(printRegisters(Debugger::registersInfo*)), this, SLOT(printRegisters(Debugger::registersInfo*)));
}

void MainWindow::enableDebugActions()
{
    debugAction->setEnabled(false);
    debugNextAction->setEnabled(true);
    debugNextNiAction->setEnabled(true);
    debugShowRegistersAction->setEnabled(true);
    debugMemoryDialogAction->setEnabled(true);
    debugExamineMemoryAction->setEnabled(true);
    debugAnyAction->setEnabled(true);
    debugExitAction->setEnabled(true);
}

void MainWindow::disableDebugActions()
{
    debugAction->setEnabled(true);
    debugNextAction->setEnabled(false);
    debugNextNiAction->setEnabled(false);
    debugShowRegistersAction->setEnabled(false);
    debugMemoryDialogAction->setEnabled(false);
    debugExamineMemoryAction->setEnabled(false);
    debugAnyAction->setEnabled(false);
    debugExitAction->setEnabled(false);
}

void MainWindow::debugNext()
{
    debugger->doInput(QString("si\n"), si);
    debugShowRegisters();
}

void MainWindow::debugNextNi()
{
    debugger->doInput(QString("ni\n"), ni);
    debugShowRegisters();
}

void MainWindow::debugShowRegisters()
{
    if (debugShowRegistersAction->isChecked()) {
        debugger->doInput(QString("info registers\n"), infoRegisters);
    } else
        if (registersWindow) {
            registersWindow->close();
            registersWindow->clear();
            delete registersWindow;
            registersWindow = 0;
        }
}

void MainWindow::printRegisters(Debugger::registersInfo *registers)
{
    int tableHeight, tableWidth;
    //create table
    if (!registersWindow) {
        registersWindow = new RegistersWindow(16, 3, this);
        connect(registersWindow, SIGNAL(closeSignal()), this, SLOT(setShowRegistersToUnchecked()));
        registersWindow->setWindowFlags(Qt::Window);

        tableWidth = registersWindow->horizontalHeader()->length() + 2;
        registersWindow->move(QDesktopWidget().availableGeometry().width() - tableWidth, 80);

        registersWindow->verticalHeader()->hide();
        registersWindow->setEditTriggers(QAbstractItemView::NoEditTriggers);
        registersWindow->setSelectionMode(QAbstractItemView::NoSelection);
        QStringList header;
        header << tr("Register") << tr("Hex") << tr("Integer");
        registersWindow->setHorizontalHeaderLabels(header);
        registersWindow->setWindowTitle(tr("Registers"));
    }

    //fill table
    if (!registersWindow->item(0, 0)) {
        for (int i = 0; i < 16; i++) {
            QTableWidgetItem *name = new QTableWidgetItem(registers[i].name);
            QTableWidgetItem *hexValue = new QTableWidgetItem(registers[i].hexValue);
            QTableWidgetItem *decValue = new QTableWidgetItem(registers[i].decValue);
            registersWindow->setItem(i, 0, name);
            registersWindow->setItem(i, 1, hexValue);
            registersWindow->setItem(i, 2, decValue);
        }
    } else {
        for (int i = 0; i < 16; i++) {
            registersWindow->item(i, 0)->setText(registers[i].name);
            registersWindow->item(i, 1)->setText(registers[i].hexValue);
            registersWindow->item(i, 2)->setText(registers[i].decValue);
        }
    }

    //adjust size
    registersWindow->resizeColumnsToContents();
    tableHeight = registersWindow->horizontalHeader()->height() +
            registersWindow->verticalHeader()->length() + 2;
    registersWindow->setFixedHeight(tableHeight);
    tableWidth = registersWindow->horizontalHeader()->length() + 2;
    registersWindow->setFixedWidth(tableWidth);
    registersWindow->resizeColumnsToContents();

    //show
    registersWindow->show();
    QThread::msleep(50);
    this->activateWindow();
}

void MainWindow::setShowRegistersToUnchecked()
{
    debugShowRegistersAction->setChecked(false);
}

void MainWindow::debugExit()
{
    delete debugger;
    Tab *currentTab = (Tab *) tabs->currentWidget();
    currentTab->loadOutputFromFile(pathInTemp("output.txt"));
    debugShowRegistersAction->setChecked(false);
    printLogWithTime(tr("Debugging finished.") + '\n', Qt::darkGreen);
    disableDebugActions();
}

void MainWindow::highlightDebugLine(int lineNum)
{
    Tab *curTab = (Tab *) tabs->widget(tabs->currentIndex());
    curTab->highlightDebugLine(lineNum);
}

void MainWindow::debugOpenMemoryDialog()
{
    if (!memoryDebugWindow) {
        memoryDebugWindow = new MemoryDebugWindow;
        memoryDebugWindow->setWindowTitle(tr("Show memory"));
        connect(memoryDebugWindow, SIGNAL(addVariable(QString, bool)), this, SLOT(debugAddVariable(QString, bool)));
    }
    memoryDebugWindow->show();
}

void MainWindow::debugExamineMemory()
{
    if (!memoryExamineWindow) {
        memoryExamineWindow = new MemoryDebugWindow;
        memoryExamineWindow->setWindowTitle(tr("Examine memory at address"));
        connect(memoryExamineWindow, SIGNAL(addVariable(QString, bool)), this, SLOT(debugAddExamine(QString, bool)));
    }
    memoryExamineWindow->show();
}

void MainWindow::debugAnyCommand()
{
    if (!anyCommandDebugWindow) {
        anyCommandDebugWindow = new CommandDebugWindow;
        connect(anyCommandDebugWindow, SIGNAL(runCommand(QString)), this, SLOT(debugRunCommand(QString)));
    }
    anyCommandDebugWindow->show();
}

void MainWindow::debugRunCommand(QString command)
{
    debugger->doInput(command + "\n", anyAction);
}

void MainWindow::debugAddVariable(QString variableName, bool once)
{
    //also may be registers with $ start
    if (once)
        debugger->doInput(QString("p ") + variableName + QString("\n"), infoMemory);
    else
        debugger->doInput(QString("display ") + variableName + QString("\n"), infoMemory);
}

void MainWindow::debugAddExamine(QString variableName, bool once)
{
    //also may be registers with $ start and number addresses
    if (once)
        debugger->doInput(QString("p *(") + variableName + QString(")\n"), infoMemory);
    else
        debugger->doInput(QString("display *(") + variableName + QString(")\n"), infoMemory);
}

void MainWindow::find()
{
    //remove current line highlight
    disconnect(((Tab *) tabs->currentWidget())->code, SIGNAL(cursorPositionChanged()),
               ((Tab *) tabs->currentWidget())->code, SLOT(highlightCurrentLine()));
    ((Tab *) tabs->currentWidget())->code->setExtraSelections(QList<QTextEdit::ExtraSelection>());

    if (!findDialog) {
        findDialog = new FindDialog;
        findDialog->
        connect(findDialog, SIGNAL(findNext(QString,Qt::CaseSensitivity,bool,bool,QString)),
                this, SLOT(findNext(QString,Qt::CaseSensitivity,bool,bool,QString)));
    }
    findDialog->show();
    findDialog->activateWindow();
}

void MainWindow::findNext(const QString &pattern, Qt::CaseSensitivity cs, bool all, bool replace, const QString &replaceText)
{
    if (pattern.isEmpty()) {
        //retrieve current line highlight
        for (int  i = 0; i < tabs->count(); i++) {
            connect(((Tab *) tabs->widget(i))->code, SIGNAL(cursorPositionChanged()),
                   ((Tab *) tabs->widget(i))->code, SLOT(highlightCurrentLine()));
            ((Tab *) tabs->widget(i))->code->highlightCurrentLine();
        }
    }
    QTextDocument *document = ((Tab *) tabs->currentWidget())->getCodeDocument();
    if (!prevFindPattern.isEmpty()) {
        //reset previous find highlight
        QTextCursor cursor(document);
        while (!cursor.isNull() && !cursor.atEnd()) {
            if (prevCs == Qt::CaseSensitive)
                cursor = document->find(prevFindPattern, cursor, QTextDocument::FindCaseSensitively);
            else
                cursor = document->find(prevFindPattern, cursor, 0);
            if (!cursor.isNull()) {
                QTextCharFormat colorFormat;
                colorFormat.setBackground(QBrush(Qt::white));
                cursor.setCharFormat(colorFormat);
            }
        }
    }
    if (all) { //find all
        QTextCursor newCursor = QTextCursor(document);
        while (!newCursor.isNull() && !newCursor.atEnd()) {
            if (cs == Qt::CaseSensitive)
                newCursor = document->find(pattern, newCursor, QTextDocument::FindCaseSensitively);
            else
                newCursor = document->find(pattern, newCursor, 0);
            if (replace) { //replace mode
                newCursor.removeSelectedText();
                newCursor.insertText(replaceText);
            }
            if (!newCursor.isNull()) {
                QTextCharFormat colorFormat;
                colorFormat.setBackground(QBrush(Qt::green));
                newCursor.setCharFormat(colorFormat);
            }
        }
    } else { //find next only
        static QTextCursor newCursor(document);
        if (replace) { //replace mode
            newCursor.removeSelectedText();
            newCursor.insertText(replaceText);
        }
        if (cs == Qt::CaseSensitive)
            newCursor = document->find(pattern, newCursor, QTextDocument::FindCaseSensitively);
        else
            newCursor = document->find(pattern, newCursor, 0);
        if (newCursor.isNull()) { //continue from start
            if (cs == Qt::CaseSensitive)
                newCursor = document->find(pattern, newCursor, QTextDocument::FindCaseSensitively);
            else
                newCursor = document->find(pattern, newCursor, 0);
        }
        if (!newCursor.isNull()) {
            QTextCharFormat colorFormat;
            colorFormat.setBackground(QBrush(Qt::green));
            newCursor.setCharFormat(colorFormat);
        }
    }
    prevFindPattern = pattern;
}

void MainWindow::restorePrevSession(bool notNotify)
{
    QSettings settings("SASM Project", "SASM");
    int count = settings.value("tabscount", 0).toInt();
    if (count == 0 && !notNotify) {
        QMessageBox::information(0, QString("SASM"), tr("In the previous session was not open any of the saved tabs!"));
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
    QSettings settings("SASM Project", "SASM");

    //GUI
    settings.beginGroup("MainWindow");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    unsigned int maxFlag = windowState() & Qt::WindowMaximized;
    settings.setValue("maximized", maxFlag);
    settings.endGroup();

    //Opened tabs
    int prevTabsCount = settings.value("tabscount", 0).toInt();
    for (int i = 0; i < prevTabsCount; i++) //remove previous
        settings.remove(QString("Tabs/") + QString::number(i));

    settings.setValue("tabscount", tabs->count()); //create current
    settings.beginGroup("Tabs");
    for (int i = 0; i < tabs->count(); i++) {
        settings.setValue(QString::number(i), ((Tab *) tabs->widget(i))->getCurrentFilePath());
    }
    settings.endGroup();

    //save and open directory
    settings.setValue("saveopendirectory", saveOpenDirectory);

    //splitters state
    settings.setValue("tabsplitterstate", tabSplitterState);
    settings.setValue("logsplitterstate", splitter->saveState());
}

void MainWindow::openSettings()
{
    QSettings settings("SASM Project", "SASM");
    if (!settingsWindow) {
        settingsWindow = new QWidget;
        settingsUi.setupUi(settingsWindow);
        connect(settingsUi.buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), this, SLOT(exitSettings()));
        connect(settingsUi.buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(saveSettings()));
        connect(settingsUi.buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), settingsWindow, SLOT(close()));
        connect(settingsUi.resetAllButton, SIGNAL(clicked()), this, SLOT(resetAllSettings()));
    }
    settingsUi.startWindow->setCurrentIndex(settings.value("startwindow", 0).toInt());
    settingsUi.codePosition->setCurrentIndex(settings.value("codeposition", 0).toInt());
    settingsUi.language->setCurrentIndex(settings.value("language", 0).toInt());

    if (!settingsStartTextEditor) {
        settingsStartTextEditor = new CodeEditor;
        settingsHighlighter = new Highlighter;
        settingsHighlighter->setDocument(settingsStartTextEditor->document());
        QVBoxLayout *startTextLayout = new QVBoxLayout;
        startTextLayout->addWidget(settingsStartTextEditor);
        settingsUi.startTextWidget->setLayout(startTextLayout);
    }
    settingsStartTextEditor->setPlainText(settings.value("starttext").toString());

    settingsWindow->show();
}

void MainWindow::saveSettings()
{
    QSettings settings("SASM Project", "SASM");
    settings.setValue("startwindow", settingsUi.startWindow->currentIndex());
    settings.setValue("codeposition", settingsUi.codePosition->currentIndex());
    settings.setValue("starttext", settingsStartTextEditor->toPlainText());
    settings.setValue("language", settingsUi.language->currentIndex());
    startText = settingsStartTextEditor->toPlainText();
}

void MainWindow::exitSettings()
{
    saveSettings();
    settingsWindow->close();
}

void MainWindow::resetAllSettings()
{
    //ask before execution
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

    QSettings settings("SASM Project", "SASM");

    settings.remove("MainWindow/size");
    settings.remove("MainWindow/pos");

    int prevTabsCount = settings.value("tabscount", 0).toInt();
    for (int i = 0; i < prevTabsCount; i++) //remove previous
        settings.remove(QString("Tabs/") + QString::number(i));

    settings.clear();

    closeFromCloseAll = true;
    this->close();
}

void MainWindow::changeActionsState(int widgetIndex)
{
    if (widgetIndex == 0) { //get started window
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
        debugExitAction->setEnabled(false);
        debugShowRegistersAction->setEnabled(false);
        debugMemoryDialogAction->setEnabled(false);
        debugExamineMemoryAction->setEnabled(false);
        debugAnyAction->setEnabled(false);
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
    }
}

void MainWindow::openHelp()
{
    help = new QTextBrowser;
    help->setAttribute(Qt::WA_DeleteOnClose);
    QFile helpFile(":help/help.html");
    helpFile.open(QFile::ReadOnly);
    QTextStream helpText(&helpFile);
    help->setHtml(helpText.readAll());
    help->setWindowState(Qt::WindowMaximized);
    help->setOpenExternalLinks(true);
    help->setWindowTitle(tr("Help"));
    help->show();
}

void MainWindow::saveTabSplitterState(){
    tabSplitterState = ((Tab *) tabs->currentWidget())->saveState();
}

void MainWindow::openAbout()
{
    QMessageBox::about(this, tr("About SASM"),
                       tr("SASM (SimpleASM) 2.0 - simple Open Source IDE for NASM.") + '\n' +
                       tr("Licensed under the GNU GPL v3.0") + '\n' +
                       tr("Based on the Qt.") + '\n' +
                       tr("Copyright © 2013 Dmitriy Manushin") + '\n' +
                       tr("Development - Dmitriy Manushin") + '\n' +
                       tr("Icon and advices - Alick Gaybullaev") + '\n' + '\n' +
                       tr("Wishes and error messages are sent to the e-mail: Dman1095@gmail.com") + '\n' + '\n' +
                       tr("Donate:") + '\n' +
                       tr("WMZ - Z282016332582") + '\n' +
                       tr("WMR - R331674303467"));
}

bool MainWindow::removeDirRecuresively(const QString &dirName){
    bool result = true;
    QDir dir(dirName);

    if (dir.exists(dirName)) {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
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
    //delete all temporary files
    removeDirRecuresively(pathInTemp(QString()));
}
