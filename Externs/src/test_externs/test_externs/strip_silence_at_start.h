#include "m_tweaq.h"

#ifdef __cplusplus
extern "C"
{
#endif // def __cplusplus
    
    enum {
        kPathIn = 0,
        kPathOut,
        kThreshold,
        kThresholdLabel,
        kNumParameters
    };
    
    struct t_args {
        const char* pathin;
        const char* pathout;
        const char* thresholdLabel;
        t_float threshold;
    };
    
    bool strip_silence_at_start_setup(const t_uint index, t_parameter& p)
    {
        switch (index)
        {
            case kThreshold:
                setParameterName(p, "threshold");
                setParameterLabels(p, 2, "dB.", "gain");
                setParameterDefault(p, -60.0);
                return true;
            default:
                return false;
        }
    }
    
    bool strip_silence_at_start_process(t_uint argc, const char* argv[])
    {
        // check and get parameter list
        if (argc != kNumParameters)
        {
            return false;
        }
        
        t_args arg;
        arg.pathin         = argv[kPathIn];
        arg.pathout        = argv[kPathOut];
        arg.thresholdLabel = argv[kThresholdLabel];
        arg.threshold      = stringToDouble(argv[kThreshold]);
        if (strcmp(arg.thresholdLabel, "dB.") == 0)
        {
            arg.threshold = dBToGain(arg.threshold);
        }
        
        // setup audio stuff
        SF_INFO sfinfo;
        SNDFILE *filein, *fileout;
        memset(&sfinfo, 0, sizeof(sfinfo));
        
        // open input and output files
        if ((filein = sf_open(arg.pathin, SFM_READ, &sfinfo)) == nullptr)
        {
            return false;
        }
        if ((fileout = sf_open(arg.pathout, SFM_WRITE, &sfinfo)) == nullptr)
        {
            return false;
        }
        
        // setup input and output buffers
        const t_uint nChannels = sfinfo.channels;
        const t_uint nChannelsMinusOne = nChannels - 1;
        const t_uint nChannelBufferSize = TQ_BUFFERSIZE * nChannels;
        t_float *bufferin = nullptr,
                *bufferout = nullptr;
        if ((bufferin = (t_float*)calloc(nChannelBufferSize, sizeof(t_float))) == nullptr)
        {
            return false;
        }
        if ((bufferout = (t_float*)calloc(nChannelBufferSize, sizeof(t_float))) == nullptr)
        {
            return false;
        }
        
        // do DSP
        t_uint outptr = 0;
        bool stillTooQuiet = true;
        t_uint samplesread;
        while ((samplesread = sf_read_double(filein, bufferin, nChannelBufferSize)) != 0)
        {
            // go through all the samples
            for (t_uint sample = 0; sample < samplesread; ++sample)
            {
                // check if the input has gotten loud enough yet
                if (stillTooQuiet)
                {
                    const t_uint startSample = sample;
                    for (t_uint channel = 0; channel < nChannels; ++channel)
                    {
                        if (fabs(bufferin[sample + channel]) >= arg.threshold)
                        {
                            // hooray! we've finally gotten loud enough :-)
                            // let everyone know, and read this frame again.
                            stillTooQuiet = false;
                            sample = startSample - 1;
                            break;
                        }
                    }
                    
                    // if this frame was too quiet, don't read it again
                    if (stillTooQuiet)
                    {
                        sample += nChannelsMinusOne;
                    }
                }
                else // if the input has ever been loud enough...
                {    // ...copy input to output
                    bufferout[outptr++] = bufferin[sample];
                    
                    // write when full...
                    if (outptr >= samplesread)
                    {
                        sf_write_double(fileout, bufferout, outptr);
                        
                        // reset output buffer pointer
                        outptr = 0;
                    }
                }
            }
        }
        
        // flush out any leftover samples
        if (outptr > 0)
        {
            sf_write_double(fileout, bufferout, outptr);
        }
        
        // clean up
        sf_close(filein);
        sf_close(fileout);
        if (bufferin != nullptr)
        {
            free(bufferin);
            bufferin = nullptr;
        }
        if (bufferout != nullptr)
        {
            free(bufferout);
            bufferout = nullptr;
        }
        
        
        return true;
    }
#ifdef __cplusplus
}
#endif // def __cplusplus
