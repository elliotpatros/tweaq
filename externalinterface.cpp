#include "externalinterface.h"

ExternalInterface::ExternalInterface(const QString fileName) :
    _absolutePath(fileName) {}

QString ExternalInterface::libraryName() const
{
    return QFileInfo(_absolutePath).baseName().split("lib").at(1);
}

QString ExternalInterface::readableName() const
{
    return libraryName().replace(QStringLiteral("_"), QStringLiteral(" "));
}


// sets
vector<ParameterHandle> ExternalInterface::setup() const
{   // todo: error handling
    vector<ParameterHandle> parameters;
    int fieldCount = 0;

    while (true)
    {   // ask external library to setup this parameter
        ParameterHandle handle;
        functionSetup()(fieldCount, handle.reference());

        // add to list if it's been setup
        if (!handle.hasAnyFields()) break;
        fieldCount += handle.nFields();
        parameters.emplace_back(handle);
    }

    return parameters;
}

void* ExternalInterface::handleInput(const vector<QString> responses) const
{   // todo: find a better solution than returning nullptr on failure.
    // convert user responses to argv/c
    const int argc = static_cast<int>(responses.size());
    if (argc < 1) return nullptr;

    char** argv = (char**)calloc(argc, sizeof(char*));
    if (argv == nullptr) return nullptr;

    for (int i = 0; i < argc; i++)
    {
        set_string(argv[i], responses[i].toUtf8());
    }

    void* argList = functionHandleInput()(argc, const_cast<const char**>(argv));
    free_string_list(argv, argc);

    return argList;
}


// gets
QFunctionPointer ExternalInterface::functionPointer(const QString functionName) const
{
    QLibrary lib(_absolutePath);
    const QString function(libraryName() % QStringLiteral("_") % functionName);
    return lib.resolve(function.toUtf8());
}

ExternalSetup ExternalInterface::functionSetup() const
{
    return reinterpret_cast<ExternalSetup>(functionPointer(QStringLiteral("setup")));
}

ExternalHandleInput ExternalInterface::functionHandleInput() const
{
    return reinterpret_cast<ExternalHandleInput>(functionPointer(QStringLiteral("handleInput")));
}

ExternalProcess ExternalInterface::process() const
{
    return reinterpret_cast<ExternalProcess>(functionPointer(QStringLiteral("process")));
}
