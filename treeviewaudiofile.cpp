#include "treeviewaudiofile.h"

// constructor
TreeViewAudioFile::TreeViewAudioFile(QWidget* const parent) :
    QTreeView(parent)
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
        emit droppedUrls(event->mimeData()->urls());
    }
}
