#include "normalize.h"

static const char* labels[] = {"dB.", "gain"};

#ifdef __cplusplus
extern "C"
{
#endif // def __cplusplus
    
    void normalize_setup(const int fieldNumber, Parameter& p)
    {
        switch (fieldNumber)
        {
            case kGain:
                set_parameter_labels(p, 2, labels);
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
        bool success = false;
        if (args == 0) return success;
        Input* input = (Input*)args;
        const double gain = input->gain / get_max_gain(pathin);
        
        // setup libsndfile stuff
        SF_INFO sfinfo  = setup_sfinfo();
        SNDFILE *filein = sf_open(pathin, SFM_READ, &sfinfo), *fileout = 0;
        if (filein == 0) return success;
        
        // setup soundfile buffer
        const size_t buffersize = TQ_BUFFERSIZE * sfinfo.channels;
        double* buffer = 0;
        char* unique_pathout = 0;
        if ((buffer = (double*)calloc(buffersize, sizeof(double))) != 0 &&
            (unique_pathout = unique_path(pathout)) != 0 &&
            (fileout = sf_open(unique_pathout, SFM_WRITE, &sfinfo)) != 0)
        {
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
