#include "af_item.h"

AF_Item::AF_Item(const QString path, AF_Item* const parent) :
    AF_Properties(path),
    _parent(parent),
    _dsp_status(DSP_Status::Unprocessed) {}

AF_Item::AF_Item(const vector<QVariant> data, AF_Item* const parent) :
    AF_Properties(data),
    _parent(parent),
    _dsp_status(DSP_Status::Unprocessed) {}

AF_Item::AF_Item(const AF_Item& other) :
    AF_Properties(other)
{
    copy(other);
}

AF_Item& AF_Item::operator=(const AF_Item& other)
{
    copy(other);
    _data = std::move(other._data);
    return *this;
}


// sets
void AF_Item::filterDuplicates(FileList& list) const
{
    const auto alreadyImported = [this](const QString path){return this->hasChildPath(path); };
    erase_if(list, alreadyImported);
}

void AF_Item::processAudioFiles(const ExternalProcess process, const QString outputDir, void* args)
{
    for (AF_Item& file : _children)
    {
        if (QThread::currentThread()->isInterruptionRequested()) return;
        if (file.wasProcessed()) continue;

        const QString input = file.absolutePath();
        const QString output = file.uniquePath(outputDir);
        const bool success = process(input.toUtf8(), output.toUtf8(), args);

        file._dsp_status = success ? DSP_Status::Succeeded : DSP_Status::Failed;
    }
}

void AF_Item::appendChildren(const FileList list)
{
    if (list.empty()) return;

    _children.reserve(_children.size() + list.size());
    for (const QString path : list)
    {
        _children.emplace_back(path, this);
    }
}

void AF_Item::removeChildren(const vector<int> list)
{
    if (list.empty())
    {   // remove all children
        _children.clear();
    }
    else
    {   // remove a list of children
        erase_if(_children, [list](const AF_Item& item)
        {
            return in_container(item.index(), list);
        });
    }
}


// gets
bool AF_Item::wasProcessed() const
{
    return _dsp_status != DSP_Status::Unprocessed;
}

QVariant AF_Item::statusColor() const
{
    switch (_dsp_status)
    {
    case DSP_Status::Unprocessed: return QVariant();
    case DSP_Status::Succeeded:   return QColor(16, 217, 32);
    case DSP_Status::Failed:      return QColor(255, 54, 0);
    }
}

bool AF_Item::hasUnprocessedChildren() const
{
    if (_children.empty()) return false;

    return !std::all_of(_children.begin(), _children.end(), [](const AF_Item& child)
    {
        return child.wasProcessed();
    });
}

vector<int> AF_Item::processedChildren() const
{
    vector<int> list;
    const size_t nChildren = _children.size();
    if (nChildren == 0) return list;

    list.reserve(_children.size());
    for (size_t i = 0; i < nChildren; i++)
    {
        if (_children[i].wasProcessed())
        {
            list.push_back(i);
        }
    }

    return list;
}

bool AF_Item::hasChildPath(const QString path) const
{
    return std::any_of(_children.begin(), _children.end(), [&path](const AF_Item& item)
    {
        return path == item.absolutePath();
    });
}

AF_Item* AF_Item::child(const int row)
{
    return childIndexIsValid(row) ? &_children[row] : nullptr;
}

int AF_Item::nChildren() const
{
    return _children.size();
}

QVariant AF_Item::data(const int column) const
{
    return indexIsValid(column) ? _data[column] : QVariant();
}

int AF_Item::index() const
{
    return (_parent == nullptr) ? -1 : (this - &_parent->_children[0]);
}

AF_Item* AF_Item::parent() const
{
    return _parent;
}

void AF_Item::copy(const AF_Item& other)
{
    _dsp_status = other._dsp_status;
    _parent = other._parent;
    _children = std::move(other._children);
}

bool AF_Item::childIndexIsValid(const int index) const
{
    return 0<= index && static_cast<size_t>(index) < _children.size();
}
