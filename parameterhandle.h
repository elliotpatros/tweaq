#ifndef PARAMETERHANDLE_H
#define PARAMETERHANDLE_H

// application header
#include "m_tweaq.h"

class ParameterHandle
{
public:
    // constructors
    explicit ParameterHandle(void);
    explicit ParameterHandle(const t_parameter& p);
    explicit ParameterHandle(std::unique_ptr<t_parameter>& p);
    explicit ParameterHandle(const ParameterHandle& other);

    // gets
    t_parameter& get(void) const;

    //sets

    // properties
    std::unique_ptr<t_parameter> _p;
};

#endif // PARAMETERHANDLE_H
