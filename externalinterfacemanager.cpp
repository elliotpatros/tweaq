#include "externalinterfacemanager.h"

ExternalInterfaceManager::ExternalInterfaceManager(const QString searchPath)
{
    if (searchPath.isEmpty()) return;

    FileList list;
    ExternFileImporter().importPath(searchPath, list, false);

    _externals.reserve(list.size());
    for (const auto path : list)
    {
        _externals.emplace_back(path);
    }
}

// gets
QStringList ExternalInterfaceManager::processNames() const
{
    QStringList list;
    for (const auto& lib : _externals)
    {
        list.append(lib.readableName());
    }

    return list;
}

ExternalInterface* ExternalInterfaceManager::get(int externalIndex)
{
    return isValid(externalIndex) ? &_externals[externalIndex] : nullptr;
}

bool ExternalInterfaceManager::isValid(const int externalIndex) const
{
    return 0 <= externalIndex && static_cast<size_t>(externalIndex) < _externals.size();
}
