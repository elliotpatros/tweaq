#ifndef AFMODEL_H
#define AFMODEL_H

// application header
#include "m_tweaq.h"

// gui
#include <QColor>

// threads
#include <QThread>

//base class
#include <QAbstractItemModel>

// mvc stuff
#include <QModelIndex>
#include <QSortFilterProxyModel>

// tweaq classes
#include "afitem.h"


class AFModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    // constructor & destructor
    explicit AFModel(QObject* parent = Q_NULLPTR);
    AFModel(const AFModel&);
    ~AFModel(void);

    // gets
    AFItem* rootItem(void) const;
    QSortFilterProxyModel* sortProxy(void) const;
    bool areThereAnyUnprocessedAudioFiles(void) const;
    bool hasBeenProcessed(const t_int row) const;

    // sets
    void addAudioFiles(QStringList* paths);
    void appendChildToRoot(const QString path);
    void removeAllRows(void);
    void removeSelectedRows(QModelIndexList indices);
    void removeFinishedRows(void);

    // model things
    QVariant data(const QModelIndex& index, int role) const Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex& index) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex parent(const QModelIndex& index) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex& = QModelIndex()) const Q_DECL_OVERRIDE;


private:
    // owned
    AFItem* _rootItem;
    QSortFilterProxyModel* _sortProxy;

    // convenience
    void removeAListOfRows(vector<t_int>& rowsToRemove);
    AFItem* itemAtIndex(const QModelIndex& index) const;
    AFItem* validItemAtParentIndex(const QModelIndex& parentIndex) const;
};

#endif // AFMODEL_H
