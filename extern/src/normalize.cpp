#include "tweaqapi.h"

#ifdef __cplusplus
extern "C"
{
#endif // def __cplusplus
    
    enum {
        kGain = 0,
        kGainLabel,
        kNumParameters
    };
    
    struct Input {
        double gain;
    };
    
    void normalize_setup(const int fieldNumber, Parameter& p)
    {
        switch (fieldNumber)
        {
            case kGain:
                set_parameter_labels(p, 2, "dB.", "gain");
                set_parameter_description(p, "normalize to a peak volume");
                set_parameter_default(p, "0.0");
                break;
            default: break;
        }
    }
    
    void* normalize_handleInput(int argc, const char* argv[])
    {
        if (argc != kNumParameters) return 0;
        
        Input* arg = (Input*)malloc(sizeof(Input));
        if (arg == 0) return arg;
        
        arg->gain = string_to_double(argv[kGain]);
        if (strcmp(argv[kGainLabel], "dB.") == 0) dB_to_gain(arg->gain);
        
        return arg;
    }
    
    bool normalize_process(const char* pathin, const char* pathout, void* args)
    {
        if (args == 0) return false;
        Input* input = (Input*)args;
        const double gain = input->gain / get_max_gain(pathin);
        
        // setup libsndfile stuff
        SF_INFO sfinfo  = setup_sfinfo();
        SNDFILE* filein = setupFilein(pathin, &sfinfo);
        if (filein == 0) return false;
        
        // setup soundfile buffer
        const size_t nChannels = sfinfo.channels;
        const size_t buffersize = TQ_BUFFERSIZE * nChannels;
        double* buffer = setupAudioBuffer(buffersize);
        if (buffer == 0)
        {
            sf_close(filein);
            return false;
        }
        
        // setup soundfile to write
        SNDFILE* fileout = setupFileout(pathout, &sfinfo);
        if (fileout  == 0)
        {
            sf_close(filein);
            free(buffer);
            return false;
        }
        
        // do dsp
        size_t samplesread;
        while ((samplesread = sf_read_double(filein, buffer, buffersize)) != 0)
        {
            for (size_t i = 0; i < samplesread; i++)
            {
                buffer[i] *= gain;
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
