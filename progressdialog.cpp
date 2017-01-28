#include "progressdialog.h"

ProgressDialog::ProgressDialog(const QString title, QThread* thread, QWidget* parent) :
    QProgressDialog(title, QStringLiteral("Cancel"), 0, 0, parent),
    _thread(thread)
{
    // setup custom connections
    disconnect();
    connect(this, SIGNAL(canceled()), SLOT(askThreadToStop()));
    if (_thread != nullptr)
    {
        connect(_thread, SIGNAL(finished()), SLOT(deleteLater()));
    }

    setWindowModality(Qt::WindowModal);
    setMinimumDuration(240);
    setValue(0);
}

void ProgressDialog::askThreadToStop()
{
    if (_thread != nullptr && _thread->isRunning())
    {
        _thread->requestInterruption();
    }
}
