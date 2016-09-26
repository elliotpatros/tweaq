#ifndef LIBIMPORTER_H
#define LIBIMPORTER_H

// application header
#include "m_tweaq.h"

// base class
#include "abstractfileimporter.h"
#include <QLibrary>

class LibImporter : public AbstractFileImporter
{
public:
    LibImporter(void);


protected:
    bool isFileValid(const QString& path) const;
};

#endif // LIBIMPORTER_H
