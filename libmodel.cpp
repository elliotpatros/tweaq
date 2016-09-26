#include "libmodel.h"

//==============================================================================
// constructor & destructor
//==============================================================================
LibModel::LibModel(QObject* parent, const QString searchPath) :
    QObject(parent)
{
    // append all the external libraries i can find to list
    QStringList paths;
    LibImporter().import(QList<QUrl>() << QUrl::fromLocalFile(searchPath), &paths);

    const t_uint nPaths = paths.size();
    for (t_uint i = 0; i < nPaths; ++i)
    {
        _libs.append(new LibMeta(paths.at(i)));
    }
}

LibModel::~LibModel(void)
{
    qDeleteAll(_libs);
}


//==============================================================================
// gets
//==============================================================================
QString LibModel::name(t_int index) const
{
    return _libs.at(index)->libName();
}

QStringList LibModel::names(void) const
{
    QStringList list;
    const t_uint nLibs = _libs.size();
    for (t_uint i = 0; i < nLibs; ++i)
    {
        list.append(name(i));
    }

    return list;
}

QFunctionPointer LibModel::getFunctionPointer(const t_int index, const QString functionName) const
{
    // load library with index
    QLibrary lib (_libs.at(index)->absolutePath());
    lib.load();

    // get name of function to call from library
    const QString funcName = _libs.at(index)->libFileName() % QStringLiteral("_") % functionName;
    return lib.resolve(funcName.toUtf8());
}

//==============================================================================
// do DSP
//==============================================================================
void LibModel::getExternalParameterHandles(const t_int libIndex, vector<ParameterHandle>* plist) const
{
    // load function from library with index
    ExternalSetup externalSetup = reinterpret_cast<ExternalSetup>(getFunctionPointer(libIndex, QStringLiteral("setup")));

    // call function
    if (externalSetup != nullptr)
    {
        t_uint index = TQ_NUM_DEF_ARGS;
        std::unique_ptr<t_parameter> p(new t_parameter);
        while (clearParameter(*p), externalSetup(index, *p))
        {
            plist->emplace_back(*p);
            setNextParameterIndex(*p, &index);
        }
    }
}

void LibModel::processFiles(const t_int libIndex, AFItem *rootItem, const QString destination, QStringList* args)
{
    // try to load the external dsp function
    ExternalProcessDSP dsp = reinterpret_cast<ExternalProcessDSP>(getFunctionPointer(libIndex, QStringLiteral("process")));

    // bail if the function couldn't load
    if (dsp == nullptr)
    {
        qDebug() << "bailed loading dsp function";
        return;
    }

    // setup dsp function arguments
    const t_uint argc = args->size() + TQ_NUM_DEF_ARGS;
    char** argv = (char**)calloc(argc + 1, sizeof(char*));

    // bail is memory couldn't be allocated
    if (argv == nullptr)
    {
        qDebug() << "bailed allocating argv";
        return;
    }

    // set arguments entered by the user (these won't change after this)
    for (t_uint i = 0; i < argc; ++i)
    {
        // make room for default args
        if (i < TQ_NUM_DEF_ARGS)
        {
            argv[i] = (char*)calloc(TQ_MAX_ARG_LENGTH, sizeof(char));

            // bail if memory couldn't be allocated
            if (&argv[i] == nullptr)
            {
                qDebug() << "bailed allocating argv[" << i << "]";
                return;
            }
        }
        else // setup arguments the user gave us
        {
            const QString arg = args->value(i - TQ_NUM_DEF_ARGS);
            argv[i] = (char*)calloc(arg.size() + 1, sizeof(char));

            // bail if memory couldn't be allocated
            if (&argv[i] == nullptr)
            {
                qDebug() << "bailed allocating argv[" << i << "]";
                return;
            }

            strcpy(argv[i], arg.toUtf8());
        }
    }

    // do the DSP for each file
    const t_int nAudioFiles = rootItem->childCount();
    for (t_int row = 0; row < nAudioFiles; ++row)
    {
        // do process anything that's already been processed
        AFItem* file = rootItem->child(row);
        if (file->hasBeenProcessed()) {continue; }

        // figure out the actual destination for this audio file
        const QString nameWithExt = file->nameWithExt();
        QString absDest = destination % QStringLiteral("/") % nameWithExt;

        // don't overwrite ever
        t_int iteration = 0;
        while (QFileInfo(absDest).exists())
        {
            absDest = destination
                    % QStringLiteral("/tweaqd-")
                    % QString::number(++iteration)
                    % QStringLiteral("_")
                    % nameWithExt;
        }

        // set default args
        setString(argv[0], file->absolutePath().toUtf8(), TQ_MAX_ARG_LENGTH);
        setString(argv[1], absDest.toUtf8(), TQ_MAX_ARG_LENGTH);

        // do DSP, update model
        file->setProccessingStatus(dsp(argc, const_cast<const char**>(argv)));
    }

    // clean up
    for (t_uint i = 0; i < argc; ++i)
    {
        free(argv[i]);
    }

    free(argv);
}
