#include "lineeditexportto.h"

const QString LineEditExportTo::_defaultPath(QDir::homePath());

// constructor
LineEditExportTo::LineEditExportTo(QWidget *parent) :
    QLineEdit(parent)
{
    // setup connection rules
    disconnect();
    connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(validateTextForUser()));

    // set directory
    setText(_defaultPath);

    // set look and feel
    setPlaceholderText("choose a folder to export audio files...");

    // drag and drop
    setAcceptDrops(true);
}

// gets
QString LineEditExportTo::path() const
{
    return currentPathIsValid() ? text() : _defaultPath;
}

bool LineEditExportTo::currentPathIsValid() const
{
    return _currentPath.isDir() && _currentPath.isWritable();
}

// drag and drop
void LineEditExportTo::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
}

void LineEditExportTo::dragMoveEvent(QDragMoveEvent* event)
{
    if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
}

void LineEditExportTo::dropEvent(QDropEvent* event)
{
    if (event->mimeData()->hasUrls())
    {
        const QFileInfo info(event->mimeData()->urls().constFirst().toLocalFile());
        setText(info.isDir() ? info.canonicalFilePath() : info.canonicalPath());
    }
}

// export to directory
void LineEditExportTo::validateTextForUser()
{
    _currentPath.setFile(text());

    const bool pathIsValidOrEmpty = currentPathIsValid() || text().isEmpty();
    setStyleSheet(pathIsValidOrEmpty
                ? QStringLiteral("font:normal; color:black;")
                : QStringLiteral("font:italic; color:gray;"));
}
