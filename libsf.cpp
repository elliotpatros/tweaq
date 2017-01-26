#include "libsf.h"
#include <chrono>

// private static members
vector<QString> LibSF::_validExtensions = LibSF::generateValidExtensions();

// public static members
QString LibSF::extension(const int format)
{
    SF_FORMAT_INFO info;
    if (formatExtension(format, &info))
    {
        return QString(info.extension);
    }

    return QStringLiteral("unknown");
}

bool LibSF::audioFileIsValid(const QString path)
{
    // is the path safe for libsndfile to try to open?
    const QFileInfo file(path);
    if (file.isFile() &&
        file.isReadable() &&
        in_sorted(file.suffix(), _validExtensions))
    {   // can libsndfile handle it?
        SF_INFO sfinfo;
        memset(&sfinfo, 0, sizeof(SF_INFO));
        SNDFILE* sf = sf_open(path.toUtf8(), SFM_READ, &sfinfo);

        const bool opened_ok = sf != nullptr;
        const bool closed_ok = sf_close(sf) == SF_ERR_NO_ERROR;
        return opened_ok && closed_ok;
    }

    return false;
}

QString LibSF::durationAsString(const int sampleRate, const size_t nSamples)
{
    using std::chrono::milliseconds;
    using std::chrono::minutes;
    using std::chrono::duration_cast;

    const milliseconds ms (nSamples * 1000 / sampleRate);
    const minutes      min(duration_cast<minutes>(ms));
    const double       sec((ms - min).count() * 0.001);

    return QString("%1m %2s")
            .arg(min.count(), 7, 'g', -1, QChar())
            .arg(sec, 4, 'f', 1, '0');
}


// helpers
int LibSF::formatCount()
{
    int n;
    sf_command(NULL, SFC_GET_FORMAT_MAJOR_COUNT, &n, sizeof(int));
    return n;
}

bool LibSF::formatExtension(const int format, SF_FORMAT_INFO* const info)
{
    info->format = format;
    return sf_command(NULL, SFC_GET_FORMAT_INFO, info, sizeof(SF_FORMAT_INFO)) == SF_ERR_NO_ERROR;
}

bool LibSF::majorFormatExtension(const int format, SF_FORMAT_INFO* const info)
{
    info->format = format;
    return sf_command(NULL, SFC_GET_FORMAT_MAJOR, info, sizeof(SF_FORMAT_INFO)) == SF_ERR_NO_ERROR;
}

vector<QString> LibSF::generateValidExtensions()
{
    int nMajorFormats(formatCount());
    vector<QString> list(nMajorFormats);

    std::generate(list.begin(), list.end(), [&nMajorFormats]
    {
        SF_FORMAT_INFO info;
        if (!majorFormatExtension(--nMajorFormats, &info)) return "";
        return info.extension;
    });

    list.emplace_back(QStringLiteral("aif"));
    std::sort(list.begin(), list.end());
    list.erase(std::unique(list.begin(), list.end()), list.end());

    return list;
}
