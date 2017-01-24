#ifndef MAIN_WINDOW_HELPER_H
#define MAIN_WINDOW_HELPER_H

// application header
#include "tweaq.h"

// Qt classes
#include <QDesktopWidget>   // for desktop()
#include <QApplication>     // for desktop()->size
#include <QDir>

class AppMeta
{
public:
    static QString windowName()
    {
        return "tweaq v0.0";
    }

    static QSize preferredWindowSize()
    {
        return QSize(800, 300);
    }

    static QString pathToBundledExternals()
    {
        const QDir folder(QCoreApplication::applicationDirPath() % "/../Resources/extern");
        return folder.absolutePath();
    }

    static void centerWidgetOnScreen(QWidget* const widget)
    {
        widget->move(QApplication::desktop()->screen()->rect().center() - widget->rect().center());
    }
};

#endif // MAIN_WINDOW_HELPER_H defined
