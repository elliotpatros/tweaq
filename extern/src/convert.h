#ifndef TQ_CONVERT_
#define TQ_CONVERT_

#include "tweaqapi.h"

struct Input {
    // sample rate conversion
    double samplerate;
    int converter;
    
    // bit depth conversion
    int bitDepth;
    
    // format conversion
    int format;
};

enum ParameterFields{
    kSampleRate = 0,
    kQuality,
    kBitDepth,
    kFormat,
    kNumParameters
};

char* get_pathout(const char* pathout, const int format);
void src_apply_gain(float* samples, const long nSamples, double gain, double& peakGain);
double do_src(const char* pathin, const char* pathout, const double gain, Input* input);
bool validate_format_out(Input* input, SF_INFO& sfinfo);
bool post_apply_gain(const char* pathin, const char* pathout, const double gain, Input* input);

#ifdef __cplusplus
extern "C"
{
#endif // def __cplusplus
    
void convert_setup(const int fieldNumber, Parameter& p);
void* convert_handleInput(int argc, const char* argv[]);
bool convert_process(const char* pathin, const char* pathout, void* args);
    
#ifdef __cplusplus
}
#endif // def __cplusplus

#endif // TQ_CONVERT_ defined
