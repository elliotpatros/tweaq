#include "fileimporter.h"

// gets
void FileImporter::importUrls(const QList<QUrl> urls, FileList &fileList, const bool recursive) const
{
    for (const QUrl url : urls)
    {
        importPath(url.toLocalFile(), fileList, recursive);
    }
}

void FileImporter::importPath(const QString path,
                              FileList& fileList,
                              const bool recursive) const
{
    const auto searchType = recursive ? QDirIterator::Subdirectories : QDirIterator::NoIteratorFlags;

    if (QFileInfo(path).isDir())
    {
        QDirIterator dit(path, QDir::Files | QDir::NoDotAndDotDot, searchType);
        while (dit.hasNext())
        {
            appendIfValid(dit.next(), fileList);
        }
    }
    else
    {
        appendIfValid(path, fileList);
    }
}

void FileImporter::appendIfValid(const QString path, FileList& fileList) const
{
    if (fileIsValid(path))
    {
        fileList.emplace_back(path);
    }
}
