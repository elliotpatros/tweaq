#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// application header
#include "m_tweaq.h"

// base class
#include <QMainWindow>

// child widgets
#include "tqlineedit.h"
#include "tqtreeview.h"
#include "tqcombobox.h"
#include "parameterdialog.h"

// tweaq data structures
#include "instancemeta.h"
#include "afmodel.h"
#include "libmodel.h"
#include "afimporter.h"

// Qt dialogs
#include <QFileDialog>
#include <QDesktopServices>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = Q_NULLPTR);
    ~MainWindow(void);


private slots:
    // gui callbacks
    void on_comboBoxRemoveFiles_currentIndexChanged(int);
    void on_buttonExportTo_clicked();
    void on_buttonProcessFiles_clicked();
    void on_actionShow_export_folder_triggered();
    void on_actionImport_Audio_Files_triggered();
    void on_actionChoose_Export_Folder_triggered();

    // application actions
    void addSoundFilesToModel(const QList<QUrl> urls);


private:
    // layout (owned)
    Ui::MainWindow *ui;

    // tweaq data structures (owned but Q_OBJECT, don't manually delete)
    AFModel* _afModel;
    LibModel* _libModel;

    // application actions
    void removeImportedFiles(void);
    void processImportedSoundFiles(void);
    void showExportFolderExternally(void);
    void openExportDirectoryDialog(void);
    void openImportDirectoryDialog(void);
};

#endif // MAINWINDOW_H
