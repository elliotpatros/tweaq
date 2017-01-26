#include "mainwindow.h"
#include "ui_mainwindow.h"

// constructor and destructor
MainWindow::MainWindow(QWidget* const parent) :
    QMainWindow(parent),
    _ui(new Ui::MainWindow),
    _audioFiles(new AF_Model(this)),
    _externals(new ExternalInterfaceManager(AppMeta::pathToBundledExternals()))
{
    // expand user interface
    _ui->setupUi(this);

    // setup connection rules
    disconnect();

    // setup main window
    setWindowTitle(AppMeta::windowName());
    resize(AppMeta::preferredWindowSize());
    AppMeta::centerWidgetOnScreen(this);

    // setup tree view
    _ui->treeView->setModel(_audioFiles->sortProxy());
    _ui->treeView->hideColumn(AF_Properties::AbsolutePath);

    // setup combo box dsp
    _ui->comboBoxDSP->setTitle("dsp...");
    _ui->comboBoxDSP->addItems(_externals->processNames());

    // setup combo box remove
    _ui->comboBoxRemove->setTitle("remove...");
    _ui->comboBoxRemove->addNamedAction("finished", [this]{removeFinishedAudioFiles();});
    _ui->comboBoxRemove->addNamedAction("selected", [this]{removeSelectedAudioFiles();});
    _ui->comboBoxRemove->addNamedAction("everything", [this]{removeAllAudioFiles();});

    // custom connection rules
    connect(_ui->buttonExportTo, SIGNAL(released()), SLOT(exportFolderDialog()));
    connect(_ui->buttonProcess, SIGNAL(released()), SLOT(processAudioFiles()));

    connect(_ui->treeView, SIGNAL(droppedUrls(QList<QUrl>)), SLOT(importAudioFiles(QList<QUrl>)));
//    connect(_ui->treeView, SIGNAL(droppedUrls(QList<QUrl>)), SLOT(importOnBackgroundThread(QList<QUrl>)));

    connect(_ui->actionChooseExportFolder, SIGNAL(triggered(bool)), SLOT(exportFolderDialog()));
    connect(_ui->actionImportAudioFiles, SIGNAL(triggered(bool)), SLOT(importFilesDialog()));
    connect(_ui->actionImportFolder, SIGNAL(triggered(bool)), SLOT(importFolderDialog()));
    connect(_ui->actionProcess, SIGNAL(triggered(bool)), SLOT(processAudioFiles()));
    connect(_ui->actionRemoveAll, SIGNAL(triggered(bool)), SLOT(removeAllAudioFiles()));
    connect(_ui->actionRemoveSelected, SIGNAL(triggered(bool)), SLOT(removeSelectedAudioFiles()));
    connect(_ui->actionShowExportFolder, SIGNAL(triggered(bool)), SLOT(showExportFolder()));
    connect(_ui->actionRemoveFinished, SIGNAL(triggered(bool)), SLOT(removeFinishedAudioFiles()));
    connect(_ui->actionCloseTweaq, SIGNAL(triggered(bool)), SLOT(close()));
    connect(_ui->actionMinimize, SIGNAL(triggered(bool)), SLOT(showMinimized()));
    connect(_ui->buttonTest, SIGNAL(released()), SLOT(testBackgroundThread()));

    // drag and drop
    setAcceptDrops(false);
}

MainWindow::~MainWindow()
{
    delete _ui;
}

// application actions
void MainWindow::exportFolderDialog()
{
    const QString folderName =
            QFileDialog::getExistingDirectory(this, "save audio files to...", _ui->lineEdit->path());

    if (!folderName.isEmpty())
    {
        _ui->lineEdit->setText(folderName);
    }
}

void MainWindow::importFilesDialog()
{
    const QUrl openFolder(QUrl::fromLocalFile(_ui->lineEdit->path()));
    importAudioFiles(QFileDialog::getOpenFileUrls(this, "import audio files...", openFolder));
}

void MainWindow::importFolderDialog()
{
    const QUrl openAt(QUrl::fromLocalFile(_ui->lineEdit->path()));
    const QUrl opened(QFileDialog::getExistingDirectoryUrl(this, "import folder...", openAt));
    importAudioFiles(QList<QUrl>() << opened);
}

void MainWindow::showExportFolder()
{
    system(QString("open %1").arg(_ui->lineEdit->path()).toUtf8());
}

void MainWindow::importAudioFiles(const QList<QUrl> urls)
{
    QThread* thread = new QThread;
    BackgroundWorker* worker = new BackgroundWorker(urls);
    worker->moveToThread(thread);

    connect(worker, SIGNAL(error(QString)), SLOT(errorString(QString)));
    connect(thread, SIGNAL(started()), worker, SLOT(importAudioFiles()));
    connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->start();


//    ui->treeView->setSortingEnabled(false);
//    _audioFiles->importAudioFiles(urls);
//    ui->treeView->setSortingEnabled(true);
//    ui->treeView->clearSelection();
}

void MainWindow::removeAllAudioFiles()
{
    _ui->treeView->setSortingEnabled(false);
    _audioFiles->removeAllRows();
    _ui->treeView->setSortingEnabled(true);
    _ui->treeView->clearSelection();
}

void MainWindow::removeSelectedAudioFiles()
{
    _ui->treeView->setSortingEnabled(false);
    _audioFiles->removeSelectedRows(_ui->treeView->selected());
    _ui->treeView->setSortingEnabled(true);
    _ui->treeView->clearSelection();
}

void MainWindow::removeFinishedAudioFiles()
{
    _ui->treeView->setSortingEnabled(false);
    _audioFiles->removeFinishedRows();
    _ui->treeView->setSortingEnabled(true);
    _ui->treeView->clearSelection();
}

void MainWindow::processAudioFiles()
{
    const int processId = _ui->comboBoxDSP->processIndex();

    // are we ready to process?
    if (!_audioFiles->areAnyAudioFilesUnprocessed()) return importFilesDialog();
    if (!_externals->isValid(processId))             return _ui->comboBoxDSP->showPopup();
    if (!_ui->lineEdit->currentPathIsValid())         return exportFolderDialog();

    // get parameter entries from user
    ExternalInterface* external(_externals->get(processId));
    if (external == nullptr) return;

    // make a dialog window for the user to enter parameters
    unique_ptr<ParameterDialog> dialog(new ParameterDialog(_ui->comboBoxDSP->currentText(), this));
    if (dialog.get() == nullptr) return;
    if (dialog->show(external->setup()) == QDialog::Rejected) return;

    // get entries from user
    void* arguments = external->handleInput(dialog->entries());

    // process files
    _audioFiles->rootItem()->processAudioFiles(external->process(), _ui->lineEdit->path(), arguments);

    // clean up
    if (arguments != nullptr) free(arguments);
}



// test: deletme
//void MainWindow::importOnBackgroundThread(const QList<QUrl> urls)
//{
//    QThread* thread = new QThread;
//    BackgroundWorker* worker = new BackgroundWorker(_audioFiles, _ui, urls);
//    worker->moveToThread(thread);

//    connect(worker, SIGNAL(error(QString)), SLOT(errorString(QString)));
//    connect(thread, SIGNAL(started()), worker, SLOT(importAudioFiles()));
//    connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
//    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

//    thread->start();
//}

void MainWindow::testBackgroundThread()
{
    QThread* thread = new QThread;
    BackgroundWorker* worker = new BackgroundWorker;
    worker->moveToThread(thread);

    connect(worker, SIGNAL(error(QString)), SLOT(errorString(QString)));
    connect(thread, SIGNAL(started()), worker, SLOT(process()));
    connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
    connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->start();
}

void MainWindow::errorString(QString msg)
{
    qWarning() << "error: " << msg;
}

