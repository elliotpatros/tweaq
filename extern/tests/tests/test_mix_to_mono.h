#include "tweaqapi.h"

#ifdef __cplusplus
extern "C"
{
#endif // def __cplusplus
    
    enum ParameterFields {
        kAttenuationLabel = 0,
        kNumParameters
    };
    
    enum ParameterChoice {
        kNone,
        kMinusThree,
        kThreePerChannel,
        kOneOverNumChannels,
        kNumChoices
    };
    
    struct Input {
        ParameterChoice choice;
    };
    
    static const char* choices[] = {"no attenuation", "-3 dB.", "-3 dB. per channel", "÷ number of channels"};
    
    void mix_to_mono_setup(const int fieldNumber, Parameter& p)
    {
        switch (fieldNumber)
        {
            case kAttenuationLabel:
                set_parameter_labels(p, kNumChoices, choices[0], choices[1], choices[2], choices[3]); // auto-normalize (slower)
                set_parameter_description(p, "set attenuation");
                break;
            default: break;
        }
    }
    
    void* mix_to_mono_handleInput(int argc, const char* argv[])
    {
        if (argc != kNumParameters) return 0;
        
        Input* arg = (Input*)malloc(sizeof(Input));
        if (arg == 0) return arg;
        
        arg->choice = (ParameterChoice)0;
        for (int i = 1; i < kNumChoices; ++i)
        {
            if (strcmp(argv[kAttenuationLabel], choices[i]) == 0)
            {
                arg->choice = (ParameterChoice)i;
                return arg;
            }
        }
        
        return arg;
    }
    
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
    
    bool mix_to_mono_process(const char* pathin, const char* pathout, void* args)
    {
        if (args == 0) return false;
        Input* input = (Input*)args;
        
        // setup libsndfile stuff
        SF_INFO sfinfo;
        SNDFILE *filein, *fileout;
        memset(&sfinfo, 0, sizeof(SF_INFO));
        
        // open read sound files
        if ((filein = sf_open(pathin, SFM_READ, &sfinfo)) == 0)
        {
            return false;
        }
        
        // get number of input channels
        const size_t nChannels = sfinfo.channels;
        const size_t read_buffersize = TQ_BUFFERSIZE * sfinfo.channels;
        const double gain = calculate_gain(input, nChannels);
        
        // open write sound file
        sfinfo.channels = 1; // output is mono
        if ((fileout = sf_open(pathout, SFM_WRITE, &sfinfo)) == 0)
        {
            sf_close(filein);
            return false;
        }
        
        // allocate buffers for input and output
        double bufferout[TQ_BUFFERSIZE];
        double *bufferin;
        if ((bufferin = (double*)calloc(read_buffersize, sizeof(double))) == 0)
        {
            sf_close(filein);
            sf_close(fileout);
            return false;
        }
        
        
        size_t samplesread;
        while ((samplesread = sf_read_double(filein, bufferin, read_buffersize)) != 0)
        {
            const size_t nFrames = samplesread / nChannels;
            for (size_t frame = 0; frame < nFrames; frame++)
            {
                const size_t sample = frame * nChannels;
                bufferout[frame] = bufferin[sample];
                
                for (size_t channel = 1; channel < nChannels; channel++)
                {
                    bufferout[frame] += bufferin[sample + channel];
                }
                
                bufferout[frame] *= gain;
            }
            
            sf_write_double(fileout, bufferout, nFrames);
        }
        
        // clean up
        sf_close(filein);
        sf_close(fileout);
        free(bufferin);
        
        return true;
    }
    
    
#ifdef __cplusplus
}
#endif // def__cplusplus