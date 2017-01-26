#ifndef __tests__ExternalHelpers__
#define __tests__ExternalHelpers__

#include <math.h>

// curves
typedef double (*CurveType)(const double);

double curve_s(const double x) {
    if      (x >= 1.) return 1.;
    else if (x <= 0.) return 0.;
    
    return -0.5 * cos(M_PI * x) + 0.5;
}

double curve_lin(const double x) {
    if      (x >= 1.) return 1.;
    else if (x <= 0.) return 0.;

    return x;
}

double curve_log(const double x) {
    if      (x >= 1.) return 1.;
    else if (x <= 0.) return 0.;
    
    return 2. * x - x * x;
}

double curve_exp(const double x) {
    if      (x >= 1.) return 1.;
    else if (x <= 0.) return 0.;
    
    return x * x;
}

// time
enum TimeType {
    Milliseconds = 0,
    Seconds,
    Samples,
    NumTimeTypes
};

#endif /* defined(__tests__ExternalHelpers__) */
