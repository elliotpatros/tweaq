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
#include "af_model.h"
#include "externalinterface.h"

class BackgroundWorker : public QObject
{
    Q_OBJECT

public:
    explicit BackgroundWorker(QObject* parent = nullptr);


signals:
    void finished();
};


class ImportWorker : public BackgroundWorker
{
    Q_OBJECT

public:
    explicit ImportWorker(const QList<QUrl> urls = QList<QUrl>(),
                          const AF_Item* root = nullptr,
                          QObject* parent = nullptr);


public slots:
    void importUrlsAsAudioFiles();


signals:
    void fileListReady(const FileList fileList);


private:
    const QList<QUrl> _urls;
    const AF_Item* _root; // non-owned
};


class DSP_Worker : public BackgroundWorker
{
    Q_OBJECT

public:
    explicit DSP_Worker(const ExternalInterface* const external,
                        const QString destination,
                        const vector<QString> userInput,
                        AF_Model* model,
                        QObject* parent = nullptr);


public slots:
    void processAudioFiles();


private:
    const ExternalInterface* const _external;
    const QString _destination;
    const vector<QString> _userInput;
    AF_Model* _model;
};

#endif // BACKGROUNDWORKER_H
