#include "backgroundworker.h"

BackgroundWorker::BackgroundWorker(const QList<QUrl> urls, const AF_Item* root, QObject* parent) :
    QObject(parent),
    _urls(urls),
    _root(root)

{
    // don't allocate any new resources here (w/ new or similar)
    // or they'll belong to the calling thread (probably gui thread)

    disconnect();
    connect(this, SIGNAL(finished()), SLOT(deleteLater()));
}

void BackgroundWorker::importUrlsAsAudioFiles()
{
    // import valid files to a list
    FileList list;
    if (AudioFileImporter().importUrls(_urls, list) == FileImporter::Canceled)
    {
        return;
    }

    // delete any duplicate items
    if (_root != nullptr)
    {
        const auto alreadyImported = [this](const QString path)
        {
            return _root->hasChildPath(path);
        };

        erase_if(list, alreadyImported);
    }

    emit fileListReady(list);
    emit finished();
}
