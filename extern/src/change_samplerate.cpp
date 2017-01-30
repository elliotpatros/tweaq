#include "tweaqapi.h"
#include <samplerate.h>

// data
enum ParameterFields{
    kSampleRate = 0,
    kQuality,
    kNumParameters
};

struct Input {
    double sampleRate;
    int converter;
};

static const char* sr_labels[] = {"44100", "48000", "96000", "11025", "22050", "88200", "176400", "192000"};
static const char* sr_quality[] = {"sinc (best quality)", "sinc (medium quality)", "sinc (fast)", "sample/hold", "linear (fastest)"};

// apply gain
static double
apply_gain(float* data, long nFrames, int nChannels, double maxGain, double gain)
{
    const long nSamples = nFrames * nChannels;
    for (long i = 0; i < nSamples; i++)
    {
        data[i] *= gain;
        if (fabs(data[i]) > maxGain)
        {
            maxGain = fabs(data[i]);
        }
    }
    
    return maxGain;
}

// convert sample rate
static sf_count_t
do_convertion(SNDFILE* filein, SNDFILE* fileout, int converter, double src_ratio, int nChannels, double& gain, bool normalize)
{
    // setup read and write buffers
    float bufferin[TQ_BUFFERSIZE];
    float bufferout[TQ_BUFFERSIZE];
    
    // setup converter
    SRC_STATE* src_state;
    SRC_DATA   src_data;
    int        error = SF_ERR_NO_ERROR;
    double     maxGainOut = 0.0;
    sf_count_t nFramesOut = 0;
    sf_count_t nFramesIn = TQ_BUFFERSIZE / nChannels;
    
    sf_seek(filein, 0, SEEK_SET);
    sf_seek(fileout, 0, SEEK_SET);
    
    // initialize the sample rate converter
    if ((src_state = src_new(converter, nChannels, &error)) == 0)
    {
        src_delete(src_state);
        return 0;
    }
    
    src_data.end_of_input = 0;
    src_data.input_frames = 0;
    src_data.data_in = bufferin;
    src_data.data_out = bufferout;
    src_data.src_ratio = src_ratio;
    src_data.output_frames = nFramesIn;
    
    // do convertion
    while (true)
    {
        // if the input buffer is empty, refill it.
        if (src_data.input_frames == 0)
        {
            src_data.input_frames = sf_readf_float(filein, bufferin, nFramesIn);
            src_data.data_in = bufferin;
            
            // the last read will not fill the buffer, so check on that
            if (src_data.input_frames < nFramesIn)
            {
                src_data.end_of_input = SF_TRUE;
            }
        }
        
        // check errors
        if ((error = src_process(src_state, &src_data)) != 0)
        {
            src_delete(src_state);
            return 0;
        }
        
        // check if we're done
        if (src_data.end_of_input && src_data.output_frames_gen == 0)
        {
            break;
        }
        
        // apply gain
        maxGainOut = apply_gain(src_data.data_out, src_data.output_frames_gen, nChannels, maxGainOut, gain);
        
        // write output
        sf_writef_float(fileout, bufferout, src_data.output_frames_gen);
        nFramesOut += src_data.output_frames_gen;
        src_data.data_in += src_data.input_frames_used * nChannels;
        src_data.input_frames -= src_data.input_frames_used;
    }
    
    // clean up
    src_delete(src_state);
    
    if (normalize && maxGainOut > 1.)
    {
        gain = 1. / maxGainOut;
        return -1;
    }
    
    return nFramesOut;
}

#ifdef __cplusplus
extern "C"
{
#endif // def __cplusplus
    void change_samplerate_setup(const int fieldNumber, Parameter& p)
    {
        switch (fieldNumber)
        {
            case kSampleRate:
                set_parameter_labels(p, 8, sr_labels);
                set_parameter_description(p, "convert sample rate to...");
                break;
            case kQuality:
                set_parameter_labels(p, 5, sr_quality);
                set_parameter_description(p, "pick the conversion type. (higher quality conversions are slower)");
                break;
            default: break;
        }
    }
    
    void* change_samplerate_handleInput(int argc, const char* argv[])
    {
        if (argc != kNumParameters) return 0;
        
        Input* arg = (Input*)malloc(sizeof(Input));
        if (arg == 0) return arg;
        
        // pick sample rate to convert to
        arg->sampleRate = string_to_double(argv[kSampleRate]);
        
        // pick sample rate converter
        const int nConverters = 5;
        arg->converter = SRC_SINC_MEDIUM_QUALITY; // default choice
        for (int i = 0; i < nConverters; i++)
        {
            if (strcmp(argv[kQuality], sr_quality[i]) == 0)
            {
                arg->converter = i;
                break;
            }
        }
        
        return arg;
    }
    
    bool change_samplerate_process(const char* pathin, const char* pathout, void* args)
    {
        if (args == 0) return false;
        Input* input = (Input*)args;
        
        // open file to read
        SF_INFO  sfinfo  = setup_sfinfo();
        SNDFILE* filein  = setup_filein(pathin, &sfinfo);
        if (filein == 0) return false;
        
        // get sample rate ratio
        const double src_ratio = input->sampleRate / (double)sfinfo.samplerate;
        if (!src_is_valid_ratio(src_ratio))
        {
            sf_close(filein);
            return false;
        }
        
        sf_count_t nFramesOut = 0;
        double gain = 1.;
        do
        {
            // close (if it exists) and reopen output file
            SNDFILE* fileout = setup_fileout(pathout, &sfinfo);
            if ((fileout = setup_fileout(pathout, &sfinfo)) == 0)
            {
                nFramesOut = 0;
                break;
            }
            
            // update the file header after every write
            sf_command(fileout, SFC_SET_UPDATE_HEADER_AUTO, 0, SF_TRUE);
            
            // make sure fileout is clipped by libsamplerate
            sf_command(fileout, SFC_SET_CLIPPING, 0, SF_TRUE);
            
            nFramesOut = do_convertion(filein, fileout, input->converter, src_ratio, sfinfo.channels, gain, true);
            sf_close(fileout);
            
        } while (nFramesOut < 0);
        
        // cleanup
        sf_close(filein);
        
        return nFramesOut != 0;
    }
    
#ifdef __cplusplus
}
#endif // def __cplusplus
