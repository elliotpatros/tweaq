#ifndef TWEAQ_LIBRARY_H
#define TWEAQ_LIBRARY_H

// application header
#include "tweaq.h"

// base class
#include <QLibrary>

// tweaq classes
#include "parameterhandle.h"
#include "af_item.h"

// Qt classes
#include <QFileInfo>

class ExternalInterface
{
public:
    explicit ExternalInterface(const QString fileName = QString());

    // sets
    vector<ParameterHandle> setup() const;
    void* handleInput(const vector<QString> responses) const;
    ExternalProcess process() const;
//    void  process(const vector<AF_Item> rootItem, const QString dest, void* args) const;

    // gets
    QString libraryName() const;
    QString readableName() const;


private:
    QString _absolutePath;

    // gets
    QFunctionPointer functionPointer(const QString functionName) const;
    ExternalSetup functionSetup() const;
    ExternalHandleInput functionHandleInput() const;
};

#endif // TWEAQ_LIBRARY_H
