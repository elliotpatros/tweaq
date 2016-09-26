#include "m_tweaq.h"

#define BUFFERSIZE 4096

t_uint splitPathNameAtExtension(const char* path,
                                char** pathPre,
                                char** pathPost)
{
    // find index of extension separator
    const t_uint nPathoutChars = strlen(path);
    t_uint sepidx = 0;
    for (t_uint i = nPathoutChars; i >= 1; --i)
    {
        if (path[i] == '.')
        {
            sepidx = i;
            break;
        }
    }
    
    // allocate memory for pre and post strings
    const t_uint preSize = sepidx;
    const t_uint postSize = nPathoutChars - sepidx;
    *pathPre = (char*)calloc(preSize, sizeof(char));
    *pathPost = (char*)calloc(postSize, sizeof(char));
    
    // copy results
    strncpy(*pathPre, path, preSize * sizeof(char));
    strncpy(*pathPost, path + sepidx, postSize * sizeof(char));
    
    return nPathoutChars;
}

#ifdef __cplusplus
extern "C"
{
#endif // def __cplusplus
    
    enum {
        kPathIn = 0,
        kPathOut,
        kNumParameters
    };
    
    struct t_args {
        const char* pathin;
        const char* pathout;
    };
    
    bool split_to_mono_setup(const t_uint index, t_parameter& p)
    {
        return false; // no parameters
    }
    
    bool split_to_mono_process(const t_uint argc, const char* argv[])
    {
        // check and get parameter list
        if (argc != kNumParameters)
        {
            return false;
        }
        
        t_args arg;
        arg.pathin  = argv[kPathIn];
        arg.pathout = argv[kPathOut];
        
        // setup libsoundfile stuff
        SF_INFO sfinfo;
        SNDFILE *filein(nullptr), *fileout(nullptr);
        memset(&sfinfo, 0, sizeof(sfinfo));
        
        // open source sound file
        if ((filein = sf_open(arg.pathin, SFM_READ, &sfinfo)) == nullptr)
        {
            return false;
        }
        
        // get pathin's number of channels
        const t_uint nChannels = sfinfo.channels;
        
        // setup input (multichannel) and output (mono) buffers
        const t_uint multiChannelBufferSize = BUFFERSIZE * nChannels;
        t_float* bufferin;
        if ((bufferin = (t_float*)calloc(multiChannelBufferSize, sizeof(t_float))) == nullptr)
        {
            return false;
        }
        
        t_float bufferout[BUFFERSIZE] = {0};
        
        // split pathout at index of last file separator
        // we're going to append the file name with the channel number that it was
        char *pathPre, *pathPost;
        const t_uint nPathoutChars =
        splitPathNameAtExtension(arg.pathout, &pathPre, &pathPost);
        
        // do DSP
        for (t_uint channel = 0; channel < nChannels; ++channel)
        {
            // start reading from the beginning of the file
            if (filein == nullptr)
            {
                if ((filein = sf_open(arg.pathin, SFM_READ, &sfinfo)) == nullptr)
                {
                    return false;
                }
            }
            
            // make pathout ready for mono
            sfinfo.channels = 1;
            
            // make file name suffix
            char suffix[16] = {'\0'};
            sprintf(suffix, "-ch%02llu", channel + 1);
            const t_uint nSuffixChars = strlen(suffix);
            
            // rename pathout ('dir'/'name'-ch.'ext')
            char* newPathout;
            newPathout = (char*)calloc(nPathoutChars + nSuffixChars, sizeof(char));
            sprintf(newPathout, "%s%s%s", pathPre, suffix, pathPost);
            
            // open destination sound file
            if ((fileout = sf_open(newPathout, SFM_WRITE, &sfinfo)) == nullptr)
            {
                return false;
            }
            
            // copy and deinterleave incoming sound file
            t_uint samplesread;
            while ((samplesread = sf_read_double(filein, bufferin, multiChannelBufferSize)) != 0)
            {
                // read
                for (t_uint sample = channel; sample < samplesread; sample += nChannels)
                {
                    bufferout[(sample - channel) / nChannels] = bufferin[sample];
                }
                
                // write (samples read will always be an integer multiple of nChannels)
                sf_write_double(fileout, bufferout, samplesread / nChannels);
            }
            
            // clean up new name
            sf_close(filein);
            sf_close(fileout);
            filein =
            fileout = nullptr;
            
            if (newPathout != nullptr)
            {
                free(newPathout);
                newPathout = nullptr;
            }
        }
        
        // clean up
        free(bufferin);
        if (pathPre != nullptr)
        {
            free(pathPre);
            pathPre = nullptr;
        }
        if (pathPost != nullptr)
        {
            free(pathPost);
            pathPost = nullptr;
        }
        
        return true;
    }
#ifdef __cplusplus
}
#endif // def __cplusplus
