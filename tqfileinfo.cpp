#include "tqfileinfo.h"

//==============================================================================
// constructors
//==============================================================================
TqFileInfo::TqFileInfo(void)
{
    _absolutePath.clear();
}

TqFileInfo::TqFileInfo(const QString path)
{
    _absolutePath = QFileInfo(path).canonicalFilePath();
}


//==============================================================================
// gets
//==============================================================================
QString TqFileInfo::absolutePath(void) const
{
    return _absolutePath;
}

QString TqFileInfo::parentDir(void) const
{
    return QFileInfo(_absolutePath).canonicalPath();
}

QString TqFileInfo::nameWithExtension(void) const
{
    return baseName() % extension();
}

QString TqFileInfo::baseName(void) const
{
    return QFileInfo(_absolutePath).completeBaseName();
}

QString TqFileInfo::extension(void) const
{
    return QFileInfo(_absolutePath).suffix();
}
