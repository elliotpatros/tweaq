#include "libimporter.h"

LibImporter::LibImporter(QObject* parent) :
    AbstractFileImporter(parent)
{}

bool LibImporter::isFileValid(const QString& path) const
{
    return QLibrary::isLibrary(path);
}
