#include "treeviewaudiofile.h"

/*
 * constructor
 */
TreeViewAudioFile::TreeViewAudioFile(QWidget* const parent) :
    QTreeView(parent)
{
    // setup connection rules
    this->disconnect();

    // setup look and feel
    setUniformRowHeights(true);
    setAlternatingRowColors(true);
    setIndentation(0);
    setRootIsDecorated(false);
    setItemsExpandable(false);
    setExpandsOnDoubleClick(false);
    setSelectionMode(QAbstractItemView::ExtendedSelection);

    // setup drag and drop
    setAcceptDrops(true);
}


// gets
QModelIndexList TreeViewAudioFile::selected() const
{
    return selectedIndexes();
}


// drag and drop
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
