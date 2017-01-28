#include "treeviewaudiofile.h"

// constructor
TreeViewAudioFile::TreeViewAudioFile(QWidget* const parent) :
    QTreeView(parent),
    _hint(this)
{
    // setup connection rules
    disconnect();

    // setup look and feel
    setUniformRowHeights(true);
    setIndentation(0);
    setRootIsDecorated(false);
    setItemsExpandable(false);
    setExpandsOnDoubleClick(false);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    sortByColumn(1);

    // show hint to user
    _hint.show(this);

    // setup drag and drop
    setAcceptDrops(true);
}


// gets
QModelIndexList TreeViewAudioFile::selected() const
{
    return selectedIndexes();
}


// events
void TreeViewAudioFile::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
}

void TreeViewAudioFile::dragMoveEvent(QDragMoveEvent* event)
{
    if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
}

void TreeViewAudioFile::dropEvent(QDropEvent* event)
{
    if (event->mimeData()->hasUrls())
    {
        _hint.hide();
        emit droppedUrls(event->mimeData()->urls());
    }
}


StartupHint::StartupHint(QWidget* const parent) :
    _layout(new QVBoxLayout(parent)),
    _label(new QLabel("drop files and folders", parent))
{
    _layout->addWidget(_label, 0, Qt::AlignCenter);

    _label->setAlignment(Qt::AlignCenter);
    _label->setStyleSheet(QStringLiteral("font:normal; color:rgba(0, 0, 0, 15%); font-size:32px;"));
    _label->setEnabled(false);
    _label->setAttribute(Qt::WA_TransparentForMouseEvents);
}

void StartupHint::show(QWidget* widget)
{
    widget->setLayout(_layout);
}

void StartupHint::hide()
{
    delete _label;
    _label = nullptr;
}
