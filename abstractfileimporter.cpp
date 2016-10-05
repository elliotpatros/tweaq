#include "abstractfileimporter.h"
t_uint AbstractFileImporter::_maxNumFilesICanImport = 65536; /* 2^16 */

//==============================================================================
// constructor & destructor
//==============================================================================
AbstractFileImporter::AbstractFileImporter(QObject* parent) :
    QObject(parent)
{
    _fileList = nullptr;
    _nFilesExamined = 0;
}

AbstractFileImporter::AbstractFileImporter(const AbstractFileImporter& other) :
    QObject(other.parent())
{
    _fileList = nullptr;
    _nFilesExamined = 0;
}


//==============================================================================
// gets
//==============================================================================
void AbstractFileImporter::import(const QList<QUrl> urls, QStringList* list)
{
    // drag and drop gives me urls... wtf is that?
    // they don't easily cast to QFileInfo either. :-(
    // make some strings out of them.

    // setup file list reference
    _fileList = list;
    if (_fileList == nullptr)
    {
        return;
    }

    // setup counting stuff
    _nFilesExamined = 0;

    // import each url
    const t_uint nUrls = urls.size();
    for (t_uint i = 0; i < nUrls; ++i)
    {
        if (!importValidFilesInPath(urls.at(i).toLocalFile()))
        {
            break;
        }
    }

    // that's it! remove duplicates
    _fileList->removeDuplicates();
}

bool AbstractFileImporter::importValidFilesInPath(const QString path)
{
    // try to import this file (or if it's a directory, everything in it).
    // return true to say that it's ok to keep importing more files, false otherwise.

    // is this path a directory? let's import it's insides!
    if (QFileInfo(path).isDir())
    {
        QDirIterator dit(path, QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
        while (dit.hasNext())
        {
            if (!appendToListIfValid(dit.next()))
            {
                return false;
            }
        }
    }
    else // must be a file of some sort. let's try importing it!
    {
        if (!appendToListIfValid(path))
        {
            return false;
        }
    }

    return true;
}

bool AbstractFileImporter::appendToListIfValid(const QString path)
{
    // this function should only get passed files. we check if they're valid.
    // we also check if our parent thread has asked us to stop, and tell anyone
    // who's listening how much work we've done so far.

    if (QThread::currentThread()->isInterruptionRequested())
    {
        return false;
    }

    emit progress(++_nFilesExamined);
    if (isFileValid(path))
    {
        _fileList->append(path);
    }

    if (_nFilesExamined >= _maxNumFilesICanImport)
    {
        return false;
    }

    return true;
}

t_uint AbstractFileImporter::numFilesInUrls(const QList<QUrl>* urls)
{
    // unfortunately, the only way i can think of to count the number of files
    // that need to be imported is to actually scan them all. ew...

    t_uint nFilesToExamine = 0;
    const t_uint nUrls = urls->size();
    for (t_uint i = 0; i < nUrls; ++i)
    {
        const QString path(urls->at(i).toLocalFile());
        if (QFileInfo(path).isDir())
        {
            QDirIterator dit(path, QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
            while (dit.hasNext())
            {
                if (QThread::currentThread()->isInterruptionRequested())
                {
                    return 0;
                }

                dit.next();
                ++nFilesToExamine;
            }
        }
        else
        {
            if (QThread::currentThread()->isInterruptionRequested())
            {
                return 0;
            }

            ++nFilesToExamine;
        }
    }

    return nFilesToExamine;
}
