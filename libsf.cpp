#include "libsf.h"

//==============================================================================
// static members
//==============================================================================
QStringList LibSF::_validExtensions = LibSF::validExtensions();


//==============================================================================
// gets
//==============================================================================
bool LibSF::isAudioFileValid(const QString path)
{
    const QFileInfo fileInfo(path);

    // is this file a directory?
    if (fileInfo.isDir())
    {
        return false;
    }

    // does this file have a soundfile-y extension?
    if (!_validExtensions.contains(fileInfo.suffix()))
    {
        return false;
    }

    // can we open this file with libsndfile?
    SF_INFO sfinfo;
    memset(&sfinfo, 0, sizeof(sfinfo));

    SNDFILE* sf = sf_open(path.toUtf8(), SFM_READ, &sfinfo);
    const bool isValid = sf != nullptr;
    sf_close(sf);

    return isValid;
}

QString LibSF::extension(const QString path)
{
    SF_INFO sfinfo;
    memset(&sfinfo, 0, sizeof(sfinfo));

    SNDFILE* sf = sf_open(path.toUtf8(), SFM_READ, &sfinfo);

    SF_FORMAT_INFO info;
    info.format = sfinfo.format;
    const t_int success = sf_command(NULL, SFC_GET_FORMAT_INFO, &info, sizeof(info));
    sf_close(sf);

    return QString((success == 0) ? info.extension : QStringLiteral("unknown"));
}

QString LibSF::durationAsString(const t_uint sampleRate, const t_uint nSamples)
{
    // this is hilarious.
    // the tree view sorts data alphabetically, so make sure to front-pad the
    // return value with, like, just a bunch of empty characters. at least
    // that way, no one will think that i think that a 7 minute audio file is
    // longer than one that's 42 minutes.
    
    // cast time info to t_float
    const t_float sr = static_cast<t_float>(sampleRate);
    t_float ns = static_cast<t_float>(nSamples);

    // conversion scalars
    const t_float minutesToSamples = sr * 60.0;
    const t_float samplesToHours = 1.0 / (minutesToSamples * 60.0);
    const t_float samplesToMinutes = samplesToHours * 60.0;

    // find how many minutes
    const t_float minutes = floor(ns * samplesToMinutes);

    // find how many seconds
    ns -= minutes * minutesToSamples;
    const t_float seconds = ns * samplesToMinutes * 60.0;

    // format string (mm : ss.dd)
    return QString("%1:%2").arg(minutes, 9, 'g', -1, QChar()).arg(seconds, 5, 'f', 2, '0');
}


//==============================================================================
// properties
//==============================================================================
QStringList LibSF::validExtensions(void)
{
    QStringList list;
    SF_FORMAT_INFO info;
    SF_INFO sfinfo;
    int nMajor;

    memset(&sfinfo, 0, sizeof(sfinfo));

    sf_command(NULL, SFC_GET_FORMAT_MAJOR_COUNT, &nMajor, sizeof(int));
    sfinfo.channels = 1;
    for (int m = 0; m < nMajor; ++m)
    {
        info.format = m;
        sf_command(NULL, SFC_GET_FORMAT_MAJOR, &info, sizeof(info));
        list.append(info.extension);
    }

    list.append(QStringLiteral("aif"));
    return list;
}
