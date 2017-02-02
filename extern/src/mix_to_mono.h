#ifndef TQ_MIX_TO_MONO_
#define TQ_MIX_TO_MONO_

#include "tweaqapi.h"

enum ParameterFields {
    kAttenuationLabel = 0,
    kNumParameters
};

enum ParameterChoice {
    kNone,
    kMinusThree,
    kThreePerChannel,
    kOneOverNumChannels,
    kNumChoices
};

struct Input {
    ParameterChoice choice;
};

double calculate_gain(Input* input, const size_t nChannels);

#ifdef __cplusplus
extern "C"
{
#endif // def __cplusplus
    
    void mix_to_mono_setup(const int fieldNumber, Parameter& p);
    void* mix_to_mono_handleInput(int argc, const char* argv[]);
    bool mix_to_mono_process(const char* pathin, const char* pathout, void* args);
    
    
#ifdef __cplusplus
}
#endif // def__cplusplus

#endif // TQ_MIX_TO_MONO_ defined
