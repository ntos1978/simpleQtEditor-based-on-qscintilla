#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QIcon>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPoint>
#include <QSettings>
#include <QSize>
#include <QStatusBar>
#include <QTextStream>
#include <QToolBar>

#include <Qsci/qsciscintilla.h>
#include <Qsci/qscistyle.h>
#include <Qsci/qscilexercpp.h>

MainWindow::MainWindow()
{
    textEdit = new QsciScintilla;
    setCentralWidget(textEdit);

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();

    readSettings();

    connect(textEdit, &QsciScintilla::textChanged, this, &MainWindow::documentWasModified);

    setCurrentFile("");
    // set starting text. Don't do this.
    // textEdit->append("Append text");
    textEdit->setUtf8(true);

    // make current line stand out.
    textEdit->setCaretLineVisible(true);
    // set colour for the current line
    textEdit->setCaretLineBackgroundColor(QColor("lightGray"));
    textEdit->setCaretForegroundColor(QColor("black"));

    //! Выравнивание
    textEdit->setAutoIndent(true);
    textEdit->setIndentationGuides(false);
    textEdit->setIndentationsUseTabs(true);
    textEdit->setIndentationWidth(4);

    //! margin это полоска слева, на которой обычно располагаются breakpoints
    textEdit->setMarginsBackgroundColor(QColor("gainsboro"));
    textEdit->setMarginLineNumbers(1, true);
    textEdit->setMarginWidth(1, 50);

    //! Авто-дополнение кода в зависимости от источника
    textEdit->setAutoCompletionSource(QsciScintilla::AcsAll);
    textEdit->setAutoCompletionCaseSensitivity(true);
    textEdit->setAutoCompletionReplaceWord(true);
    textEdit->setAutoCompletionUseSingle(QsciScintilla::AcusAlways);
    textEdit->setAutoCompletionThreshold(0);

    // With this code, matching pairs of braces are highlighted.
    textEdit->setBraceMatching(QsciScintilla::SloppyBraceMatch);
    textEdit->setMatchedBraceBackgroundColor(Qt::yellow);
    textEdit->setUnmatchedBraceForegroundColor(Qt::blue);

    // This function, setEdgeMode is used to configure the edge mode in a QsciScintilla editor. Edge mode is a
    // feature that helps developers adhere to a specific line length by visually indicating a boundary or edge in the editor.
//    textEdit->setEdgeColor(Qt::green);
//    textEdit->setEdgeColumn(80);
//    textEdit->setEdgeMode(QsciScintilla::EdgeMode::EdgeLine);

    //set colour of font
    textEdit->setColor(Qt::black);
    textEdit->setFont(QFont("Ubuntu"));
    // set colour of document's background
    textEdit->setPaper(Qt::lightGray);
    textEdit->setWhitespaceForegroundColor(Qt::white);
    textEdit->setWhitespaceVisibility(QsciScintilla::WhitespaceVisibility::WsVisible);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::newFile()
{
    if (maybeSave()) {
        textEdit->clear();
        setCurrentFile("");
    }
}

void MainWindow::open()
{
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this);
        if (!fileName.isEmpty())
            loadFile(fileName);
    }
}

bool MainWindow::save()
{
    if (curFile.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

bool MainWindow::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this);
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

void MainWindow::about()
{
   QMessageBox::about(this, tr("About Application"),
            tr("The <b>Application</b> example demonstrates how to "
               "write modern GUI applications using Qt, with a menu bar, "
               "toolbars, and a status bar."));
}

// QMainWindow is a child of QWidget so it inherits setWindowModified(), which signifies that
// this QMainWindow has unsaved changes.
void MainWindow::documentWasModified()
{
    setWindowModified(textEdit->isModified());
}

void MainWindow::createActions()
{
    newAct = new QAction(QIcon(":/images/new.png"), tr("&New"), this);
    newAct->setShortcut(tr("Ctrl+N"));
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

    openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
    openAct->setShortcut(tr("Ctrl+O"));
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    saveAct = new QAction(QIcon(":/images/save.png"), tr("&Save"), this);
    saveAct->setShortcut(tr("Ctrl+S"));
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAct = new QAction(tr("Save &As..."), this);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    cutAct = new QAction(QIcon(":/images/cut.png"), tr("Cu&t"), this);
    cutAct->setShortcut(tr("Ctrl+X"));
    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
    connect(cutAct, SIGNAL(triggered()), textEdit, SLOT(cut()));

    copyAct = new QAction(QIcon(":/images/copy.png"), tr("&Copy"), this);
    copyAct->setShortcut(tr("Ctrl+C"));
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(copyAct, SIGNAL(triggered()), textEdit, SLOT(copy()));

    pasteAct = new QAction(QIcon(":/images/paste.png"), tr("&Paste"), this);
    pasteAct->setShortcut(tr("Ctrl+V"));
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    connect(pasteAct, SIGNAL(triggered()), textEdit, SLOT(paste()));

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    cutAct->setEnabled(false);
    copyAct->setEnabled(false);
    connect(textEdit, SIGNAL(copyAvailable(bool)), cutAct, SLOT(setEnabled(bool)));
    connect(textEdit, SIGNAL(copyAvailable(bool)), copyAct, SLOT(setEnabled(bool)));

    undoAct = new QAction(this);
    undoAct->setIcon(QIcon::fromTheme("edit-undo"));
    connect(undoAct, &QAction::triggered, textEdit, &QsciScintilla::undo );
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
}

void MainWindow::createToolBars()
{
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(newAct);
    fileToolBar->addAction(openAct);
    fileToolBar->addAction(saveAct);

    editToolBar = addToolBar(tr("Edit"));
    editToolBar->addAction(cutAct);
    editToolBar->addAction(copyAct);
    editToolBar->addAction(pasteAct);
    editToolBar->addAction(undoAct);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::readSettings()
{
    QSettings settings("Trolltech", "Application Example");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    resize(size);
    move(pos);
}

void MainWindow::writeSettings()
{
    QSettings settings("Trolltech", "Application Example");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}

bool MainWindow::maybeSave()
{
    if (textEdit->isModified()) {
        // This warning function is deprecated, use Save and Discard instead of yes and no
        // int ret = QMessageBox::warning(this, tr("Application"),
        //              tr("The document has been modified.\n"
        //                 "Do you want to save your changes?"),
        //              QMessageBox::Yes | QMessageBox::Default,
        //              QMessageBox::No,
        //              QMessageBox::Cancel | QMessageBox::Escape);
        int ret = QMessageBox::warning(this, tr("Application"),
                                       tr("The document has been modified.\n"
                                          "Do you want to save your changes?"),
                                       QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                                       QMessageBox::Save);
        if (ret == QMessageBox::Save)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

void MainWindow::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    textEdit->setText(in.readAll());
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File loaded"), 2000);
}

bool MainWindow::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << textEdit->text();
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    textEdit->setModified(false);
    setWindowModified(false);

    QString shownName;
    if (curFile.isEmpty())
        shownName = "untitled.txt";
    else
        shownName = strippedName(curFile);

    setWindowTitle(tr("%1[*] - %2").arg(shownName).arg(tr("Application")));
}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

