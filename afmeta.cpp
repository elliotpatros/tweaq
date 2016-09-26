#include "afmeta.h"

//==============================================================================
// constructor
//==============================================================================
AFMeta::AFMeta(const QString path) :
    TqFileInfo(path)
{
    SndfileHandle handle(absolutePath().toUtf8());
    _channels = handle.channels();
    _frames = handle.frames();
    _sampleRate = handle.samplerate();
}


//==============================================================================
// properties
//==============================================================================
t_uint AFMeta::channels(void) const
{
    return _channels;
}

t_uint AFMeta::frames(void) const
{
    return _frames;
}

t_uint AFMeta::sampleRate(void) const
{
    return _sampleRate;
}

QString AFMeta::nameWithExt(void) const
{
    return baseName() % QStringLiteral(".") % extension();
}

QString AFMeta::extension(void) const
{
    return LibSF::extension(absolutePath());
}

QString AFMeta::durationAsString(void) const
{
    return LibSF::durationAsString(sampleRate(), frames());
}
