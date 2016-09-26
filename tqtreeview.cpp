#include "tqtreeview.h"

//==============================================================================
// constructor
//==============================================================================
TqTreeView::TqTreeView(QWidget* parent) :
    QTreeView(parent)
{
    // setup look and feel
    setUniformRowHeights(true);
    setAlternatingRowColors(true);
    setIndentation(0);
    setRootIsDecorated(false);
    setItemsExpandable(false);
    setExpandsOnDoubleClick(false);

    // setup model
    setSelectionMode(QAbstractItemView::ExtendedSelection);

    // setup drag and drop
    setAcceptDrops(true);
}


//==============================================================================
// drag and drop handling
//==============================================================================
void TqTreeView::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
}

void TqTreeView::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
}

void TqTreeView::dropEvent(QDropEvent* event)
{
    if (event->mimeData()->hasUrls())
    {
        emit droppedAudioFileUrls(event->mimeData()->urls());
    }
}


//==============================================================================
// gets
//==============================================================================
QModelIndexList TqTreeView::selectedRows(void) const
{
    return selectedIndexes();
}
