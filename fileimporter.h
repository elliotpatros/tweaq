#ifndef FILEIMPORTER_H
#define FILEIMPORTER_H

// application header
#include "tweaq.h"

// Qt classes
#include <QFileInfo>
#include <QUrl>
#include <QDirIterator>
#include <QLibrary>

// tweaq classes
#include "libsf.h"

class FileImporter
{
public:
    enum Result {Success, Canceled};

    // gets
    Result importUrls(const QList<QUrl> urls, FileList& fileList, const bool recursive = true) const;
    Result importPath(const QString path, FileList& fileList, const bool recursive = true) const;


protected:
    virtual bool fileIsValid(const QString path) const = 0;


private:
    Result appendIfValid(const QString path, FileList& fileList) const;
};


class AudioFileImporter : public FileImporter
{
protected:
    bool fileIsValid(const QString path) const
    {
        return LibSF::audioFileIsValid(path);
    }
};


class ExternFileImporter : public FileImporter
{
protected:
    bool fileIsValid(const QString path) const
    {
        return QLibrary::isLibrary(path);
    }
};

#endif // FILEIMPORTER_H
