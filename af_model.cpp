#include "af_model.h"

// constructor and destructor
AF_Model::AF_Model(QObject* parent) :
    QAbstractItemModel(parent),
    _rootItem(new AF_Item)
{
    disconnect();
    setupSortingProxy();
}

AF_Model::AF_Model(const AF_Model& other) :
    QAbstractItemModel(other.QAbstractItemModel::parent()),
    _rootItem(new AF_Item(*other._rootItem))
{
    disconnect();
    setupSortingProxy();
}

AF_Model& AF_Model::operator= (const AF_Model& other)
{
    _rootItem.reset(new AF_Item(*other._rootItem));
    setupSortingProxy();

    return *this;
}


// sets
void AF_Model::importAudioFiles(const QList<QUrl> urls)
{   // todo: refactor sound to audio everywhere
    if (urls.isEmpty()) return;

    // import audio files
    FileList fileList;
    AudioFileImporter().importUrls(urls, fileList);
    if (fileList.empty()) return;

    // remove paths that have already been imported
    const auto alreadyImported = [this](const QString path){return _rootItem->hasChildPath(path); };
    erase_if(fileList, alreadyImported);
    if (fileList.empty()) return;

    // add imported files to model
    beginInsertRows(QModelIndex(), rowCount(), rowCount() + (fileList.size() - 1));
    _rootItem->appendChildren(fileList);
    endInsertRows();
    resort();
}

void AF_Model::removeAllRows()
{
    if (rowCount() < 1) return;

    beginRemoveRows(QModelIndex(), 0, rowCount() - 1);
    _rootItem->removeChildren();
    endRemoveRows();
    resort();
}

void AF_Model::removeFinishedRows()
{
    removeAListOfRows(_rootItem->processedChildren());
}

void AF_Model::removeSelectedRows(const QModelIndexList list)
{
    if (list.isEmpty()) return;
    removeAListOfRows(mapRowsFromSortingProxy(list));
}

void AF_Model::removeAListOfRows(const vector<int> rows)
{
    if (rows.empty()) return;

    beginRemoveRows(QModelIndex(), rowCount() - rows.size(), rowCount() - 1);
    _rootItem->removeChildren(rows);
    endRemoveRows();
    resort();
}


// gets
AF_Item* AF_Model::rootItem() const
{
    return _rootItem.get();
}

QSortFilterProxyModel* AF_Model::sortProxy() const
{
    return _sortProxy;
}

bool AF_Model::areAnyAudioFilesUnprocessed() const
{
    return _rootItem->hasUnprocessedChildren();
}


// model things
QVariant AF_Model::data(const QModelIndex& index, int role) const
{
    if (index.isValid())
    {
        switch (role)
        {
        case Qt::DisplayRole:    return itemAtIndex(index)->data(index.column());
        case Qt::BackgroundRole: return itemAtIndex(index)->statusColor();
        default:                 break;
        }
    }

    return QVariant();
}

Qt::ItemFlags AF_Model::flags(const QModelIndex& index) const
{
    return index.isValid() ? (Qt::ItemIsEnabled | Qt::ItemIsSelectable) : Qt::ItemIsEnabled;
}

QVariant AF_Model::headerData(int column, Qt::Orientation orientation, int role) const
{
    return (orientation == Qt::Horizontal && role == Qt::DisplayRole) ? _rootItem->data(column) : QVariant();
}

QModelIndex AF_Model::index(int row, int column, const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent)) return QModelIndex();

    AF_Item* const child = itemAtParentIndex(parent)->child(row);
    return (child != nullptr) ? createIndex(row, column, child) : QModelIndex();
}

QModelIndex AF_Model::parent(const QModelIndex& index) const
{
    if (!index.isValid()) return QModelIndex();

    AF_Item* const parent = itemAtIndex(index)->parent();
    return (parent != _rootItem.get()) ? createIndex(parent->index(), 0, parent) : QModelIndex();
}

int AF_Model::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : itemAtParentIndex(parent)->nChildren();
}

int AF_Model::columnCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : AF_Properties::NumProperties;
}


// helpers
void AF_Model::resort() const noexcept
{
    _sortProxy->sort(_sortProxy->sortColumn() - 1);
}

AF_Item* AF_Model::itemAtIndex(const QModelIndex& index) const noexcept
{
    return reinterpret_cast<AF_Item*>(index.internalPointer());
}

AF_Item* AF_Model::itemAtParentIndex(const QModelIndex& parentIndex) const noexcept
{
    return parentIndex.isValid() ? itemAtIndex(parentIndex) : _rootItem.get();
}

void AF_Model::setupSortingProxy()
{
    _sortProxy = new QSortFilterProxyModel(this);
    _sortProxy->setSourceModel(this);
    _sortProxy->setSortCaseSensitivity(Qt::CaseInsensitive);
}

vector<int> AF_Model::mapRowsFromSortingProxy(const QModelIndexList list) const
{
    const int nVisibleColumns = columnCount() - 1;
    vector<int> rows(list.size() / nVisibleColumns);

    int i = 0;
    std::generate(rows.begin(), rows.end(), [&]
    {
        return _sortProxy->mapToSource(list[i++ * nVisibleColumns]).row();
    });

    return rows;
}
