#include "afitem.h"

//==============================================================================
// constructors & destructor
//==============================================================================

// called when constructing imported audio files
AFItem::AFItem(const QString path, AFItem* parentItem) :
    AFMeta(path)
{
    // copy parent item
    _parentItem = parentItem;

    // setup audio file meta data for model
    // ... this should be in the same order as the PropertyIndex enum
    _itemData.emplace_back(absolutePath());
    _itemData.emplace_back(baseName());
    _itemData.emplace_back(extension());
    _itemData.emplace_back(channels());
    _itemData.emplace_back(durationAsString());
    _itemData.emplace_back(sampleRate());
    _itemData.emplace_back(parentDir());

    // processing status
    completed = Unprocessed;
}

// called when constructing tree view column names (AFMeta will be empty)
AFItem::AFItem(const vector<QVariant> data, AFItem *parentItem) :
    AFMeta(QString())
{
    _itemData = data;
    _parentItem = parentItem;

    completed = Unprocessed;
}

AFItem::AFItem(const AFItem& other) :
    AFMeta(other.absolutePath())
{
    // copy data
    _parentItem = other._parentItem;
    _itemData = other._itemData;
    completed = other.completed;

    // kill all my own children...
    _childItems.clear();

    // ...and steal someone else's
    const t_int nChildren = other.childCount();
    for (t_int i = 0; i < nChildren; ++i)
    {
        appendChild(new AFItem(other.absolutePath(), this /* i'm your parent now */));
    }
}

AFItem::~AFItem(void)
{
    _childItems.clear();
}


//==============================================================================
// sets
//==============================================================================
void AFItem::appendChild(AFItem *child)
{
    _childItems.push_back(child);
}

void AFItem::removeChild(t_int row)
{
    if (childIndexIsValid(row))
    {
        _childItems.erase(_childItems.begin() + row);
    }
}

void AFItem::setProccessingStatus(const bool succeeded)
{
    completed = succeeded ? SucceededDSP : FailedDSP;
}

//==============================================================================
// gets
//==============================================================================
bool AFItem::childIndexIsValid(const t_int index) const
{
    return static_cast<t_uint>(index) < _childItems.size();
}

bool AFItem::hasBeenProcessed(void) const
{
    return completed != AFItem::Unprocessed;
}

AFItem* AFItem::child(t_int row)
{
    return childIndexIsValid(row) ? _childItems[row] : nullptr;
}

t_int AFItem::childCount(void) const
{
    return _childItems.size();
}

t_int AFItem::columnCount(void)
{
    return NumProperties;
}

t_int AFItem::visibleColumnCount(void)
{
    // absolute path column will always be invisible
    return NumProperties - 1;
}

QVariant AFItem::data(t_int column) const
{
    return _itemData.at(column);
}

t_int AFItem::isAtRow(void) const
{
    // find the row index of this item in it's parent vector
    if (_parentItem != nullptr)
    {
        const vector<AFItem*>::iterator first = _parentItem->_childItems.begin();
        const vector<AFItem*>::iterator last = _parentItem->_childItems.end();
        const vector<AFItem*>::iterator it = std::find(first, last, this);
        return (it == last) ? 0 : static_cast<t_int>(std::distance(first, it));
    }

    return 0;
}

AFItem* AFItem::parentItem(void)
{
    return _parentItem;
}

vector<QVariant> AFItem::soundFileProperties(void)
{
    vector<QVariant> properties;
    properties.emplace_back(QStringLiteral("absolute path"));
    properties.emplace_back(QStringLiteral("name"));
    properties.emplace_back(QStringLiteral("type"));
    properties.emplace_back(QStringLiteral("channels"));
    properties.emplace_back(QStringLiteral("duration"));
    properties.emplace_back(QStringLiteral("sample rate"));
    properties.emplace_back(QStringLiteral("directory"));

    return properties;
}
