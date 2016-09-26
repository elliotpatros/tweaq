#include "parameterhandle.h"

//==============================================================================
// constructors
//==============================================================================
ParameterHandle::ParameterHandle(void) :
    _p(new t_parameter)
{}

ParameterHandle::ParameterHandle(const t_parameter &p) :
    _p(new t_parameter(p))
{}

ParameterHandle::ParameterHandle(std::unique_ptr<t_parameter>& p) :
    _p(new t_parameter(*p))
{}

ParameterHandle::ParameterHandle(const ParameterHandle& other) :
    _p(new t_parameter(*other._p))
{}

//==============================================================================
// gets
//==============================================================================
t_parameter& ParameterHandle::get(void) const
{
    return *_p;
}
