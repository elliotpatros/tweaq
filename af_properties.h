#ifndef AFPROPERTIES_H
#define AFPROPERTIES_H

// application header
#include "tweaq.h"

// libsndfile stuff
#include "libsf.h"

// Qt classes
#include <QVariant>

struct AF_Properties
{
    // constructors
    explicit AF_Properties(const QString path);
    explicit AF_Properties(const vector<QVariant> data);
    explicit AF_Properties(const AF_Properties& other);
    AF_Properties& operator=(const AF_Properties& other);

    // gets
    static bool indexIsValid(const int index);
    QString absolutePath() const;
    QString baseName() const;
    QString extension() const;
    int channels() const;
    QString duration() const;
    int sampleRate() const;
    QString parentDirectory() const;
    QString uniquePath(const QString folder) const;
    QString buildPath(const QString folder, const int nthCopy = 0) const;

    // properties
    enum PropertyIndex
    {
        AbsolutePath = 0,
        BaseName,
        Extension,
        Channels,
        Duration,
        SampleRate,
        ParentDirectory,
        NumProperties
    };


protected:
    vector<QVariant> _data;

    // helpers
    static vector<QVariant> audioFileProperties();
};

enum class DSP_Status
{
    Succeeded = 0,
    Failed,
    Unprocessed
};


#endif // AFPROPERTIES_H
