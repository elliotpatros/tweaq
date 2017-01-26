#ifndef BACKGROUNDWORKER_H
#define BACKGROUNDWORKER_H

// application class
#include "tweaq.h"

// base class
#include <QObject>

// Qt classes
#include <QThread>
#include "ui_mainwindow.h"

// tweaq classes
#include "af_model.h"

class BackgroundWorker : public QObject
{
    Q_OBJECT

public:
    explicit BackgroundWorker(const QList<QUrl> urls = QList<QUrl>(),
                              QObject* parent = nullptr);
    ~BackgroundWorker();


public slots:
    void process();
    void importAudioFiles();


signals:
    void finished();
    void error(QString errorMessage);


private:
    const QList<QUrl> _urls;
};

#endif // BACKGROUNDWORKER_H
