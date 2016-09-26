#ifndef LIBSF_H
#define LIBSF_H

// application header
#include "m_tweaq.h"

// Qt classes
#include <QFileInfo>
#include <QVariant>

class LibSF
{
public:
    // gets
    static bool isAudioFileValid(const QString path);
    static QString extension(const QString path);
    static QString durationAsString(const t_uint sampleRate, const t_uint nSamples);


private:
    // properties
    static QStringList _validExtensions;
    static QStringList validExtensions(void);
};

#endif // LIBSF_H
