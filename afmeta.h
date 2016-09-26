#ifndef AFMETA_H
#define AFMETA_H

// application header
#include "m_tweaq.h"

// base classes
#include "tqfileinfo.h"
#include "libsf.h"

class AFMeta : public TqFileInfo
{
public:
    explicit AFMeta(const QString path);

    // properties
    t_uint  channels(void) const;
    t_uint  frames(void) const;
    t_uint  sampleRate(void) const;
    QString extension(void) const;
    QString nameWithExt(void) const;
    QString durationAsString(void) const;


private:
    t_uint _channels, _frames, _sampleRate;
};

#endif // AFMETA_H
