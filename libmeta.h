#ifndef LIBMETA_H
#define LIBMETA_H

// application header
#include "m_tweaq.h"

// Qt library
#include <QLibrary>

// base class
#include "tqfileinfo.h"

class LibMeta : public TqFileInfo
{
public:
    // constructors & destructor
    LibMeta(void);
    explicit LibMeta(const QString path);
    virtual ~LibMeta(void) {}

    // gets
    QString libFileName(void) const;
    QString libName(void) const;
};

#endif // LIBMETA_H
