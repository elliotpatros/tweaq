#include "afmodel.h"

//==============================================================================
// constructor & destructor
//==============================================================================
AFModel::AFModel(QObject* parent) :
    QAbstractItemModel(parent),
    _rootItem(nullptr),
    _sortProxy(nullptr)
{
    _rootItem = new AFItem;
    _sortProxy = new QSortFilterProxyModel;
    _sortProxy->setSourceModel(this);
}

AFModel::AFModel(const AFModel&) :
    QAbstractItemModel(nullptr),
    _rootItem(nullptr),
    _sortProxy(nullptr)
{
    _rootItem = new AFItem;
    _sortProxy = new QSortFilterProxyModel;
    _sortProxy->setSourceModel(this);
}

AFModel::~AFModel(void)
{
    if (_rootItem != nullptr)
    {
        delete _rootItem;
        _rootItem = nullptr;
    }

    if (_sortProxy != nullptr)
    {
        delete _sortProxy;
        _sortProxy = nullptr;
    }
}


//==============================================================================
// gets
//==============================================================================
AFItem* AFModel::rootItem(void) const
{
    return _rootItem;
}

QSortFilterProxyModel* AFModel::sortProxy(void) const
{
    return _sortProxy;
}

bool AFModel::hasBeenProcessed(const t_int row) const
{
    return _rootItem->child(row)->hasBeenProcessed();
}

AFItem* AFModel::itemAtIndex(const QModelIndex& index) const
{
    return reinterpret_cast<AFItem*>(index.internalPointer());
}

AFItem* AFModel::validItemAtParentIndex(const QModelIndex& parentIndex) const
{
    return (parentIndex.isValid()) ? itemAtIndex(parentIndex) : _rootItem;
}

bool AFModel::areThereAnyUnprocessedAudioFiles(void) const
{
    const t_int nFiles = rowCount();
    for (t_int row = 0; row < nFiles; ++row)
    {
        if (!hasBeenProcessed(row))
        {
            return true;
        }
    }

    return false;
}


//==============================================================================
// sets
//==============================================================================
void AFModel::addAudioFiles(QStringList* paths)
{
    if (paths == nullptr) {return; }
    if (paths->isEmpty())  {return; }

    // remove items that have already been imported
    if (rowCount() > 0)
    {
        t_int i = paths->size();
        while (i-- > 0)
        {
            // is there at least one new entry that matches an old entry?
            if (!match(index(0, 0), Qt::DisplayRole, QVariant(paths->at(i)), 1).isEmpty())
            {
                paths->removeAt(i);
            }
        }
    }

    // add any remaining imported paths to model (as rows)
    if (paths->isEmpty()) {return; }

    // tell the model we're going to add new paths to the end
    const t_int nPaths = paths->size();
    const t_int newRowsStart = rowCount();
    const t_int newRowsEnd = newRowsStart + (nPaths - 1);
    beginInsertRows(QModelIndex(), newRowsStart, newRowsEnd);

    // add the new paths to model, and tell it when we're done
    for (t_int i = 0; i < nPaths; ++i)
    {
        appendChildToRoot(paths->at(i));
    }

    endInsertRows();
}

void AFModel::appendChildToRoot(const QString path)
{
    _rootItem->appendChild(new AFItem(path, _rootItem));
}

void AFModel::removeSelectedRows(QModelIndexList indices)
{
    // don't do no work...
    if (indices.isEmpty()) {return; }

    // map rows to match the unsorted model
    const t_int nColumns = AFItem::visibleColumnCount();
    const t_int nRows = indices.size() / nColumns;
    vector<t_int> selectedRows;
    for (t_int i = 0; i < nRows; ++i)
    {
        selectedRows.push_back(_sortProxy->mapToSource(indices.at(i * nColumns)).row());
    }

    // remove the list
    removeAListOfRows(selectedRows);
}

void AFModel::removeFinishedRows(void)
{
    // create a list of rows that are finished processing
    const t_int nRows = _rootItem->childCount();
    vector<t_int> finishedRows;

    for (t_int i = 0; i < nRows; ++i)
    {
        if (hasBeenProcessed(i))
        {
            finishedRows.push_back(i);
        }
    }

    // remove the list
    removeAListOfRows(finishedRows);
}

void AFModel::removeAllRows(void)
{
    t_int nRows = _rootItem->childCount();
    if (nRows < 1) {return; } // don't do no work

    beginRemoveRows(QModelIndex(), 0, nRows - 1);
    while (nRows-- > 0)
    {
        _rootItem->removeChild(nRows);
    }

    endRemoveRows();
}

void AFModel::removeAListOfRows(vector<t_int>& rowsToRemove)
{
    // don't do no work
    if (rowsToRemove.empty()) {return; }

    // sort in descending order, so we don't have to shift indices
    std::sort(rowsToRemove.begin(), rowsToRemove.end(), std::greater<t_int>());

    // remove a list of rows.
    const t_int nRowsToRemove = rowsToRemove.size();
    for (t_int i = 0; i < nRowsToRemove; ++i)
    {
        const t_int rowToRemove = rowsToRemove[i];
        beginRemoveRows(QModelIndex(), rowToRemove, rowToRemove);
        _rootItem->removeChild(rowToRemove);
        endRemoveRows();
    }
}

//==============================================================================
// model things
//==============================================================================
QModelIndex AFModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent))
    {
        return QModelIndex();
    }

    AFItem* parentItem = validItemAtParentIndex(parent);
    AFItem* childItem = parentItem->child(row);

    if (childItem == nullptr)
    {
        return QModelIndex();
    }

    return createIndex(row, column, childItem);
}

QModelIndex AFModel::parent(const QModelIndex& index) const
{
    if (!index.isValid())
    {
        return QModelIndex();
    }

    AFItem* childItem = itemAtIndex(index);
    AFItem* parentItem = childItem->parentItem();

    if (parentItem == _rootItem)
    {
        return QModelIndex();
    }

    return createIndex(parentItem->isAtRow(), 0, parentItem);
}

int AFModel::rowCount(const QModelIndex& parent) const
{
    if (parent.column() > 0)
    {
        return 0;
    }

    return validItemAtParentIndex(parent)->childCount();
}

int AFModel::columnCount(const QModelIndex&) const
{
    return AFItem::columnCount();
}

QVariant AFModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    switch (role)
    {
    case Qt::DisplayRole:
        return itemAtIndex(index)->data(index.column());
    case Qt::BackgroundRole:
        switch (itemAtIndex(index)->completed)
        {
        case AFItem::SucceededDSP: return QColor(16, 217, 32);
        case AFItem::FailedDSP:    return QColor(255, 54, 0);
        default:                   return QVariant();
        }
    default:
        return QVariant();
    }
}

Qt::ItemFlags AFModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
    {
        return Qt::ItemIsEnabled;
    }

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant AFModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        return _rootItem->data(section);
    }

    return QVariant();
}
