#include "backgroundworker.h"

// base class constructor
BackgroundWorker::BackgroundWorker(QObject* parent) :
    QObject(parent)
{
    // don't allocate any new resources here (w/ new or similar)
    // or they'll belong to the calling thread
    disconnect();
    connect(this, SIGNAL(finished()), SLOT(deleteLater()));
}


// import audio files
ImportWorker::ImportWorker(const QList<QUrl> urls,
                           const AF_Item* root,
                           QObject* parent) :
    BackgroundWorker(parent),
    _urls(urls),
    _root(root) {}

void ImportWorker::importUrlsAsAudioFiles()
{
    // import valid files to a list
    FileList list;
    if (AudioFileImporter().importUrls(_urls, list) == FileImporter::Success)
    {
        // delete any duplicate items
        if (_root != nullptr)
        {
            _root->filterDuplicates(list);
        }

        emit fileListReady(list);
    }

    emit finished();
}


// process audio files
DSP_Worker::DSP_Worker(ExternalInterface* const external,
                       const QString destination,
                       const vector<QString> userInput,
                       AF_Item* root,
                       QObject* parent) :
    BackgroundWorker(parent),
    _external(external),
    _destination(destination),
    _userInput(userInput),
    _root(root) {}


void DSP_Worker::processAudioFiles()
{
    void* arguments = _external->handleInput(_userInput);
    _root->processAudioFiles(_external->process(), _destination, arguments);
    free(arguments); // by standard, ok to free nullptr
    emit finished();
}
