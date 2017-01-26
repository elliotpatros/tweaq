#include "backgroundworker.h"

//#include <functional> // deleteme

BackgroundWorker::BackgroundWorker(const QList<QUrl> urls, QObject* parent) :
    QObject(parent),
    _audioFiles(model),
    _ui(ui),
    _urls(urls)
{
    // don't allocate any new resources here (w/ new or similar)
    // or they'll belong to the calling thread (probably gui thread)

    disconnect();
    connect(this, SIGNAL(finished()), SLOT(deleteLater()));
}

BackgroundWorker::~BackgroundWorker()
{

}

void BackgroundWorker::importAudioFiles()
{
    if (_ui == nullptr ||
        _ui->treeView == nullptr ||
        _audioFiles == nullptr) return;

    _ui->treeView->setSortingEnabled(false);
    _audioFiles->importAudioFiles(_urls);
    _ui->treeView->setSortingEnabled(true);
    _ui->treeView->clearSelection();
}

void BackgroundWorker::process()
{
    // allocate new resources here instead
    for (int i = 0; i < 5; i++)
    {
        QThread::msleep(250);
        qDebug() << "mmmm hmmm";
    }

    emit finished();
}
