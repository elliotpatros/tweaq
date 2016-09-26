#include "libmeta.h"

//==============================================================================
// constructors
//==============================================================================
LibMeta::LibMeta(void) :
    TqFileInfo()
{}

LibMeta::LibMeta(const QString path) :
    TqFileInfo(path)
{}


//==============================================================================
// gets
//==============================================================================
QString LibMeta::libFileName(void) const
{
    return baseName().split(QStringLiteral("lib")).at(1);
}

QString LibMeta::libName(void) const
{
    return libFileName().replace(QStringLiteral("_"), QStringLiteral(" "));
}
