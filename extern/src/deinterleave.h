#ifndef TQ_DEINTERLEAVE_
#define TQ_DEINTERLEAVE_

#include "tweaqapi.h"

void insertChannelNumber(const char* input, const size_t channel, char*& output);

#ifdef __cplusplus
extern "C"
{
#endif // def __cplusplus
void deinterleave_setup(const int fieldNumber, Parameter& p);
void* deinterleave_handleInput(int argc, const char* argv[]);
bool deinterleave_process(const char* pathin, const char* pathout, void* args);
#ifdef __cplusplus
}
#endif // def __cplusplus

#endif // TQ_DEINTERLEAVE_ defined
