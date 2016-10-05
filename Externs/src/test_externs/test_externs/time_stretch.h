#include "m_tweaq.h"

#ifdef __cplusplus
extern "C"
{
#endif // def __cplusplus
    
    enum {
        kPathIn = 0,
        kPathOut,
        kStretchFactor,
        kStretchLabel,
        kNumParameters
    };
    
    struct t_args {
        const char* pathin;
        const char* pathout;
        const char* stretchLabel;
        t_float stretchFactor;
    };
    
    /*
     * setup any parameters that you need from tweaq to do _process.
     * the first two parameters, pathin and pathout, are default.
     */
    bool time_stretch_setup(const t_uint index, t_parameter& p)
    {
        switch (index)
        {
            case kStretchFactor:
                setParameterName(p, "scale time by");
                setParameterLabels(p, 4, "percent", "scale", "duration (mm:ss)", "by halfsteps");
                setParameterDefault(p, 100.0);
                return true;
            default:
                return false;
        }
    }
    
    bool time_stretch_process(const t_uint argc, const char* argv[])
    {
        // check and get parameter list
        if (argc != kNumParameters)
        {
            return false;
        }
        
        t_args arg;
        arg.pathin          = argv[kPathIn];
        arg.pathout         = argv[kPathOut];
        arg.stretchLabel    = argv[kStretchLabel];
        
        // convert stretchFactor argument to scale
        if (strcmp(arg.stretchLabel, "percent") == 0)
        {
            arg.stretchFactor = stringToDouble(arg.stretchLabel) / 100;
        }
        else if (strcmp(arg.stretchLabel, "by halfsteps") == 0)
        {
            
        }
        else if (strcmp(arg.stretchLabel, "duration (mm:ss)") == 0)
        {
            
        }
        else // "scale"
        {
            
        }
//        if (strcmp(arg.gainLabel, "dB.") == 0)
//        {
//            arg.gain = dBToGain(arg.gain);
//        }
        
        // setup libsndfile stuff
        SF_INFO sfinfo;
        SNDFILE *filein, *fileout;
        memset(&sfinfo, 0, sizeof(sfinfo));
        
        // open input file
        if ((filein = sf_open(arg.pathin, SFM_READ, &sfinfo)) == nullptr)
        {
            return false;
        }
        
        // setup multichannel buffer
        const t_uint nChannels = sfinfo.channels;
        const t_uint nChannelBufferSize = TQ_BUFFERSIZE * nChannels;
        t_float* buffer = nullptr;
        if ((buffer = (t_float*)calloc(nChannelBufferSize, sizeof(t_float))) == nullptr)
        {
            return false;
        }
        
        // find max gain
        t_float maxGain = 0.0;
        t_uint samplesread;
        while ((samplesread = sf_read_double(filein, buffer, nChannelBufferSize)) != 0)
        {
            for (t_uint i = 0; i < samplesread; ++i)
            {
                const t_float sampleGain = fabs(buffer[i]);
                if (sampleGain > maxGain)
                {
                    maxGain = sampleGain;
                }
            }
        }
        
        // close filein
        sf_close(filein);
        filein = nullptr;
        memset(&sfinfo, 0, sizeof(sfinfo));
        
        // fix up maxGain
        if (maxGain < 0.00000001)
        {
            maxGain = 1.0;
        }
        
        arg.gain /= maxGain;
        
        // reopen input file, and open output file
        if ((filein = sf_open(arg.pathin, SFM_READ, &sfinfo)) == nullptr)
        {
            return false;
        }
        if ((fileout = sf_open(arg.pathout, SFM_WRITE, &sfinfo)) == nullptr)
        {
            return false;
        }
        
        // do DSP
        while ((samplesread = sf_read_double(filein, buffer, nChannelBufferSize)) != 0)
        {
            // read
            for (t_uint i = 0; i < samplesread; ++i)
            {
                buffer[i] *= arg.gain;
            }
            
            // write
            sf_write_double(fileout, buffer, samplesread);
        }
        
        // clean up
        sf_close(filein);
        sf_close(fileout);
        if (buffer != nullptr)
        {
            free(buffer);
            buffer = nullptr;
        }
        
        return true;
    }
#ifdef __cplusplus
}
#endif // def __cplusplus
