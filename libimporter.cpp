#include "libimporter.h"

LibImporter::LibImporter(void) :
    AbstractFileImporter()
{}

bool LibImporter::isFileValid(const QString& path) const
{
    return QLibrary::isLibrary(path);
}
