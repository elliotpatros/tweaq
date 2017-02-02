#include "deinterleave.h"

void insertChannelNumber(char*& dest, const char* src, const size_t channel)
{
    size_t input_len = strlen(src);
    
    if (src == 0 ||
        dest != 0 ||
        channel > 99 ||
        (dest = (char*)calloc(input_len + strlen("-ch99") + 1, 1)) == 0)
    {
        return;
    }
    
    size_t insertFrom = input_len;
    while (insertFrom-- > 0) if (src[insertFrom] == '.') break;
    
    if (strncpy(dest, src, insertFrom) == 0 ||
        sprintf(dest + insertFrom, "-ch%02lu%s", channel + 1, src + insertFrom) < 0)
    {
        free(dest);
        dest = 0;
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
        bool success = false;
        
        // setup libsndfile stuff
        SF_INFO sfinfo  = setup_sfinfo();
        SNDFILE *filein = sf_open(pathin, SFM_READ, &sfinfo), *fileout = 0;
        if (filein == 0) return success;
        
        // get pathin's channel count
        const size_t nChannels = sfinfo.channels;
        
        // setup input buffer
        const size_t buffersize = TQ_BUFFERSIZE * nChannels;
        double* bufferin = (double*)calloc(buffersize, sizeof(double));
        if (bufferin == 0)
        {
            sf_close(filein);
            return success;
        }
        
        // setup output buffer
        double bufferout[TQ_BUFFERSIZE];
        
        // do dsp
        for (size_t channel = 0; channel < nChannels; channel++)
        {
            // reopen input file
            if (filein == 0 &&
               (memset(&sfinfo, 0, sizeof(SF_INFO))) != 0 &&
               (filein = sf_open(pathin, SFM_READ, &sfinfo)) == 0)
            {
                free(bufferin);
                return success;
            }
            
            // insert channel number in pathout
            char *mono_pathout = 0, *unique_mono_pathout = 0;
            if (nChannels > 1)
            {
                insertChannelNumber(mono_pathout, pathout, channel);
            }
            else
            {
                set_string(mono_pathout, pathout);
            }
            
            // write nth channel out
            sfinfo.channels = 1;
            if (mono_pathout != 0 &&
               (unique_mono_pathout = unique_path(mono_pathout)) != 0 &&
               (fileout = sf_open(unique_mono_pathout, SFM_WRITE, &sfinfo)) != 0)
            {
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
                
                success = channel == nChannels - 1;
            }
            
            // clean up
            sf_close(filein);
            sf_close(fileout);
            free(mono_pathout);
            free(unique_mono_pathout);
            filein = 0; // let the next loop know to reopen the filein
        }
        
        // clean up
        free(bufferin);
        
        return success;
    }
    
#ifdef __cplusplus
}
#endif // def __cplusplus
