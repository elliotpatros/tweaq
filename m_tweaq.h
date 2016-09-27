#ifndef M_TWEAQ_H
#define M_TWEAQ_H

//==============================================================================
// generic stuff
//==============================================================================
// includes
#include <stdarg.h>   // for variadic stuff
#include <stdint.h>   // for uint64_t
#include <string.h>   // for strlen, strcpy
#include <stdlib.h>   // for *alloc, free
#include <math.h>     // for math stuff

#ifdef __cplusplus
#include "qtweaq.h" // libsndfile.hh and others
#else  // __cpluspluc
#include "sndfile.h"
#endif // __cplusplus

// defines
#define TQ_BUFFERSIZE 4096
#define TQ_MAX_NAME_LENGTH 32
#define TQ_MAX_NUM_LABELS 8
#define TQ_MAX_LABEL_LENGTH 16
#define TQ_MAX_ARG_LENGTH 1024
#define TQ_MAX_NUM_ARGS 64
#define TQ_NUM_DEF_ARGS 2
#define TQ_TRUE 1
#define TQ_FALSE 0
#define TQ_PI 3.1415926535897932

// common types
typedef double   t_float;
typedef uint64_t t_uint;
typedef int64_t  t_int;


//==============================================================================
// parameter type
//==============================================================================
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

struct t_parameter {
    char    name   [TQ_MAX_NAME_LENGTH];
    char    labels [TQ_MAX_NUM_LABELS] [TQ_MAX_LABEL_LENGTH];

    t_float defaultValue;

    t_uint  nLabels;
    short   shouldUseDefault;
};

// generic function prototypes
void setString(char* destination, const char* source, const t_uint maxsize);
t_float stringToDouble(const char* string);
t_float dBToGain(const t_float dB);

// t_parameter function prototypes
t_uint nDefaultParameters(void);
void setNextParameterIndex(t_parameter& p, t_uint *currentIndex);
void setParameterName(t_parameter& p, const char* name);
void setParameterLabels(t_parameter& p, t_uint nLabels, ...);
void setParameterDefault(t_parameter& p, t_float value);
void clearParameter(t_parameter& p);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // M_TWEAQ_H
