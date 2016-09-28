#include "m_tweaq.h"

#ifdef __cplusplus
extern "C"
{
#endif // def __cplusplus
    
    enum {
        kPathIn = 0,
        kPathOut,
        kDuration,
        kDurationLabel,
        kFadeType,
        kNumParameters
    };
    
    struct t_args {
        const char* pathin;
        const char* pathout;
        t_uint      durationFrames;
        const char* durationLabel;
        const char* fadeType;
    };
    
    bool fade_out_setup(const t_uint index, t_parameter& p)
    {
        switch (index)
        {
            case kDuration:
                setParameterName(p, "fade out for");
                setParameterLabels(p, 3, "milliseconds", "seconds", "samples");
                setParameterDefault(p, 100.0);
                return true;
            case kFadeType:
                setParameterName(p, "type of fade");
                setParameterLabels(p, 3, "equal power", "equal gain", "S curve");
                return true;
            default:
                return false;
        }
    }
    
    bool fade_out_process(const t_uint argc, const char* argv[])
    {
        // check and get parameter list
        if (argc != kNumParameters)
        {
            return false;
        }
        
        t_args arg;
        arg.pathin          = argv[kPathIn];
        arg.pathout         = argv[kPathOut];
        arg.durationFrames  = stringToDouble(argv[kDuration]);
        arg.durationLabel   = argv[kDurationLabel];
        arg.fadeType        = argv[kFadeType];
        // right now, arg.durationFrames is not necessarily
        // a number of frames. we'll fix that later after opening
        // the input file.
        
        // setup libsndfile stuff
        SF_INFO sfinfo;
        SNDFILE *filein, *fileout;
        memset(&sfinfo, 0, sizeof(sfinfo));
        
        // open input file
        if ((filein = sf_open(arg.pathin, SFM_READ, &sfinfo)) == nullptr)
        {
            return false;
        }
        
        // convert duration to samples
        if (strcmp(arg.durationLabel, "milliseconds") == 0)
        {
            arg.durationFrames *= sfinfo.samplerate / 1000;
        }
        else if (strcmp(arg.durationLabel, "seconds") == 0)
        {
            arg.durationFrames *= sfinfo.samplerate;
        }
        
        // make sure we're not fading past the end of the audio file
        const t_uint framesInFile = sfinfo.frames;
        if (arg.durationFrames > framesInFile)
        {
            arg.durationFrames = framesInFile;
        }
        
        // setup multichannel buffer
        t_uint nChannels = sfinfo.channels;
        const t_uint nChannelBufferSize = TQ_BUFFERSIZE * nChannels;
        t_float* buffer = nullptr;
        if ((buffer = (t_float*)calloc(nChannelBufferSize, sizeof(t_float))) == nullptr)
        {
            return false;
        }
        
        // make fade buffer
        t_float* fadeBuffer = nullptr;
        if ((fadeBuffer = (t_float*)calloc(arg.durationFrames, sizeof(t_float))) == nullptr)
        {
            return false;
        }
        
        // linear gain
        t_float rDuration = 1.0 / double(arg.durationFrames);
        for (t_uint i = 0; i < arg.durationFrames; ++i)
        {
            fadeBuffer[i] = (arg.durationFrames - double(i + 1)) * rDuration;
        }
        
        // linear power
        if (strcmp(arg.fadeType, "equal power") == 0)
        {
            for (t_uint i = 0; i < arg.durationFrames; ++i)
            {
                fadeBuffer[i] = sqrt(fadeBuffer[i]);
            }
        }
        // S curve
        else if (strcmp(arg.fadeType, "S curve") == 0)
        {
            for (t_uint i = 0; i < arg.durationFrames; ++i)
            {
                fadeBuffer[i] = -0.5 * cos(TQ_PI * fadeBuffer[i]) + 0.5;
            }
        }
        
        // open output file
        if ((fileout = sf_open(arg.pathout, SFM_WRITE, &sfinfo)) == nullptr)
        {
            return false;
        }
        
        // do DSP
        t_float fadeBy = 1.0;
        const t_uint firstFrameOfFade = framesInFile - arg.durationFrames;
        t_uint samplesread, framesFaded = 0, totalFramesRead = 0;
        while ((samplesread = sf_read_double(filein, buffer, nChannelBufferSize)) != 0)
        {
            // dsp
            for (t_uint sample = 0; sample < samplesread; ++totalFramesRead)
            {
                if (totalFramesRead >= firstFrameOfFade)
                {
                    fadeBy = fadeBuffer[framesFaded++];
                }
                
                for (t_uint channel = 0; channel < nChannels; ++channel, ++sample)
                {
                    buffer[sample] *= fadeBy;
                }
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
        if (fadeBuffer != nullptr)
        {
            free(fadeBuffer);
            fadeBuffer = nullptr;
        }
        
        return true;
    }
#ifdef __cplusplus
}
#endif // def __cplusplus
