#ifndef test_externs_mix_to_mono_h
#define test_externs_mix_to_mono_h // START TEST

#include "m_tweaq.h"

#define BUFFERSIZE 4096

#ifdef __cplusplus
extern "C"
{
#endif // def __cplusplus
    
enum {
    kPathIn = 0,
    kPathOut,
    kAttenuation,
    kAttenuationLabel,
    kNumParameters
};

struct t_args {
    const char* pathin;
    const char* pathout;
    const char* attenuationLabel;
    t_float attenuation;
};

/*
 * setup any parameters that you need from tweaq to do _process.
 * the first two parameters, pathin and pathout, are default.
 */
bool mix_to_mono_setup(const t_uint index, t_parameter* p)
{
    switch (index)
    {
        case kAttenuation:
            return setParameterName(p, "attenuate")
            && setParameterLabels(p, 2, "dB.", "gain")
            && setParameterDefault(p, -3.0);
        default:
            return false;
    }
}

bool mix_to_mono_process(const t_uint argc, const char* argv[])
{
    // check and get parameter list
    if (argc != kNumParameters)
    {
        return false;
    }

    t_args arg;
    arg.pathin           = argv[kPathIn];
    arg.pathout          = argv[kPathOut];
    arg.attenuationLabel = argv[kAttenuationLabel];
    arg.attenuation      = stringToDouble(argv[kAttenuation]);
    if (strcmp(arg.attenuationLabel, "dB.") == 0)
    {
        arg.attenuation = dBToGain(arg.attenuation);
    }
    
    
    // setup libsoundfile stuff
    SF_INFO sfinfo;
    SNDFILE *filein, *fileout;
    memset(&sfinfo, 0, sizeof(sfinfo));
    
    // open source sound file
    if ((filein = sf_open(arg.pathin, SFM_READ, &sfinfo)) == nullptr)
    {
        return false;
    }
    
    // get number of input channels (to setup the read buffer)
    t_float bufferout[BUFFERSIZE] = {0};
    const int nChannels = sfinfo.channels;
    const int multiChannelBufferSize = BUFFERSIZE * nChannels;
    t_float* bufferin;
    if ((bufferin = (t_float*)calloc(multiChannelBufferSize, sizeof(t_float))) == nullptr)
    {
        return false;
    }
    
    // open destination sound file (should probably be mono)
    sfinfo.channels = 1;
    if ((fileout = sf_open(arg.pathout, SFM_WRITE, &sfinfo)) == nullptr)
    {
        return false;
    }
    
    // do DSP
    t_uint samplesread;
    while ((samplesread = sf_read_double(filein, bufferin, multiChannelBufferSize)) != 0)
    {
        // read interleaved
        for (t_uint sample = 0; sample < samplesread; sample += nChannels)
        {
            const t_uint monoSample = sample / nChannels;
            bufferout[monoSample] = 0.0;
            
            for (t_uint channel = 0; channel < nChannels; ++channel)
            {
                bufferout[monoSample] += bufferin[monoSample * nChannels + channel];
            }
            
            bufferout[monoSample] *= arg.attenuation;
        }
        
        // write (samplesread will always be a multiple of nChannels)
        sf_write_double(fileout, bufferout, samplesread / nChannels);
    }
    
    // clean up
    sf_close(filein);
    sf_close(fileout);
    free(bufferin);
    
    return true;
}
#ifdef __cplusplus
}
#endif // def __cplusplus

#endif // END TEST
