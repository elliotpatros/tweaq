#ifndef AFIMPORTER_H
#define AFIMPORTER_H

// application header
#include "m_tweaq.h"

// base class
#include "abstractfileimporter.h"
#include "libsf.h"

class AFImporter : public AbstractFileImporter
{
    Q_OBJECT

public:
    explicit AFImporter(QObject* parent = Q_NULLPTR);


protected:
    bool isFileValid(const QString& path) const;
};

#endif // AFIMPORTER_H
