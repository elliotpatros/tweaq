#include "change_gain.h"

static const char* description = "enter a value by which to change the volume.";
static const char* labels[] = {"dB.", "gain"};

#ifdef __cplusplus
extern "C"
{
#endif // def __cplusplus

    void change_gain_setup(const int fieldNumber, Parameter& p)
    {
        switch (fieldNumber)
        {
            case kGain:
                set_parameter_labels(p, 2, labels);
                set_parameter_default(p, "0.0");
                set_parameter_description(p, description);
                break;
            default: break;
        }
    }
    
    void* change_gain_handleInput(int argc, const char* argv[])
    {
        if (argc != kNumParameters) return 0;
        
        Input* arg = (Input*)malloc(sizeof(Input));
        if (arg == 0) return arg;
        
        arg->gain = string_to_double(argv[kGain]);
        if (strcmp(argv[kGainLabel], "dB.") == 0) dB_to_gain(arg->gain);
        
        return arg;
    }
    
    bool change_gain_process(const char* pathin, const char* pathout, void* args)
    {
        if (args == 0) return false;
        Input* input = (Input*)args;
        
        // open file to read
        SF_INFO  sfinfo = setup_sfinfo();
        SNDFILE *filein = sf_open(pathin, SFM_READ, &sfinfo), *fileout = 0;
        if (filein == 0) return false;
        
        // setup read buffer (size must be a multiple of the number of channels)
        const int buffersize = TQ_BUFFERSIZE * sfinfo.channels;
        double* buffer;
        if ((buffer = (double*)calloc(buffersize, sizeof(double))) != 0 &&
            (fileout = sf_open(pathout, SFM_WRITE, &sfinfo)) != 0)
        {
            // dsp
            size_t samplesread;
            while ((samplesread = sf_read_double(filein, buffer, buffersize)) != 0)
            {   // process the read buffer and write it to disk
                for (size_t i = 0; i < samplesread; ++i)
                {
                    buffer[i] *= input->gain;
                }
                
                sf_write_double(fileout, buffer, samplesread);
            }
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
