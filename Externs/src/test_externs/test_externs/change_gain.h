#ifndef test_externs_change_gain_h
#define test_externs_change_gain_h // START TEST

#include "m_tweaq.h"

#define BUFFERSIZE 4096

#ifdef __cplusplus
extern "C"
{
#endif // def __cplusplus
    
enum {
    kPathIn = 0,
    kPathOut,
    kGain,
    kGainLabel,
    kNumParameters
};

struct t_args {
    const char* pathin;
    const char* pathout;
    const char* gainLabel;
    t_float gain;
};

/*
 * setup any parameters that you need from tweaq to do _process.
 * the first two parameters, pathin and pathout, are default.
 */
bool change_gain_setup(const t_uint index, t_parameter* p)
{
    switch (index)
    {
        case kGain:
            return setParameterName(p, "change volume")
            && setParameterLabels(p, 2, "dB.", "gain")
            && setParameterDefault(p, 0.0);
        default:
            return false;
    }
}

bool change_gain_process(t_int argc, const char* argv[])
{
    // check and get parameter list
    if (argc != kNumParameters)
    {
        return false;
    }
    
    t_args arg;
    arg.pathin    = argv[kPathIn];
    arg.pathout   = argv[kPathOut];
    arg.gainLabel = argv[kGainLabel];
    arg.gain      = stringToDouble(argv[kGain]);
    if (strcmp(arg.gainLabel, "dB.") == 0)
    {
        arg.gain = dBToGain(arg.gain);
    }
    
    // setup libsndfile stuff
    t_float buffer[BUFFERSIZE] = {0};
    SF_INFO sfinfo;
    SNDFILE *filein, *fileout;
    memset(&sfinfo, 0, sizeof(sfinfo));
    
    // open files to read and write
    if ((filein = sf_open(arg.pathin, SFM_READ, &sfinfo)) == nullptr)
    {
        return false;
    }
    if ((fileout = sf_open(arg.pathout, SFM_WRITE, &sfinfo)) == nullptr)
    {
        return false;
    }
    
    // do dsp
    t_uint samplesread;
    while ((samplesread = sf_read_double(filein, buffer, BUFFERSIZE)) != 0)
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
    
    return true;
}
#ifdef __cplusplus
}
#endif // def __cplusplus

#endif // END TEST
