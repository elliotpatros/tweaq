#include "fileimporter.h"
#include <QThread>

// gets
FileImporter::Result
FileImporter::importUrls(const QList<QUrl> urls, FileList &fileList, const bool recursive) const
{
    for (const QUrl url : urls)
    {
        if (importPath(url.toLocalFile(), fileList, recursive) == Canceled)
        {
            return Canceled;
        }
    }

    return Success;
}

FileImporter::Result
FileImporter::importPath(const QString path, FileList& fileList, const bool recursive) const
{
    const auto searchType = recursive ? QDirIterator::Subdirectories : QDirIterator::NoIteratorFlags;

    if (QFileInfo(path).isDir())
    {
        QDirIterator dit(path, QDir::Files | QDir::NoDotAndDotDot, searchType);
        while (dit.hasNext())
        {
            if (appendIfValid(dit.next(), fileList) == Canceled)
            {
                return Canceled;
            }
        }
    }
    else
    {
        if (appendIfValid(path, fileList) == Canceled)
        {
            return Canceled;
        }
    }

    return Success;
}

FileImporter::Result
FileImporter::appendIfValid(const QString path, FileList& fileList) const
{
    if (QThread::currentThread()->isInterruptionRequested())
    {
        return Canceled;
    }

    if (fileIsValid(path))
    {
        fileList.emplace_back(path);
    }

    return Success;
}
