#ifndef TQFILEINFO_H
#define TQFILEINFO_H

// application header
#include "m_tweaq.h"

// Qt classes
#include <QFileInfo>

class TqFileInfo
{
public:
    // constructors
    explicit TqFileInfo(const QString path = QString());
    virtual ~TqFileInfo(void) {}

    // gets
    virtual QString absolutePath(void) const;
    virtual QString parentDir(void) const;
    virtual QString nameWithExtension(void) const;
    virtual QString baseName(void) const;
    virtual QString extension(void) const;


private:
    QString _absolutePath;
};

#endif // TQFILEINFO_H
