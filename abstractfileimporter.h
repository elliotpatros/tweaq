#ifndef ABSTRACTFILEIMPORTER_H
#define ABSTRACTFILEIMPORTER_H

// application header
#include "m_tweaq.h"

// threads
#include <QThread>

// base class
#include <QObject>

// Qt classes
#include <QFileInfo>
#include <QUrl>
#include <QDirIterator>

class AbstractFileImporter : public QObject
{
    Q_OBJECT

public:
    // constructor & destructor
    AbstractFileImporter(void);
    AbstractFileImporter(const AbstractFileImporter&);

    // gets
    void import(const QList<QUrl> urls, QStringList* list);


signals:
    void progress(t_uint);


protected:
    virtual bool isFileValid(const QString& path) const = 0;


private:
    // gets
    bool importValidFilesInPath(const QString path);
    bool appendToListIfValid(const QString path);
    t_uint numFilesInUrls(const QList<QUrl>* urls);

    // how many files have we looked at since calling import(urls);
    t_uint _nFilesExamined;
    static t_uint _maxNumFilesICanImport;

    // non-owned
    QStringList* _fileList;

};

#endif // ABSTRACTFILEIMPORTER_H
