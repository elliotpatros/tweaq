#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// application header
#include "tweaq.h"

// base class
#include <QMainWindow>

// widgets
#include "comboboxremove.h"
#include "comboboxdsp.h"
#include "treeviewaudiofile.h"
#include "parameterdialog.h"
#include "progressdialog.h"

// Qt classes
#include <QFileDialog>

// tweaq classes
#include "af_model.h"
#include "externalinterfacemanager.h"
#include "mainwindowhelper.h"
#include "libsf.h"
#include "backgroundworker.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* const parent = nullptr);
    ~MainWindow();


private slots:
    // application actions
    void importAudioFiles(const QList<QUrl> urls);
    void removeAllAudioFiles();
    void removeSelectedAudioFiles();
    void removeFinishedAudioFiles();
    void processAudioFiles();
    void importFilesDialog();
    void importFolderDialog();
    void exportFolderDialog();
    void showExportFolder();
//    void redrawModelView();


private:
    Ui::MainWindow* ui;
    AF_Model* _audioFiles;
    unique_ptr<ExternalInterfaceManager> _externals;
};

#endif // MAINWINDOW_H
