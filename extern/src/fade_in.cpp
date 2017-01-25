#include "tweaqapi.h"

#ifdef __cplusplus
extern "C"
{
#endif // def __cplusplus
    
#define FADE_SIZE 16
    
    enum ParameterFields {
        kDuration = 0,
        kTimeType,
        kFadeType,
        kNumParameters
    };
    
    enum FadeType {
        kLogarithmic = 0,
        kInvLogarithmic,
        kLinear,
        kSCurve,
        kNumFadeTypes
    };
    
    enum TimeType {
        kMilliseconds = 0,
        kSeconds,
        kSamples,
        kNumTimeTypes
    };
    
    struct Input {
        double   fade[FADE_SIZE + 1];
        TimeType timeType;
        double   duration;
    };
    
    static const char* timeTypes[kNumTimeTypes] = {"milliseconds", "seconds", "samples"};
    static const char* fadeTypes[kNumFadeTypes] = {"logarithmic", "inverse log", "linear", "'S' curve"};
    
    size_t getFadeTableSize(Input* input, const SF_INFO sfinfo)
    {
        double nSamples;
        switch (input->timeType)
        {
            case kSamples:
                nSamples = input->duration;
                break;
            case kSeconds:
                nSamples = input->duration * sfinfo.samplerate;
                break;
            case kMilliseconds:
                nSamples = input->duration * sfinfo.samplerate * 0.001;
                break;
            default:
                nSamples = input->duration;
                break;
        }
        
        if      (nSamples < 2)             nSamples = 2;
        else if (nSamples > sfinfo.frames) nSamples = sfinfo.frames;
        return   nSamples - 1;
    }
    
    void fade_in_setup(const int fieldNumber, Parameter& p)
    {
        switch (fieldNumber)
        {
            case kDuration:
                set_parameter_description(p, "fade in for how long?");
                set_parameter_default(p, "250.0");
                set_parameter_labels(p, kNumTimeTypes, timeTypes[0], timeTypes[1], timeTypes[2]);
                break;
            case kFadeType:
                set_parameter_description(p, "what kind of curve should the fade in have?");
                set_parameter_labels(p, kNumFadeTypes, fadeTypes[0], fadeTypes[1], fadeTypes[2], fadeTypes[3]);
                break;
            default: break;
        }
    }
    
    void* fade_in_handleInput(int argc, const char* argv[])
    {
        if (argc != kNumParameters) return 0;
        
        Input* input = (Input*)malloc(sizeof(Input));
        if (input == 0) return input;
        
        // get duration as double
        input->duration = string_to_double(argv[kDuration]);
        
        // get time type (ms, sec, samples)
        input->timeType = (TimeType)0;
        for (int i = 1; i < kNumTimeTypes; i++)
        {
            if (strcmp(argv[kTimeType], timeTypes[i]) == 0)
            {
                input->timeType = (TimeType)i;
                break;
            }
        }
        
        // set fade table
        if (strcmp(argv[kFadeType], fadeTypes[kLogarithmic]) == 0)
        {   // logarithmic
            for (size_t i = 0; i <= FADE_SIZE; i++)
            {
                input->fade[i] = sqrt((double)i / (double)FADE_SIZE);
            }
        }
        else if (strcmp(argv[kFadeType], fadeTypes[kSCurve]) == 0)
        {   // 'S' curve
            for (size_t i = 0; i <= FADE_SIZE; i++)
            {
                input->fade[i] = -0.5 * cos(M_PI * (double)i / (double)FADE_SIZE) + 0.5;
            }
        }
        else if (strcmp(argv[kFadeType], fadeTypes[kInvLogarithmic]) == 0)
        {   // inverse logarithmic
            for (size_t i = 0; i <= FADE_SIZE; i++)
            {
                input->fade[i] = pow((double)i / (double)FADE_SIZE, 2.);
            }
        }
        else
        {   // linear curve
            for (size_t i = 0; i <= FADE_SIZE; i++)
            {
                input->fade[i] = (double)i / (double)FADE_SIZE;
            }
        }
        
        return input;
    }
    
    bool fade_in_process(const char* pathin, const char* pathout, void* args)
    {
        if (args == 0) return false;
        Input* input = (Input*)args;
        
        // setup soundfile in
        SF_INFO  sfinfo  = setup_sfinfo();
        SNDFILE* filein  = setupFilein (pathin, &sfinfo);
        if (filein == 0) return false;
        
        // get the fade duration in samples
        const size_t fadeTableSize = getFadeTableSize(input, sfinfo);
        double fadeRate = (double)FADE_SIZE / (double)fadeTableSize;
        
        // setup soundfile out
        SNDFILE* fileout = setupFileout(pathout, &sfinfo);
        if (fileout == 0)
        {
            sf_close(filein);
            return false;
        }
        
        // setup audiosample buffer
        const size_t nChannels = sfinfo.channels;
        const size_t buffersize = TQ_BUFFERSIZE * nChannels;
        double* buffer = setupAudioBuffer(buffersize);
        if (buffer == 0)
        {
            sf_close(filein);
            sf_close(fileout);
            return false;
        }
        
        // do dsp
        size_t samplesread, totalFramesRead = 0;
        while ((samplesread = sf_read_double(filein, buffer, buffersize)) != 0)
        {
            for (size_t sample = 0; sample < samplesread; totalFramesRead++)
            {
                double fadeBy = 1.;
                if (totalFramesRead < fadeTableSize)
                {
                    const double position = fadeRate * (double)totalFramesRead;
                    const double pointer = floor(position);
                    const double delta = position - pointer;
                    fadeBy = (1. - delta) * input->fade[(size_t)pointer] + delta * input->fade[(size_t)pointer + 1];
                }
                
                for (size_t channel = 0; channel < nChannels; channel++, sample++)
                {
                    buffer[sample] *= fadeBy;
                }
            }
            
            sf_write_double(fileout, buffer, samplesread);
        }
        
        // clean up
        sf_close(filein);
        sf_close(fileout);
        free(buffer);
        
        return true;
    }
    
#ifdef __cplusplus
}
#endif // def __cplusplus
