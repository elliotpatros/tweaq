#ifndef BACKGROUNDWORKER_H
#define BACKGROUNDWORKER_H

// application class
#include "tweaq.h"

// base class
#include <QObject>

// Qt classes
#include <QThread>

// tweaq classes
#include "fileimporter.h"
#include "af_item.h"

class BackgroundWorker : public QObject
{
    Q_OBJECT

public:
    explicit BackgroundWorker(const QList<QUrl> urls = QList<QUrl>(),
                              const AF_Item* root = nullptr,
                              QObject* parent = nullptr);


public slots:
    void importUrlsAsAudioFiles();


signals:
    void fileListReady(const FileList fileList);
    void finished();


private:
    const QList<QUrl> _urls;
    const AF_Item* _root; // non-owned
};

#endif // BACKGROUNDWORKER_H
