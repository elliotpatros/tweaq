#ifndef TREEVIEWAUDIOFILE_H
#define TREEVIEWAUDIOFILE_H

// application header
#include "tweaq.h"

// base class
#include <QTreeView>

// Qt classes
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>

class TreeViewAudioFile : public QTreeView
{
    Q_OBJECT

public:
    explicit TreeViewAudioFile(QWidget* const parent = nullptr);

    // gets
    QModelIndexList selected() const;


signals:
    void droppedUrls(const QList<QUrl>);


protected:
    // drag and drop
    void dragEnterEvent(QDragEnterEvent* event);
    void dragMoveEvent(QDragMoveEvent* event);
    void dropEvent(QDropEvent* event);
};

#endif // TREEVIEWAUDIOFILE_H
