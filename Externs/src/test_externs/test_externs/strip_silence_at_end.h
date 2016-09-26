#ifndef test_externs_strip_silence_at_end_h
#define test_externs_strip_silence_at_end_h

#include "m_tweaq.h"

#define BUFFERSIZE 4096

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
    
bool strip_silence_at_end_setup(const t_uint index, t_parameter& p)
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

bool strip_silence_at_end_process(const t_int argc, const char* argv[])
{
    // check and get parameter list
    if (argc != kNumParameters)
    {
        return false;
    }
    
    t_args arg;
    arg.pathin = argv[kPathIn];
    arg.pathout = argv[kPathOut];
    arg.thresholdLabel = argv[kThresholdLabel];
    arg.threshold = stringToDouble(argv[kThreshold]);
    if (strcmp(arg.thresholdLabel, "dB.") == 0)
    {
        arg.threshold = dBToGain(arg.threshold);
    }
    
    // setup sndfile stuff
    SF_INFO sfinfo;
    SNDFILE *filein, *fileout;
    memset(&sfinfo, 0, sizeof(sfinfo));
    
    // open input file
    if ((filein = sf_open(arg.pathin, SFM_READ, &sfinfo)) == nullptr)
    {
        return false;
    }
    
    // setup input buffer so that we can...
    const t_uint nChannels = sfinfo.channels;
    const t_uint multichannelBufferSize = BUFFERSIZE * nChannels;
    t_float* buffer;
    if ((buffer = (t_float*)calloc(multichannelBufferSize, sizeof(t_float))) == nullptr)
    {
        return false;
    }
    
    // ...find the last frame that's loud enough
    t_uint lastLoudEnoughSample = 0,
    totalSamplesRead = 0,
    nSamplesRead = 0;
    while ((nSamplesRead = sf_read_double(filein, buffer, multichannelBufferSize)) != 0)
    {
        for (t_uint sample = 0; sample < nSamplesRead; ++sample, ++totalSamplesRead)
        {
            if (fabs(buffer[sample]) >= arg.threshold)
            {
                lastLoudEnoughSample = totalSamplesRead;
            }
        }
    }
    
    // clean up
    sf_close(filein);
    memset(&sfinfo, 0, sizeof(sfinfo));
    
    // copy input to output until the last frame that's loud enough
    // open input and output files
    if ((filein = sf_open(arg.pathin, SFM_READ, &sfinfo)) == nullptr)
    {
        return false;
    }
    if ((fileout = sf_open(arg.pathout, SFM_WRITE, &sfinfo)) == nullptr)
    {
        return false;
    }
    
    // do DSP
    totalSamplesRead = 0;
    while ((nSamplesRead = sf_read_double(filein, buffer, multichannelBufferSize)) != 0)
    {
        // write
        if ((totalSamplesRead += nSamplesRead) <= lastLoudEnoughSample)
        {
            sf_write_double(fileout, buffer, nSamplesRead);
        }
        else
        {
            t_uint nSamplesToWrite = ((lastLoudEnoughSample % multichannelBufferSize) / nChannels + 1) * nChannels;
            if (nSamplesToWrite > nSamplesRead)
            {
                nSamplesToWrite = nSamplesRead;
            }
            
            sf_write_double(fileout, buffer, nSamplesToWrite);
            break;
        }
    }
    
    // clean up
    sf_close(filein);
    sf_close(fileout);
    free(buffer);
    
    return true;
}
#ifdef __cplusplus
}
#endif // def __cplusplus

#endif // END TEST
