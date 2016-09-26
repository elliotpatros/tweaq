#include "mainwindow.h"
#include "ui_mainwindow.h"

//==============================================================================
// constructor & destructor
//==============================================================================
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // layout child widgets
    ui->setupUi(this);

    // setup tweaq data structures
    InstanceMeta appInstance;
    _afModel = new AFModel(this);
    _libModel = new LibModel(this, appInstance.externalDirectorySearchPath());

    // setup main window
    setWindowTitle(appInstance.appNameAndVersion());
    setMinimumSize(appInstance.minWindowSize());
    setMaximumSize(appInstance.maxWindowSize());
    resize(appInstance.preferredWindowSize());

    // setup child widgets
    {   // tree view
        TqTreeView* w = ui->treeViewImportedFiles;
        w->setModel(_afModel->sortProxy());
        w->hideColumn(AFItem::AbsolutePath);
        connect(w, SIGNAL(droppedAudioFileUrls(const QList<QUrl>)),
                this, SLOT(addSoundFilesToModel(const QList<QUrl>)));
    }
    {   // combo box remove files
        TqComboBox* w = ui->comboBoxDSP;
        w->setTitle(QStringLiteral("choose process"));
        w->addItems(_libModel->names());
    }
    {   // combo box remove files
        TqComboBox* w = ui->comboBoxRemoveFiles;
        w->setTitle(QStringLiteral("remove files"));
        w->addItems(QStringList()
                    << QStringLiteral("all")
                    << QStringLiteral("selected")
                    << QStringLiteral("completed"));
    }

    // drag and drop
    setAcceptDrops(false);
}

MainWindow::~MainWindow(void)
{
    delete ui;
}


//==============================================================================
// gui callbacks
//==============================================================================
void MainWindow::on_comboBoxRemoveFiles_currentIndexChanged(int)
{
    // turn off gui sorting
    TqTreeView* treeView = ui->treeViewImportedFiles;
    treeView->setSortingEnabled(false);

    // which type of remove was it?
    TqComboBox* comboBox = ui->comboBoxRemoveFiles;
    switch (comboBox->processIndex())
    {
    case 0: // remove all
        _afModel->removeAllRows();
        break;
    case 1: // remove selected
        _afModel->removeSelectedRows(ui->treeViewImportedFiles->selectedRows());
        break;
    case 2: // remove finished
        _afModel->removeFinishedRows();
        break;
    default:
        break;
    }

    // turn gui sorting back on
    treeView->setSortingEnabled(true);

    // reset combo box title
    comboBox->setSelectionToTitleIndex();
}

void MainWindow::on_buttonExportTo_clicked()
{
    openExportDirectoryDialog();
}

void MainWindow::on_buttonProcessFiles_clicked()
{
    // let's find our guis
    TqLineEdit* editDest = ui->lineEditExportTo;
    TqComboBox* chooseDSP = ui->comboBoxDSP;
    const t_int libIndex = chooseDSP->processIndex();

    // STEP 1
    // decide whether we're ready to process stuff
    // ...has the user picked any sounds to edit?
    if (!_afModel->areThereAnyUnprocessedAudioFiles())
    {
        openImportDirectoryDialog();
        return;
    }

    // ...has the user picked a valid sound file destination?
    if (!editDest->isCurrentPathValid())
    {
        openExportDirectoryDialog();
        return;
    }

    // ...has the user picked a DSP process from the combo box?
    if (libIndex < 0)
    {
        chooseDSP->showPopup();
        return;
    }

    // STEP 2
    // ask the external DSP what parameters it needs form the user
    vector<ParameterHandle> handles;
    _libModel->getExternalParameterHandles(libIndex, &handles);

    // make and show the user a parameter dialog
    ParameterDialog dialog(this);
    dialog.appendParameterFields(&handles);
    dialog.makeAndShow(_libModel->name(libIndex));

    QStringList args;
    dialog.getEntries(&args);

    // whew! ok, we're ready to tweaq some shit now.
    // STEP 3
    _libModel->processFiles(libIndex, _afModel->rootItem(), editDest->currentValidFileLocation(), &args);
}


//==============================================================================
// application actions
//==============================================================================
void MainWindow::addSoundFilesToModel(const QList<QUrl> urls)
{
    // turn off gui sorting
    TqTreeView* w = ui->treeViewImportedFiles;
    w->setSortingEnabled(false);

    // import files
    QStringList paths;
    AFImporter importer;
    importer.import(urls, &paths);

    // add audio files to model
    _afModel->addAudioFiles(paths);

    // turn gui sorting back on
    w->setSortingEnabled(true);
}

void MainWindow::openExportDirectoryDialog(void)
{
    TqLineEdit* w = ui->lineEditExportTo;
    QString result = QFileDialog::getExistingDirectory(
                this,
                QStringLiteral("save new sound files to..."),
                w->currentValidFileLocation());

    if (!result.isEmpty()) // result is empty if user canceled
    {
        w->setText(result);
    }
}

void MainWindow::openImportDirectoryDialog(void)
{
    const QUrl openAt = QUrl::fromLocalFile(ui->lineEditExportTo->currentValidFileLocation());
    addSoundFilesToModel(QFileDialog::getOpenFileUrls(this, QStringLiteral("sound files to open..."), openAt));
}
