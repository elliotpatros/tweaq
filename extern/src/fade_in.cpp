#include "fade_in.h"

static const char*  timeTypes[] = {"milliseconds", "seconds", "samples"};
static const char* curveNames[] = {"logarithmic", "exponential", "linear", "'S' curve"};
static const CurveType Curves[] = {curve_log, curve_exp, curve_lin, curve_s};
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
    
    void fade_in_setup(const int fieldNumber, Parameter& p)
    {
        switch (fieldNumber)
        {
            case kDuration:
                set_parameter_description(p, "fade in for how long?");
                set_parameter_default(p, "250.0");
                set_parameter_labels(p, NumTimeTypes, timeTypes);
                break;
            case kCurveType:
                set_parameter_description(p, "what kind of curve should the fade in have?");
                set_parameter_labels(p, nCurveTypes, curveNames);
                break;
            default: break;
        }
    }
    
    void* fade_in_handleInput(int argc, const char* argv[])
    {
        if (argc != kNumParameters) return 0;
        
        Input* input = (Input*)calloc(1, sizeof(Input));
        if (input == 0) return 0;
        
        // get duration as double
        input->fadeDuration = string_to_double(argv[kDuration]);
        
        // get time type (ms, sec, samples)
        for (int i = 1; i < NumTimeTypes; i++)
            if (strcmp(argv[kTimeType], timeTypes[i]) == 0)
            {
                input->timeType = (TimeType)i;
                break;
            }
        
        // set fade function
        input->fade = Curves[0];
        for (size_t i = 1; i < nCurveTypes; i++)
            if (strcmp(argv[kCurveType], curveNames[i]) == 0)
            {
                input->fade = Curves[i];
                break;
            }
        
        return input;
    }
    
    bool fade_in_process(const char* pathin, const char* pathout, void* args)
    {
        // parse arguments from user
        bool success = false;
        if (args == 0) return success;
        Input* input = (Input*)args;
        
        // setup soundfile in
        SF_INFO  sfinfo = setup_sfinfo();
        SNDFILE *filein = sf_open(pathin, SFM_READ, &sfinfo), *fileout = 0;
        if (filein == 0) return success;
        
        // convert fade in duration to samples, and setup duration counter
        to_samples(input->fadeDuration, input->timeType, sfinfo.samplerate);
        clamp_double(input->fadeDuration, 0, sfinfo.frames - 1);
        
        // setup counter for current frame
        double framesread = 0.;
        
        // setup audiosample buffer
        const size_t nChannels = sfinfo.channels;
        const size_t buffersize = TQ_BUFFERSIZE * nChannels;
        double* buffer = (double*)malloc(buffersize * sizeof(double));
        char* unique_pathout = unique_path(pathout);
        if (buffer == 0 || unique_pathout == 0) goto clean_up;
        
        // open file to write
        if ((fileout = sf_open(unique_pathout, SFM_WRITE, &sfinfo)) == 0)
            goto clean_up;

        // dsp (fade in)
        size_t samplesread;
        while ((samplesread = sf_read_double(filein, buffer, buffersize)) != 0)
        {
            for (size_t sample = 0; sample < samplesread; framesread += 1.)
            {
                const double fadein = (framesread < input->fadeDuration)
                ? input->fade(framesread / input->fadeDuration)
                : 1.;
                
                for (size_t channel = 0; channel < nChannels; channel++, sample++)
                    buffer[sample] *= fadein;
            }
            
            if (sf_write_double(fileout, buffer, samplesread) != (sf_count_t)samplesread)
                goto clean_up;
        }
        
        // done
        success = true;
    clean_up:
        sf_close(filein);
        sf_close(fileout);
        free(buffer);
        free(unique_pathout);
        
        return success;
    }
    
#ifdef __cplusplus
}
#endif // def __cplusplus
