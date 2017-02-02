#include "fade_in_and_out.h"

static const char*  timeTypes[] = {"milliseconds", "seconds", "samples"};
static const char* curveNames[] = {"logarithmic", "exponential", "linear", "'S' curve", "none"};
static const CurveType Curves[] = {curve_log, curve_exp, curve_lin, curve_s, curve_none};
static const size_t nCurveTypes = sizeof(Curves) / sizeof(CurveType);

// curves
double curve_s(const double x)    {return -0.5 * cos(M_PI * x) + 0.5; }
double curve_lin(const double x)  {return x; }
double curve_log(const double x)  {return 2. * x - x * x; }
double curve_exp(const double x)  {return x * x; }
double curve_none(const double x) {(void)x; return 1.; }

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

#ifdef __cplusplus
extern "C"
{
#endif // def __cplusplus
    
    void fade_in_and_out_setup(const int fieldNumber, Parameter& p)
    {
        switch (fieldNumber)
        {
            case kDurationIn:
                set_parameter_description(p, "fade in for how long?");
                set_parameter_default(p, "125.0");
                set_parameter_labels(p, NumTimeTypes, timeTypes);
                break;
            case kCurveTypeIn:
                set_parameter_description(p, "what kind of curve should the fade in have?");
                set_parameter_labels(p, nCurveTypes, curveNames);
                break;
            case kDurationOut:
                set_parameter_description(p, "fade out for how long?");
                set_parameter_default(p, "400.0");
                set_parameter_labels(p, NumTimeTypes, timeTypes);
                break;
            case kCurveTypeOut:
                set_parameter_description(p, "what kind of curve should the fade out have?");
                set_parameter_labels(p, nCurveTypes, curveNames);
                break;
            default: break;
        }
    }
    
    void* fade_in_and_out_handleInput(int argc, const char* argv[])
    {
        if (argc != kNumParameters) return 0;
        
        Input* input = (Input*)malloc(sizeof(Input));
        if (input == 0) return input;
        
        // get duration as double
        input->fadeDurationIn = string_to_double(argv[kDurationIn]);
        input->fadeDurationOut = string_to_double(argv[kDurationOut]);
        
        // get time type (ms, sec, samples)
        input->timeTypeIn = (TimeType)0;
        for (int i = 1; i < NumTimeTypes; i++)
        {
            if (strcmp(argv[kTimeTypeIn], timeTypes[i]) == 0)
            {
                input->timeTypeIn = (TimeType)i;
                break;
            }
        }
        
        input->timeTypeOut = (TimeType)0;
        for (int i = 1; i < NumTimeTypes; i++)
        {
            if (strcmp(argv[kTimeTypeOut], timeTypes[i]) == 0)
            {
                input->timeTypeOut = (TimeType)i;
                break;
            }
        }
        
        // set fade table
        input->fadeIn = Curves[0];
        for (size_t i = 1; i < nCurveTypes; i++)
        {
            if (strcmp(argv[kCurveTypeIn], curveNames[i]) == 0)
            {
                input->fadeIn = Curves[i];
                break;
            }
        }
        
        input->fadeOut = Curves[0];
        for (size_t i = 1; i < nCurveTypes; i++)
        {
            if (strcmp(argv[kCurveTypeOut], curveNames[i]) == 0)
            {
                input->fadeOut = Curves[i];
                break;
            }
        }
        
        return input;
    }
    
    bool fade_in_and_out_process(const char* pathin, const char* pathout, void* args)
    {
        bool success = false;
        if (args == 0) return success;
        Input* input = (Input*)args;
        
        // setup soundfile in
        SF_INFO  sfinfo = setup_sfinfo();
        SNDFILE *filein = sf_open(pathin, SFM_READ, &sfinfo), *fileout = 0;
        if (filein == 0) return success;
        
        // get the fade duration in samples
        to_samples(input->fadeDurationIn, input->timeTypeIn, sfinfo.samplerate);
        clamp_double(input->fadeDurationIn, 0., sfinfo.frames - 1);
        
        to_samples(input->fadeDurationOut, input->timeTypeOut, sfinfo.samplerate);
        clamp_double(input->fadeDurationOut, 0., sfinfo.frames - 1);
        const double startFadeout = sfinfo.frames - (input->fadeDurationOut + 1.);
        
        // setup audiosample buffer
        const size_t nChannels = sfinfo.channels;
        const size_t buffersize = TQ_BUFFERSIZE * nChannels;
        double* buffer = 0;
        char* unique_pathout = 0;
        if ((buffer = (double*)calloc(buffersize, sizeof(double))) != 0 &&
            (unique_pathout = unique_path(pathout)) != 0 &&
            (fileout = sf_open(unique_pathout, SFM_WRITE, &sfinfo)) != 0)
        {
            // do dsp
            double framesread = 0.;
            size_t samplesread;
            while ((samplesread = sf_read_double(filein, buffer, buffersize)) != 0)
            {
                for (size_t sample = 0; sample < samplesread; framesread += 1.)
                {
                    const double fadeOutBy = (framesread > startFadeout)
                    ? (1. - input->fadeOut((framesread - startFadeout) / input->fadeDurationOut))
                    : 1.;
                    
                    const double fadeInBy = (framesread < input->fadeDurationIn)
                    ? input->fadeIn(framesread / input->fadeDurationIn)
                    : 1.;
                    
                    for (size_t channel = 0; channel < nChannels; channel++, sample++)
                    {
                        buffer[sample] *= fadeInBy * fadeOutBy;
                    }
                }
                
                sf_write_double(fileout, buffer, samplesread);
            }
            
            success = true;
        }
        
        // clean up
        sf_close(filein);
        sf_close(fileout);
        free(buffer);
        free(unique_pathout);
        
        return success;
    }
    
#ifdef __cplusplus
}
#endif // def __cplusplus
