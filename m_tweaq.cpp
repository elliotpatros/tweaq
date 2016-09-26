#include "m_tweaq.h"

// includes
#include <stdio.h>  // for sscanf

#ifndef __cplusplus
extern "C" {
#endif // __cplusplus


//==============================================================================
// general helper stuff
//==============================================================================
t_float stringToDouble(const char* string)
{
    t_float d;
    sscanf(string, "%lf", &d);
    return d;
}

t_float dBToGain(const t_float dB)
{
    return powf(10.0, dB / 20.0);
}


//==============================================================================
// parameter type
//==============================================================================
void setString(char* destination, const char* source, const t_uint maxsize)
{
    if (strlen(source) > maxsize)
    {
        strncpy(destination, source, maxsize);
    }
    else
    {
        strcpy(destination, source);
    }
}

void setNextParameterIndex(t_parameter& p, t_uint* currentIndex)
{
    if (p.shouldUseDefault == TQ_TRUE)
    {
        ++(*currentIndex);
    }

    if (p.nLabels > 1)
    {
        ++(*currentIndex);
    }
}

void setParameterName(t_parameter& p, const char* name)
{
    setString(p.name, name, TQ_MAX_NAME_LENGTH);
}

void setParameterLabels(t_parameter& p, t_uint nLabels, ...)
{
    // allocate space for list
    if (nLabels > TQ_MAX_NUM_LABELS)
    {
        nLabels = TQ_MAX_NUM_LABELS;
    }

    p.nLabels = nLabels;

    // set labels
    va_list list;
    va_start(list, nLabels);
    for (t_uint i = 0; i < p.nLabels; ++i)
    {
        setString(p.labels[i], va_arg(list, const char*), TQ_MAX_LABEL_LENGTH);
    }
}

void setParameterDefault(t_parameter& p, t_float value)
{
    p.defaultValue = value;
    p.shouldUseDefault = TQ_TRUE;
}

void clearParameter(t_parameter& p)
{
    memset(&p, 0, sizeof(p));
}

#ifndef __cplusplus
}
#endif // __cplusplus
