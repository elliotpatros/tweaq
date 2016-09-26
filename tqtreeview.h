#ifndef TQTREEVIEW_H
#define TQTREEVIEW_H

// application header
#include "m_tweaq.h"

// base class
#include <QtreeView>

// drag and drop
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>

// mvc stuff
#include "afitem.h"

class TqTreeView : public QTreeView
{
    Q_OBJECT

public:
    explicit TqTreeView(QWidget* parent = Q_NULLPTR);

    // gets
    QModelIndexList selectedRows(void) const;


signals:
    void droppedAudioFileUrls(const QList<QUrl> urls);


protected:
    // drag and drop handling
    void dragEnterEvent(QDragEnterEvent* event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent* event);
};

#endif // TQTREEVIEW_H
