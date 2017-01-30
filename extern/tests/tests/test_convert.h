#include "tweaqapi.h"
#include <samplerate.h>

#ifdef __cplusplus
extern "C"
{
#endif // def __cplusplus
    
    enum ParameterFields{
        kSampleRate = 0,
        kNumParameters
    };
    
    struct Input {
        double sampleRate;
    };
    
    static const char* description = "convert sample rate";
    static const char* labels[] = {"22050, 44100, 48000"};
    
    void convert_setup(const int fieldNumber, Parameter& p)
    {
        switch (fieldNumber)
        {
            case kSampleRate:
                set_parameter_labels(p, 3, labels);
                set_parameter_description(p, description);
                break;
            default: break;
        }
    }
    
    void* convert_handleInput(int argc, const char* argv[])
    {
        if (argc != kNumParameters) return 0;
        
        Input* arg = (Input*)malloc(sizeof(Input));
        if (arg == 0) return arg;
        
        arg->sampleRate = string_to_double(argv[kSampleRate]);
        
        return arg;
    }
    
    bool convert_process(const char* pathin, const char* pathout, void* args)
    {
        if (args == 0) return false;
        Input* input = (Input*)args;
        
        // open file to read
        SF_INFO  sfinfo  = setup_sfinfo();
        SNDFILE* filein  = setup_filein(pathin, &sfinfo);
        if (filein == 0) return false;
        
        // setup read buffer (size must be a power of 2 * number of channels)
        const int buffersize = TQ_BUFFERSIZE * sfinfo.channels;
        double* buffer = (double*)calloc(buffersize, sizeof(double));
        if (buffer == 0)
        {   // allocation failed, clean up and quit
            sf_close(filein);
            return false;
        }
        
        // open file to write to (this should happen as late as possible, to
        // avoid leaving a bunch of empty files on our system if things go bad.
        SNDFILE* fileout = setup_fileout(pathout, &sfinfo);
        if (fileout == 0)
        {   // open failed, clean up and quit
            sf_close(filein);
            free(buffer);
            return false;
        }
        
        // dsp
        size_t samplesread;
        while ((samplesread = sf_read_double(filein, buffer, buffersize)) != 0)
        {   // process the read buffer and write it to disk
            for (size_t i = 0; i < samplesread; ++i)
            {
                buffer[i] *= 1.;
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
