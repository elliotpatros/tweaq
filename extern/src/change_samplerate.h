#ifndef TQ_CHANGE_SAMPLERATE_
#define TQ_CHANGE_SAMPLERATE_

#include "tweaqapi.h"

struct Input {
    // sample rate conversion
    double sampleRate;
    int converter;
};

enum ParameterFields{
    kSampleRate = 0,
    kQuality,
    kNumParameters
};

void src_apply_gain(float* samples, const long nSamples, double gain, double& peakGain);
double do_src(SNDFILE* filein, SNDFILE* fileout, int converter, double src_ratio, int nChannels, double gain);
bool setup_format_out(Input* input, SF_INFO& sfinfo);
bool tq_apply_gain(const char* pathin, const char* pathout, const double gain, Input* input);

#ifdef __cplusplus
extern "C"
{
#endif // def __cplusplus
    
void change_samplerate_setup(const int fieldNumber, Parameter& p);
void* change_samplerate_handleInput(int argc, const char* argv[]);
bool change_samplerate_process(const char* pathin, const char* pathout, void* args);

#ifdef __cplusplus
}
#endif // def __cplusplus

#endif // TQ_CHANGE_SAMPLERATE_ defined
