#include "deinterleave.h"

void insertChannelNumber(const char* input, const size_t channel, char*& output)
{
    size_t input_len = strlen(input);
    
    if (input == 0 ||
        output != 0 ||
        channel > 99 ||
        (output = (char*)calloc(input_len + strlen("-ch99") + 1, 1)) == 0)
    {
        return;
    }
    
    size_t insertFrom = input_len;
    while (insertFrom-- > 0) if (input[insertFrom] == '.') break;
    
    if (strncpy(output, input, insertFrom) == 0 ||
        sprintf(output + insertFrom, "-ch%02lu%s", channel + 1, input + insertFrom) < 0)
    {
        free(output);
        output = 0;
        return;
    }
}

#ifdef __cplusplus
extern "C"
{
#endif // def __cplusplus
    
    void deinterleave_setup(const int fieldNumber, Parameter& p)
    {
        (void)fieldNumber;
        (void)p;
        return; // no parameters
    }
    
    void* deinterleave_handleInput(int argc, const char* argv[])
    {
        (void)argc;
        (void)argv;
        return 0; // no parameters
    }
    
    bool deinterleave_process(const char* pathin, const char* pathout, void* args)
    {
        (void)args;
        
        // setup libsndfile stuff
        SF_INFO sfinfo  = setup_sfinfo();
        SNDFILE* filein = sf_open(pathin, SFM_READ, &sfinfo);
        if (filein == 0) return false;
        
        // get pathin's channel count
        const size_t nChannels = sfinfo.channels;
        
        // setup input buffer
        const size_t buffersize = TQ_BUFFERSIZE * nChannels;
        double* bufferin = (double*)calloc(buffersize, sizeof(double));
        if (bufferin == 0)
        {
            sf_close(filein);
            return false;
        }
        
        // setup output buffer
        double bufferout[TQ_BUFFERSIZE];
        
        // do dsp
        for (size_t channel = 0; channel < nChannels; channel++)
        {
            // reopen input file
            if (filein == 0)
            {
                filein = sf_open(pathin, SFM_READ, &sfinfo);
                if (filein == 0)
                {
                    free(bufferin);
                    return false;
                }
            }
            
            // insert channel number in pathout
            char* mono_pathout = 0;
            if (nChannels > 1)
            {
                insertChannelNumber(pathout, channel, mono_pathout);
            }
            else
            {
                mono_pathout = (char*)calloc(strlen(pathout), sizeof(char));
                if (mono_pathout != 0) memcpy(mono_pathout, pathout, strlen(pathout) * sizeof(char));
            }
            
            if (mono_pathout == 0)
            {
                free(bufferin);
                sf_close(filein);
                return false;
            }
            
            // open output soundfile
            sfinfo.channels = 1;
            SNDFILE* fileout = sf_open(mono_pathout, SFM_WRITE, &sfinfo);
            if (fileout == 0)
            {
                free(bufferin);
                free(mono_pathout);
                sf_close(filein);
                return false;
            }
            
            // copy and deinterleave
            size_t samplesread;
            while ((samplesread = sf_read_double(filein, bufferin, buffersize)) != 0)
            {
                size_t sampleout = 0;
                for (size_t samplein = channel; samplein < samplesread; samplein += nChannels, sampleout++)
                {
                    bufferout[sampleout] = bufferin[samplein];
                }
                
                sf_write_double(fileout, bufferout, sampleout);
            }
            
            // clean up
            sf_close(filein);
            sf_close(fileout);
            free(mono_pathout);
            filein = 0;
        }
        
        // clean up
        free(bufferin);
        
        return true;
    }
    
#ifdef __cplusplus
}
#endif // def __cplusplus
