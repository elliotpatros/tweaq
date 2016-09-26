#include "tqlineedit.h"

//==============================================================================
// static members
//==============================================================================
QString TqLineEdit::_validStyleSheet = QStringLiteral("color:rgb(0, 0, 0)");
QString TqLineEdit::_invalidStyleSheet = QStringLiteral("color:rgba(255, 0, 0, 200)");


//==============================================================================
// constructor
//==============================================================================
TqLineEdit::TqLineEdit(QWidget *parent) :
    QLineEdit(parent)
{
    // set look and feel
    setPlaceholderText(QStringLiteral("choose a folder to export audio files..."));

    // set directory
    setText(defaultPath());
    validateTextForUser();

    // drag and drop
    setAcceptDrops(true);

    // setup connections
    this->disconnect();
    connect(this, SIGNAL(textChanged(QString)), this, SLOT(validateTextForUser()));
}


//==============================================================================
// gets
//==============================================================================
QString TqLineEdit::defaultPath(void) const
{
    return QDir::homePath();
}

bool TqLineEdit::isCurrentPathValid(void)
{
    _currentPath.setPath(text());
    return _currentPath.exists();
}

QString TqLineEdit::currentValidFileLocation(void)
{
    return isCurrentPathValid() ? _currentPath.canonicalPath() : defaultPath();
}


//==============================================================================
// drag and drop handling
//==============================================================================
void TqLineEdit::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
}

void TqLineEdit::dragMoveEvent(QDragMoveEvent* event)
{
    if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
}

void TqLineEdit::dropEvent(QDropEvent* event)
{
    if (event->mimeData()->hasUrls())
    {
        // set text to folder of first url dropped
        const QFileInfo info (event->mimeData()->urls().constFirst().toLocalFile());
        setText(info.isDir() ? info.canonicalFilePath() : info.canonicalPath());
        validateTextForUser();
    }
}


//==============================================================================
// directory
//==============================================================================
void TqLineEdit::validateTextForUser(void)
{
    // let folks know right away whether the folder they see written here exists.
    setStyleSheet((isCurrentPathValid() || text().isEmpty())
                  ? _validStyleSheet
                  : _invalidStyleSheet);
}
