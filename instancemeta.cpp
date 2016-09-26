#include "instancemeta.h"

//==============================================================================
// constructor
//==============================================================================
InstanceMeta::InstanceMeta(void)
{
    // app meta
    QCoreApplication::setApplicationName(QStringLiteral("tweaq"));
    QCoreApplication::setApplicationVersion(QStringLiteral("0.0.1"));
}


//==============================================================================
// app meta
//==============================================================================
QString InstanceMeta::appNameAndVersion(void) const
{
    return QCoreApplication::applicationName()
            % QStringLiteral(" ")
            % QCoreApplication::applicationVersion();
}


//==============================================================================
// dimensions
//==============================================================================
QSize InstanceMeta::minWindowSize(void) const
{
    return QSize(800, 312);
}

QSize InstanceMeta::maxWindowSize(void) const
{
    QSize size = QApplication::desktop()->availableGeometry().size();
    size.rheight() /= 2;

    return size;
}

QSize InstanceMeta::preferredWindowSize(void) const
{
    return maxWindowSize() * 0.75;
}


//==============================================================================
// directories
//==============================================================================
QString InstanceMeta::externalDirectorySearchPath(void) const
{
    // return the folder where one might expect to find external DSP libraries
    // i should find  better way to do this...
    QDir dir (QCoreApplication::applicationFilePath());
    dir.cd(QStringLiteral("../../../../../tweaq/Externs"));

    return dir.absolutePath();
}
