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

    // application actions
    void addSoundFilesToModel(const QList<QUrl> urls);


private:
    // layout (owned)
    Ui::MainWindow *ui;

    // tweaq data structures (owned but Q_OBJECT, don't manually delete)
    AFModel* _afModel;
    LibModel* _libModel;

    // application actions
    void openExportDirectoryDialog(void);
    void openImportDirectoryDialog(void);
};

#endif // MAINWINDOW_H
