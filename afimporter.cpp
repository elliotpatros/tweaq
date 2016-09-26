#include "afimporter.h"

AFImporter::AFImporter() :
    AbstractFileImporter()
{}

bool AFImporter::isFileValid(const QString& path) const
{
    return LibSF::isAudioFileValid(path);
}
