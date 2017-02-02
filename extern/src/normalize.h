#ifndef TQ_NORMALIZE_
#define TQ_NORMALIZE_

#include "tweaqapi.h"

enum {
    kGain = 0,
    kGainLabel,
    kNumParameters
};

struct Input {
    double gain;
};


#ifdef __cplusplus
extern "C"
{
#endif // def __cplusplus
    
    void normalize_setup(const int fieldNumber, Parameter& p);
    void* normalize_handleInput(int argc, const char* argv[]);
    bool normalize_process(const char* pathin, const char* pathout, void* args);
    
#ifdef __cplusplus
}
#endif // def __cplusplus

#endif // TQ_NORMALIZE_ defined
