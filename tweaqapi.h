#ifndef _TWEAQAPI_H
#define _TWEAQAPI_H

// includes
#include <stdarg.h>     // variadic stuff
#include <string.h>     // strlen, strcpy
#include <stdlib.h>     // *alloc, free
#include <math.h>       // math stuff
#include <float.h>      // FLT_EPSILON
#include "sndfile.h"    // libsndfile

// defines
#define TQ_BUFFERSIZE 4096

// parameter type
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

struct Parameter {
    char*  description;
    char*  defaultValue;
    char** labels;
    int    nLabels;
};

// external api functions
typedef void  (*ExternalSetup)(const int field, Parameter& p);
typedef void* (*ExternalHandleInput)(int argc, const char* argv[]);
typedef bool  (*ExternalProcess)(const char* pathin, const char* pathout, void* args);
    
// helper functions
void* set_string(char*& destination, const char* source);
void  set_string_list(char**& dest, const int nStrings, ...);
void  set_string_list_private(char**& dest, const int nStrings, va_list list);
double get_max_gain(const char* pathin);

void free_string(char*& str);
void free_string_list(char**& strings, int nStrings);
double string_to_double(const char* str);
void dB_to_gain(double& dB);
double gain_from_dB(const double dB);

// type parameter functions
void set_parameter_description(Parameter& p, const char* name);
void set_parameter_labels(Parameter& p, const int nLabels, ...);
void set_parameter_default(Parameter& p, const char* value);
void init_parameter(Parameter& p);
void free_parameter(Parameter& p);
    
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _TWEAQAPI_H
