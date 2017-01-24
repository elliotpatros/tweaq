#ifndef TWEAQLIBRARYMANAGER_H
#define TWEAQLIBRARYMANAGER_H

// application header
#include "tweaq.h"

// base class
//#include <QObject>

// tweaq classes
#include "externalinterface.h"
#include "fileimporter.h"

class ExternalInterfaceManager
{
public:
    explicit ExternalInterfaceManager(const QString searchPath = QString());

    // gets
    bool isValid(const int externalIndex) const;
    QStringList processNames() const;
    ExternalInterface* get(const int externalIndex);


private:
    vector<ExternalInterface> _externals;
};

#endif // TWEAQLIBRARYMANAGER_H
