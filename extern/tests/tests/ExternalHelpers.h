#ifndef __tests__ExternalHelpers__
#define __tests__ExternalHelpers__

#include <math.h>

void clip_double(double& val, const double min, const double max)
{
    if      (val > max) val = max;
    else if (val < min) val = min;
}

// curves
typedef double (*CurveType)(const double);

double curve_s(const double x) {
    return -0.5 * cos(M_PI * x) + 0.5;
}

double curve_lin(const double x) {
    return x;
}

double curve_log(const double x) {
    return 2. * x - x * x;
}

double curve_exp(const double x) {
    return x * x;
}

double curve_none(const double x) {
    return 1.;
}

// time
enum TimeType {
    Milliseconds = 0,
    Seconds,
    Samples,
    NumTimeTypes
};

void to_samples(double& time, const TimeType timeType, const double samplerate)
{
    switch (timeType)
    {
        case Milliseconds:
            time *= samplerate * 0.001;
            break;
        case Seconds:
            time *= samplerate;
            break;
        default: break;
    }
}
//if      (x >= 1.) return 1.;
//else if (x <= 0.) return 0.;

#endif /* defined(__tests__ExternalHelpers__) */
