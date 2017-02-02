#include "af_properties.h"

/*
 * directories that get passed to this contructor
 * are guarenteed to be valid audio files
 * by the importer.
 */
// todo: exception handling for memory w/ vectors
AF_Properties::AF_Properties(const QString path) :
    _data(NumProperties)
{
    const QFileInfo fileInfo(path);

    _data[AbsolutePath] = fileInfo.canonicalFilePath();
    SndfileHandle handle(absolutePath().toUtf8());

    _data[Channels] = handle.channels();
    _data[SampleRate] = handle.samplerate();
    _data[BaseName] = fileInfo.completeBaseName();
    _data[Extension] = LibSF::extension(handle.format());
    _data[Duration] = LibSF::durationAsString(sampleRate(), handle.frames());
    _data[ParentDirectory] = fileInfo.canonicalPath();
}

AF_Properties::AF_Properties(const vector<QVariant> data)
{
    _data = std::move(data);
}

AF_Properties::AF_Properties(const AF_Properties& other)
{
    _data = std::move(other._data);
}

AF_Properties& AF_Properties::operator=(const AF_Properties& other)
{
    _data = std::move(other._data);
    return *this;
}

// gets
bool AF_Properties::indexIsValid(const int index)
{
    return 0 <= index && index < NumProperties;
}

QString AF_Properties::absolutePath() const
{
    return _data[AbsolutePath].toString();
}

QString AF_Properties::baseName() const
{
    return _data[BaseName].toString();
}

QString AF_Properties::extension() const
{
    return _data[Extension].toString();
}

int AF_Properties::channels() const
{
    return _data[Channels].toInt();
}

QString AF_Properties::duration() const
{
    return _data[Duration].toString();
}

int AF_Properties::sampleRate() const
{
    return _data[SampleRate].toInt();
}

QString AF_Properties::parentDirectory() const
{
    return _data[ParentDirectory].toString();
}

QString AF_Properties::buildPath(const QString folder, const int nthCopy) const
{
    return (nthCopy > 0)
        ? QString("%1/%2 (%3).%4").arg(folder, baseName(), QString::number(nthCopy), extension())
        : QString("%1/%2.%3").arg(folder, baseName(), extension());
}

QString AF_Properties::uniquePath(const QString folder) const
{
    int nCopies = 0;
    QString path = buildPath(folder);

    while (QFileInfo(path).exists()) path = buildPath(folder, ++nCopies);

    return path;
}

QString AF_Properties::pathAtDirectory(const QString folder) const
{
    return QString("%1/%2.%3").arg(folder, baseName(), extension());
}

vector<QVariant> AF_Properties::audioFileProperties()
{
    return
    {
        QStringLiteral("absolute path"),
        QStringLiteral("name"),
        QStringLiteral("type"),
        QStringLiteral("channels"),
        QStringLiteral("duration"),
        QStringLiteral("sample rate"),
        QStringLiteral("directory")
    };
}
