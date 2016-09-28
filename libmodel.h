#ifndef LIBMODEL_H
#define LIBMODEL_H

// application header
#include "m_tweaq.h"

// base class
#include <QObject>

// tweaq classes
#include "libimporter.h"
#include "libmeta.h"
#include "parameterhandle.h"
#include "afmodel.h"

class LibModel : public QObject
{
    Q_OBJECT

public:
    // constructor & destructor
    explicit LibModel(QObject* parent = Q_NULLPTR, const QString searchPath = QString());
    ~LibModel(void);

    // DSP
    void getExternalParameterHandles(const t_int libIndex, vector<ParameterHandle>* plist) const;
    void processFiles(const t_int libIndex, AFItem* rootItem, const QString destination, QStringList* args);

    // gets
    QStringList names(void) const;
    QString name(t_int index) const;


private:
    // pointer to meta data about external libraries
    vector<LibMeta*> _libs;

    // external function definitions
    typedef bool (*ExternalProcessDSP)(t_uint argc, const char* argv[]);
    typedef bool (*ExternalSetup)(const t_uint index, t_parameter& p);

    // convenience
    QFunctionPointer getFunctionPointer(const t_int index, const QString functionName) const;

};

#endif // LIBMODEL_H
