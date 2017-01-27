#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

// application header
#include "tweaq.h"

// base class
#include <QProgressDialog>

// Qt classes
#include <QThread>

// todo: check const correctness for Q_OBJECT constructors, especially QWidget parents
class ProgressDialog : public QProgressDialog
{
    Q_OBJECT

public:
    explicit ProgressDialog(const QString title = QStringLiteral("progress..."),
                            QThread* thread = nullptr,
                            QWidget* parent = nullptr);


private slots:
    void askThreadToStop();


private:
    QThread* _thread; // non-owned
};

#endif // PROGRESSDIALOG_H
