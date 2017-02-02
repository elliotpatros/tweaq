#ifndef TQ_FADE_IN_
#define TQ_FADE_IN_

#include "tweaqapi.h"

// time
enum TimeType {
    Milliseconds = 0,
    Seconds,
    Samples,
    NumTimeTypes
};

void to_samples(double& time, const TimeType timeType, const double samplerate);

// curves
typedef double (*CurveType)(const double);
double curve_s(const double x);
double curve_lin(const double x);
double curve_log(const double x);
double curve_exp(const double x);
double curve_none(const double x);

enum ParameterFields {
    kDuration,
    kTimeType,
    kCurveType,
    kNumParameters
};

struct Input {
    CurveType fade;
    TimeType  timeType;
    double    fadeDuration;
};

#ifdef __cplusplus
extern "C"
{
#endif // def __cplusplus
    
    void  fade_in_setup(const int fieldNumber, Parameter& p);
    void* fade_in_handleInput(int argc, const char* argv[]);
    bool  fade_in_process(const char* pathin, const char* pathout, void* args);
    
#ifdef __cplusplus
}
#endif // def __cplusplus


#endif // TQ_FADE_IN_ defined
