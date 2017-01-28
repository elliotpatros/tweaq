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

// widgets
#include <QLabel>
#include <QVBoxLayout>

class StartupHint
{
public:
    explicit StartupHint(QWidget* const parent = nullptr);

    // sets
    void show(QWidget* widget);
    void hide();


private:
    QVBoxLayout* _layout;
    QLabel* _label;
};

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
    // events
    void dragEnterEvent(QDragEnterEvent* event);
    void dragMoveEvent(QDragMoveEvent* event);
    void dropEvent(QDropEvent* event);


private:
    StartupHint _hint;
};

#endif // TREEVIEWAUDIOFILE_H
