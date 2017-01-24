#ifndef PARAMETERHANDLE_H
#define PARAMETERHANDLE_H

// application header
#include "tweaq.h"
#include "tweaqapi.h"

class ParameterHandle
{
public:
    explicit ParameterHandle();
    explicit ParameterHandle(const ParameterHandle& other);
    ParameterHandle& operator= (const ParameterHandle& other);
    ~ParameterHandle();

    // gets
    bool        hasLabels()            const;
    bool        hasDefaultValue()      const;
    bool        hasDescription()       const;
    bool        hasAnyFields()         const;
    int         nFields()              const;
    int         nLabels()              const;
    QString     description()          const;
    QString     defaultValue()         const;
    QString     label(const int index) const;
    QStringList labels()               const;
    Parameter&  reference();


private:
    Parameter p;

    void copy(const ParameterHandle& other);
};

#endif // PARAMETERHANDLE_H
