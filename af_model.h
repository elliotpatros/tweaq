#ifndef AFMODEL_H
#define AFMODEL_H

// application header
#include "tweaq.h"

// base class
#include <QAbstractItemModel>

// Qt classes
#include <QColor>

// tweaq classes
#include "fileimporter.h"

// mvc stuff
#include <QModelIndex>
#include <QSortFilterProxyModel>
#include "af_item.h"

class AF_Model : public QAbstractItemModel
{
    Q_OBJECT

public:
    // constructor and destructor
    explicit AF_Model(QObject* parent = nullptr);
    explicit AF_Model(const AF_Model& other);
    AF_Model& operator= (const AF_Model& other);

    // sets
    void importAudioFiles(const QList<QUrl> urls);
    void removeAllRows();
    void removeFinishedRows();
    void removeSelectedRows(const QModelIndexList list);

    // gets
    QSortFilterProxyModel* sortProxy() const;
    bool areAnyAudioFilesUnprocessed() const;
    AF_Item* rootItem() const;

    // inhereted model behavior
    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex& index) const Q_DECL_OVERRIDE;
    QVariant headerData(int column, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex parent(const QModelIndex& index) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;

private:
    // owned members
    unique_ptr<AF_Item> _rootItem;
    QSortFilterProxyModel* _sortProxy;

    // convenience functions
    AF_Item* itemAtIndex(const QModelIndex& index) const noexcept;
    AF_Item* itemAtParentIndex(const QModelIndex& parentIndex) const noexcept;
    void resort() const noexcept;
    void setupSortingProxy();
    vector<int> mapRowsFromSortingProxy(const QModelIndexList list) const;
    void removeAListOfRows(const vector<int> rows);
};

#endif // AFMODEL_H
