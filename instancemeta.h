#ifndef INSTANCEMETA_H
#define INSTANCEMETA_H

// application header
#include "m_tweaq.h"

// Qt utility
#include <QApplication>     // for desktop()->size
#include <QDesktopWidget>   // for desktop()
#include <QCoreApplication> // for meta that's already stored

// Qt data structures
#include <QDir>
#include <QSize>

class InstanceMeta
{
public:
    // constructor
    InstanceMeta(void);

    // app meta
    QString appNameAndVersion(void) const;

    // dimensions
    QSize minWindowSize(void) const;
    QSize maxWindowSize(void) const;
    QSize preferredWindowSize(void) const;

    // directories
    QString externalDirectorySearchPath(void) const;

    // actions
    enum {
        ComboBoxTitleIndex = 0,
        ComboBoxSeparator,
        ComboBoxTitleItems,
        ComboBoxRemoveAll = ComboBoxTitleItems,
        ComboBoxRemoveSelected,
        ComboBoxRemoveFinished
    };
};

#endif // INSTANCEMETA_H
