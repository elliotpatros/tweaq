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
        free_string(dest);
    }
}

char* get_pathout(const char* pathout, const size_t channel, const size_t nChannels)
{
    if (pathout == 0) return 0;
    
    char* channel_pathout = 0;
    if (nChannels > 1)
        insertChannelNumber(channel_pathout, pathout, channel);
    else
        set_string(channel_pathout, pathout);
    
    if (channel_pathout == 0) return 0;
    char* unique_pathout = unique_path(channel_pathout);
    free(channel_pathout);
    
    return unique_pathout;
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
        // no user arguments
        (void)args;
        bool success = false;
        
        // setup libsndfile stuff
        SF_INFO sfinfo  = setup_sfinfo();
        SNDFILE *filein = sf_open(pathin, SFM_READ, &sfinfo), *fileout = 0;
        if (filein == 0) return success;
        sf_close(filein);
        
        // get pathin's channel count
        const size_t nChannels = sfinfo.channels;
        
        // setup buffers and pathout names
        const size_t buffersize = TQ_BUFFERSIZE * nChannels;
        double bufferout[TQ_BUFFERSIZE];
        double* bufferin = (double*)malloc(buffersize * sizeof(double));
        char *unique_pathout = 0;
        if (bufferin == 0) goto clean_up;
        
        // do dsp
        for (size_t channel = 0; channel < nChannels; channel++)
        {
            // reopen input file
            sfinfo = setup_sfinfo();
            filein = sf_open(pathin, SFM_READ, &sfinfo);
            if (filein == 0) goto clean_up;
            
            // insert channel number in pathout
            if ((unique_pathout = get_pathout(pathout, channel, nChannels)) == 0)
                goto clean_up;
            
            // open nth channel to write
            sfinfo.channels = 1;
            if ((fileout = sf_open(unique_pathout, SFM_WRITE, &sfinfo)) == 0)
                goto clean_up;

            sf_seek(filein, 0, SEEK_SET);
            sf_seek(fileout, 0, SEEK_SET);
            
            // copy and deinterleave
            size_t samplesread, sampleswritten;
            while ((samplesread = sf_read_double(filein, bufferin, buffersize)) != 0)
            {
                sampleswritten = 0;
                for (size_t samplein = channel; samplein < samplesread; samplein += nChannels, sampleswritten++)
                    bufferout[sampleswritten] = bufferin[samplein];

                if (sf_write_double(fileout, bufferout, sampleswritten) != (sf_count_t)sampleswritten)
                    goto clean_up;
            }
            
            // flush file objects and clean up for next channel
            sf_close(filein);
            sf_close(fileout);
            free(unique_pathout);
            filein = fileout = 0;
            unique_pathout = 0;
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
#endif // def __cplusplus
