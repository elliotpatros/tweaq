#include "mainwindow.h"
#include "ui_mainwindow.h"

// constructor and destructor
MainWindow::MainWindow(QWidget* const parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _audioFiles(new AF_Model(this)),
    _externals(new ExternalInterfaceManager(AppMeta::pathToBundledExternals()))
{
    // expand user interface
    ui->setupUi(this);

    // setup connection rules
    disconnect();

    // setup main window
    setWindowTitle(AppMeta::windowName());
    resize(AppMeta::preferredWindowSize());
    AppMeta::centerWidgetOnScreen(this);

    // setup tree view
    ui->treeView->setModel(_audioFiles->sortProxy());
    ui->treeView->hideColumn(AF_Properties::AbsolutePath);

    // setup combo box dsp
    ui->comboBoxDSP->setTitle("dsp...");
    ui->comboBoxDSP->addItems(_externals->processNames());

    // setup combo box remove
    ui->comboBoxRemove->setTitle("remove...");
    ui->comboBoxRemove->addNamedAction("finished", [this]{removeFinishedAudioFiles();});
    ui->comboBoxRemove->addNamedAction("selected", [this]{removeSelectedAudioFiles();});
    ui->comboBoxRemove->addNamedAction("everything", [this]{removeAllAudioFiles();});

    // custom connection rules
    connect(ui->buttonExportTo, SIGNAL(released()), SLOT(exportFolderDialog()));
    connect(ui->buttonProcess, SIGNAL(released()), SLOT(processAudioFiles()));
    connect(ui->treeView, SIGNAL(droppedUrls(QList<QUrl>)), SLOT(importAudioFiles(QList<QUrl>)));
    connect(ui->actionChooseExportFolder, SIGNAL(triggered(bool)), SLOT(exportFolderDialog()));
    connect(ui->actionImportAudioFiles, SIGNAL(triggered(bool)), SLOT(importFilesDialog()));
    connect(ui->actionImportFolder, SIGNAL(triggered(bool)), SLOT(importFolderDialog()));
    connect(ui->actionProcess, SIGNAL(triggered(bool)), SLOT(processAudioFiles()));
    connect(ui->actionRemoveAll, SIGNAL(triggered(bool)), SLOT(removeAllAudioFiles()));
    connect(ui->actionRemoveSelected, SIGNAL(triggered(bool)), SLOT(removeSelectedAudioFiles()));
    connect(ui->actionShowExportFolder, SIGNAL(triggered(bool)), SLOT(showExportFolder()));
    connect(ui->actionRemoveFinished, SIGNAL(triggered(bool)), SLOT(removeFinishedAudioFiles()));
    connect(ui->actionCloseTweaq, SIGNAL(triggered(bool)), SLOT(close()));
    connect(ui->actionMinimize, SIGNAL(triggered(bool)), SLOT(showMinimized()));

    // drag and drop
    setAcceptDrops(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// application actions
void MainWindow::exportFolderDialog()
{
    const QString folderName =
            QFileDialog::getExistingDirectory(this, "save audio files to...", ui->lineEdit->path());

    if (!folderName.isEmpty())
    {
        ui->lineEdit->setText(folderName);
    }
}

void MainWindow::importFilesDialog()
{
    const QUrl openFolder(QUrl::fromLocalFile(ui->lineEdit->path()));
    importAudioFiles(QFileDialog::getOpenFileUrls(this, "import audio files...", openFolder));
}

void MainWindow::importFolderDialog()
{
    const QUrl openAt(QUrl::fromLocalFile(ui->lineEdit->path()));
    const QUrl opened(QFileDialog::getExistingDirectoryUrl(this, "import folder...", openAt));
    importAudioFiles(QList<QUrl>() << opened);
}

void MainWindow::showExportFolder()
{
    system(QString("open %1").arg(ui->lineEdit->path()).toUtf8());
}

void MainWindow::importAudioFiles(const QList<QUrl> urls)
{
    ui->treeView->setSortingEnabled(false);
    _audioFiles->importAudioFiles(urls);
    ui->treeView->setSortingEnabled(true);
    ui->treeView->clearSelection();
}

void MainWindow::removeAllAudioFiles()
{
    ui->treeView->setSortingEnabled(false);
    _audioFiles->removeAllRows();
    ui->treeView->setSortingEnabled(true);
    ui->treeView->clearSelection();
}

void MainWindow::removeSelectedAudioFiles()
{
    ui->treeView->setSortingEnabled(false);
    _audioFiles->removeSelectedRows(ui->treeView->selected());
    ui->treeView->setSortingEnabled(true);
    ui->treeView->clearSelection();
}

void MainWindow::removeFinishedAudioFiles()
{
    ui->treeView->setSortingEnabled(false);
    _audioFiles->removeFinishedRows();
    ui->treeView->setSortingEnabled(true);
    ui->treeView->clearSelection();
}

void MainWindow::processAudioFiles()
{
    const int processId = ui->comboBoxDSP->processIndex();

    // are we ready to process?
    if (!_audioFiles->areAnyAudioFilesUnprocessed()) return importFilesDialog();
    if (!_externals->isValid(processId))             return ui->comboBoxDSP->showPopup();
    if (!ui->lineEdit->currentPathIsValid())         return exportFolderDialog();

    // get parameter entries from user
    ExternalInterface* external(_externals->get(processId));
    if (external == nullptr) return;

    // make a dialog window for the user to enter parameters
    unique_ptr<ParameterDialog> dialog(new ParameterDialog(ui->comboBoxDSP->currentText(), this));
    if (dialog.get() == nullptr) return;
    if (dialog->show(external->setup()) == QDialog::Rejected) return;

    // get entries from user
    void* arguments = external->handleInput(dialog->entries());

    // process files
    _audioFiles->rootItem()->processAudioFiles(external->process(), ui->lineEdit->path(), arguments);

    // clean up
    if (arguments != nullptr) free(arguments);
}
