#include "mix_to_mono.h"

static const char* choices[] = {"no attenuation", "-3 dB.", "-3 dB. per channel", "÷ number of channels"};

double calculate_gain(Input* input, const size_t nChannels)
{
    switch (input->choice)
    {
        case kNone: return 1.;
        case kMinusThree: return gain_from_dB(-3.);
        case kThreePerChannel: return gain_from_dB(-3. * (double)nChannels);
        case kOneOverNumChannels: return 1. / (double)nChannels;
        default: return 1.;
    }
}

#ifdef __cplusplus
extern "C"
{
#endif // def __cplusplus
    
    void mix_to_mono_setup(const int fieldNumber, Parameter& p)
    {
        switch (fieldNumber)
        {
            case kAttenuationLabel:
                set_parameter_labels(p, kNumChoices, choices); // auto-normalize (slower)
                set_parameter_description(p, "set attenuation");
                break;
            default: break;
        }
    }
    
    void* mix_to_mono_handleInput(int argc, const char* argv[])
    {
        if (argc != kNumParameters) return 0;
        
        Input* input = (Input*)calloc(1, sizeof(Input));
        if (input == 0) return 0;
        
        // get attenuation type (default: no attenuation)
        for (int i = 1; i < kNumChoices; ++i)
            if (strcmp(argv[kAttenuationLabel], choices[i]) == 0)
            {
                input->choice = (ParameterChoice)i;
                return input;
            }
        
        return input;
    }
    
    bool mix_to_mono_process(const char* pathin, const char* pathout, void* args)
    {
        // parse arguments from user
        bool success = false;
        if (args == 0) return success;
        Input* input = (Input*)args;
        
        // setup libsndfile stuff
        SF_INFO  sfinfo = setup_sfinfo();
        SNDFILE *filein = sf_open(pathin, SFM_READ, &sfinfo), *fileout = 0;
        if (filein == 0) return success;
        
        // get number of input channels, attenuation gain, and sample counter
        const size_t nChannels = sfinfo.channels;
        sfinfo.channels = 1; // output is mono
        const double gain = calculate_gain(input, nChannels);
        size_t sample = 0;
        
        // setup buffers and pathout
        const size_t buffersize = TQ_BUFFERSIZE * nChannels;
        double bufferout[TQ_BUFFERSIZE];
        double* bufferin = (double*)malloc(buffersize * sizeof(double));
        char* unique_pathout = unique_path(pathout);
        if (bufferin == 0 || unique_pathout == 0) goto clean_up;
        
        // open file to write
        if ((fileout = sf_open(unique_pathout, SFM_WRITE, &sfinfo)) == 0)
            goto clean_up;

        // dsp (mix to mono)
        size_t samplesread, frame;
        while ((samplesread = sf_read_double(filein, bufferin, buffersize)) != 0)
        {
            for (frame = 0, sample = 0; sample < samplesread; sample = ++frame * nChannels)
            {
                bufferout[frame] = bufferin[sample];
                for (size_t channel = 1; channel < nChannels; channel++)
                    bufferout[frame] += bufferin[sample + channel];
                
                bufferout[frame] *= gain;
            }
            
            if (sf_write_double(fileout, bufferout, frame) != (sf_count_t)frame)
                goto clean_up;
        }
        
        // done
        success = true;
    clean_up:
        sf_close(filein);
        sf_close(fileout);
        free(bufferin);
        free(unique_pathout);
        
        return success;
    }
    
    
#ifdef __cplusplus
}
#endif // def__cplusplus
