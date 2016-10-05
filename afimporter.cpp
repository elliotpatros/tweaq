#include "afimporter.h"

AFImporter::AFImporter(QObject* parent) :
    AbstractFileImporter(parent)
{}

bool AFImporter::isFileValid(const QString& path) const
{
    return LibSF::isAudioFileValid(path);
}
