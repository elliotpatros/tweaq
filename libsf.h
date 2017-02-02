#ifndef LIBSF_H
#define LIBSF_H

// application header
#include "tweaq.h"

// base class
#include <sndfile.hh>

// Qt classes
#include <QFileInfo>

class LibSF
{
public:
    static QString extension(const int format);
    static bool audioFileIsValid(const QString path);
    static QString durationAsString(const int sampleRate, const size_t nSamples);

private:
    static vector<QString> _validExtensions;

    // helpers
    static int formatCount();
    static bool formatExtension(const int format, SF_FORMAT_INFO* const info);
    static bool majorFormatExtension(const int format, SF_FORMAT_INFO* const info);
    static vector<QString> generateValidExtensions();
};

#endif // LIBSF_H
