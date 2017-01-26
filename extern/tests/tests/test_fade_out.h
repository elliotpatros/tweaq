#include "tweaqapi.h"
#include "ExternalHelpers.h"

#ifdef __cplusplus
extern "C"
{
#endif // def __cplusplus
    
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
    
    static const char*  timeTypes[] = {"milliseconds", "seconds", "samples"};
    static const char* curveNames[] = {"logarithmic", "exponential", "linear", "'S' curve"};
    static const CurveType Curves[] = {curve_log, curve_exp, curve_lin, curve_s};
    static const size_t nCurveTypes = sizeof(Curves) / sizeof(CurveType);
    
    void fade_out_setup(const int fieldNumber, Parameter& p)
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
    
    void* fade_out_handleInput(int argc, const char* argv[])
    {
        if (argc != kNumParameters) return 0;
        
        Input* input = (Input*)malloc(sizeof(Input));
        if (input == 0) return input;
        
        // get duration as double
        input->fadeDuration = string_to_double(argv[kDuration]);
        
        // get time type (ms, sec, samples)
        input->timeType = (TimeType)0;
        for (int i = 1; i < NumTimeTypes; i++)
        {
            if (strcmp(argv[kTimeType], timeTypes[i]) == 0)
            {
                input->timeType = (TimeType)i;
                break;
            }
        }
        
        // set fade table
        input->fade = Curves[0];
        for (int i = 1; i < nCurveTypes; i++)
        {
            if (strcmp(argv[kCurveType], curveNames[i]) == 0)
            {
                input->fade = Curves[i];
                break;
            }
        }
        
        return input;
    }
    
    bool fade_out_process(const char* pathin, const char* pathout, void* args)
    {
        if (args == 0) return false;
        Input* input = (Input*)args;
        
        // setup soundfile in
        SF_INFO  sfinfo  = setup_sfinfo();
        SNDFILE* filein  = setup_filein(pathin, &sfinfo);
        if (filein == 0) return false;
        
        // get the fade duration in samples
        to_samples(input->fadeDuration, input->timeType, sfinfo.samplerate);
        clip_double(input->fadeDuration, 0., sfinfo.frames - 1);
        const double startFadeout = sfinfo.frames - (input->fadeDuration + 1.);
        std::cout << "fade duration = " << input->fadeDuration << std::endl;
        
        // setup audiosample buffer
        const size_t nChannels = sfinfo.channels;
        const size_t buffersize = TQ_BUFFERSIZE * nChannels;
        double* buffer = (double*)calloc(buffersize, sizeof(double));
        if (buffer == 0)
        {
            sf_close(filein);
            return false;
        }
        
        // setup soundfile out
        SNDFILE* fileout = setup_fileout(pathout, &sfinfo);
        if (fileout == 0)
        {
            sf_close(filein);
            free(buffer);
            return false;
        }
        
        // do dsp
        double framesread = 0.;
        size_t samplesread;
        while ((samplesread = sf_read_double(filein, buffer, buffersize)) != 0)
        {
            for (size_t sample = 0; sample < samplesread; framesread += 1.)
            {
                const double fadeBy = (framesread > startFadeout)
                    ? (1. - input->fade((framesread - startFadeout) / input->fadeDuration))
                    : 1.;
                
                for (size_t channel = 0; channel < nChannels; channel++, sample++)
                {
                    buffer[sample] *= fadeBy;
                }
                
                std::cout << "sample " << framesread << "... fade by " << fadeBy << std::endl;
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
