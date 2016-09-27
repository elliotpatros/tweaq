#include "m_tweaq.h"

void insertChannelNumberInPath(const char* input, const char* insert, char*& output)
{
    if ((input == nullptr) || (insert == nullptr))
    {
        output = nullptr; // fuck up the result and leave
        return;
    }
    
    // find index to separate input
    const t_uint inputLen = strlen(input), insertLen = strlen(insert);
    t_int sepidx = 0, i = inputLen;
    while (--i > 0)
    {
        if (input[i] == '.')
        {
            sepidx = i;
            break;
        }
    }
    
    // allocate memory in output if output == nullptr
    const t_uint memNeeded = inputLen + insertLen + 1;
    if (output == nullptr)
    {
        output = (char*)calloc(memNeeded, sizeof(char));
    }
    if ((output == nullptr) || (strlen(output) >= memNeeded))
    {
        output = nullptr; // fuck up the result and leave
        return;
    }
    
    // copy first part of input to output
    if (strncpy(output, input, sepidx) == nullptr)
    {
        output = nullptr; // fuck up the result and leave
        return;
    }
    
    // append toInsert to output
    if (strcat(output, insert) == nullptr)
    {
        output = nullptr; // fuck up the result and leave
        return;
    }
    
    // append the rest of input to output
    if (strcat(output, input + sepidx) == nullptr)
    {
        output = nullptr; // fuck up the result and leave
        return;
    }
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
        SNDFILE *filein = nullptr, *fileout = nullptr;
        memset(&sfinfo, 0, sizeof(sfinfo));
        
        // open source sound file
        if ((filein = sf_open(arg.pathin, SFM_READ, &sfinfo)) == nullptr)
        {
            return false;
        }
        
        // get pathin's number of channels
        const t_uint nChannels = sfinfo.channels;
        
        // setup input (multichannel) buffer
        const t_uint multiChannelBufferSize = TQ_BUFFERSIZE * nChannels;
        t_float* bufferin = nullptr;
        if ((bufferin = (t_float*)calloc(multiChannelBufferSize, sizeof(t_float))) == nullptr)
        {
            return false;
        }
        
        // setup output (mono) buffer
        t_float bufferout[TQ_BUFFERSIZE] = {0};
        
        // do DSP
        for (t_uint channel = 0; channel < nChannels; ++channel)
        {
            // STEP 0
            // reopen the input file
            if (filein == nullptr)
            {
                memset(&sfinfo, 0, sizeof(sfinfo)); // reset meta data struct
                if ((filein = sf_open(arg.pathin, SFM_READ, &sfinfo)) == nullptr)
                {
                    return false;
                }
            }
            
            // setup pathout to be mono
            sfinfo.channels = 1;
            
            // STEP 1
            // make channel number string
            char suffix[8] = {'\0'};
            sprintf(suffix, "-ch%02llu", channel + 1);
            
            // make the array for the new pathout, by setting this to NULL, we are asking
            // insertChannelNumberInPath() to allocate enough memory for us
            char* newPathout = nullptr;
            if (nChannels > 1)
            {
                insertChannelNumberInPath(arg.pathout, suffix, newPathout);
                if (newPathout == nullptr)
                {
                    return false;
                }
            }
            else
            {
                if ((newPathout = (char*)calloc(strlen(arg.pathout) + 1, sizeof(char))) == nullptr)
                {
                    return false;
                }
                if (strcpy(newPathout, arg.pathout) == nullptr)
                {
                    return false;
                }
            }
            
            // let's create a shiney new output file
            if ((fileout = sf_open(newPathout, SFM_WRITE, &sfinfo)) == nullptr)
            {
                return false;
            }
            
            // STEP 2
            // copy and deinterleave
            t_uint samplesread;
            while ((samplesread = sf_read_double(filein, bufferin, multiChannelBufferSize)) != 0)
            {
                // read
                t_uint sampleout = 0;
                for (t_uint samplein = channel; samplein < samplesread; samplein += nChannels, ++sampleout)
                {
                    bufferout[sampleout] = bufferin[samplein];
                }
                
                // write (samplesread will always be an integer multiple of nChannels)
                sf_write_double(fileout, bufferout, samplesread / nChannels);
            }
            
            // clean up and get ready for the next channel
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
        
        return true;
    }

#ifdef __cplusplus
}
#endif // def __cplusplus
