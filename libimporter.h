#ifndef LIBIMPORTER_H
#define LIBIMPORTER_H

// application header
#include "m_tweaq.h"

// base class
#include "abstractfileimporter.h"
#include <QLibrary>

class LibImporter : public AbstractFileImporter
{
    Q_OBJECT

public:
    explicit LibImporter(QObject* parent = Q_NULLPTR);


protected:
    bool isFileValid(const QString& path) const;
};

#endif // LIBIMPORTER_H
