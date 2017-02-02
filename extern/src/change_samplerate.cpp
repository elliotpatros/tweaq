#include "change_samplerate.h"

static const char* sr_labels[] = {"44100", "48000", "96000", "11025", "22050", "88200", "176400", "192000"};
static const char* sr_quality_labels[] = {"sinc (best quality)", "sinc (medium quality)", "sinc (fast)", "sample/hold", "linear (fastest)"};

void src_apply_gain(float* samples, const long nSamples, double gain, double& maxGain)
{
    for (long i = 0; i < nSamples; i++)
    {
        samples[i] *= gain;
        if (fabs(samples[i]) > maxGain)
        {
            maxGain = fabs(samples[i]);
        }
    }
}

double do_src(SNDFILE* filein, SNDFILE* fileout, int converter, double src_ratio, int nChannels, double gain)
{
    // setup read and write buffers
    float* bufferin = (float*)malloc(nChannels * TQ_BUFFERSIZE * sizeof(float));
    float* bufferout = (float*)malloc(nChannels * TQ_BUFFERSIZE * sizeof(float));
    if (bufferin == 0 || bufferout == 0)
    {
        free(bufferin);
        free(bufferout);
        return -1.;
    }
    
    // setup converter data
    SRC_STATE* src_state;
    SRC_DATA   src_data;
    int        error = SF_ERR_NO_ERROR;
    double     maxGain = 0.;
    sf_count_t nFramesWritten = 0;
    
    sf_seek(filein, 0, SEEK_SET);
    sf_seek(fileout, 0, SEEK_SET);
    
    // initialize the sample rate converter
    if ((src_state = src_new(converter, nChannels, &error)) == 0)
    {
        free(bufferin);
        free(bufferout);
        return -1.;
    }
    
    src_data.end_of_input = 0;
    src_data.input_frames = 0;
    src_data.data_in = bufferin;
    src_data.data_out = bufferout;
    src_data.src_ratio = src_ratio;
    src_data.output_frames = TQ_BUFFERSIZE;
    
    // do convertion
    while (true)
    {
        // if the input buffer is empty, refill it.
        if (src_data.input_frames == 0)
        {
            src_data.input_frames = sf_readf_float(filein, bufferin, TQ_BUFFERSIZE);
            src_data.data_in = bufferin;
            
            // the last read will not fill the buffer, so check on that
            if (src_data.input_frames < TQ_BUFFERSIZE)
            {
                src_data.end_of_input = SF_TRUE;
            }
        }
        
        // check errors
        if ((error = src_process(src_state, &src_data)) != 0)
        {
            maxGain = -1.;
            break;
        }
        
        // check if we're done
        if (src_data.end_of_input && src_data.output_frames_gen == 0)
        {
            break;
        }
        
        // apply gain
        src_apply_gain(src_data.data_out, src_data.output_frames_gen * nChannels, gain, maxGain);
        
        // write output
        sf_writef_float(fileout, bufferout, src_data.output_frames_gen);
        nFramesWritten += src_data.output_frames_gen;
        src_data.data_in += src_data.input_frames_used * nChannels;
        src_data.input_frames -= src_data.input_frames_used;
    }
    
    // clean up
    free(bufferin);
    free(bufferout);
    src_delete(src_state);
    
    return maxGain;
}

// convert format
bool setup_format_out(Input* input, SF_INFO& sfinfo)
{
    sfinfo.samplerate = (int)input->sampleRate;
    return sf_format_check(&sfinfo) == SF_TRUE;
}

// normalize
bool tq_apply_gain(const char* pathin, const char* pathout, const double gain, Input* input)
{
    // should we convert the audio file in place?
    const bool process_in_place = strcmp(pathin, pathout) == 0;
    
    // setup file in
    SF_INFO  sfinfo = setup_sfinfo();
    SNDFILE* filein = sf_open(pathin, process_in_place ? SFM_RDWR : SFM_READ, &sfinfo);
    if (filein == 0) return false;
    
    // setup soundfile buffer
    const size_t buffersize = TQ_BUFFERSIZE * sfinfo.channels;
    double* buffer = (double*)malloc(buffersize * sizeof(double));
    if (buffer == 0)
    {
        sf_close(filein);
        return false;
    }
    
    // setup file out
    sf_seek(filein, 0, SEEK_SET);
    SNDFILE* fileout = filein;
    if (!process_in_place)
    {
        if (!setup_format_out(input, sfinfo) ||
            (fileout = sf_open(pathout, SFM_WRITE, &sfinfo)) == 0)
        {
            sf_close(filein);
            free(buffer);
            return false;
        }
    }
    
    // adjust gain
    size_t samplesread;
    while ((samplesread = sf_read_double(filein, buffer, buffersize)) != 0)
    {
        for (size_t i = 0; i < samplesread; i++)
        {
            buffer[i] *= gain;
        }
        
        sf_write_double(fileout, buffer, samplesread);
    }
    
    // clean up
    sf_close(filein);
    sf_close(fileout);
    free(buffer);
    
    return true;
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
                set_parameter_labels(p, sizeof(sr_labels) / sizeof(char*), sr_labels);
                set_parameter_description(p, "convert to sample rate...");
                break;
            case kQuality:
                set_parameter_labels(p, sizeof(sr_quality_labels) / sizeof(char*), sr_quality_labels);
                set_parameter_description(p, "convert quality... (higher quality conversions are slower)");
                break;
            default: break;
        }
    }
    
    void* change_samplerate_handleInput(int argc, const char* argv[])
    {
        if (argc != kNumParameters) return 0;
        
        Input* arg = (Input*)malloc(sizeof(Input));
        if (arg == 0) return arg;
        
        // get sample rate
        arg->sampleRate = string_to_double(argv[kSampleRate]);
        
        // get converter
        const int nConverters = 5;
        arg->converter = SRC_SINC_MEDIUM_QUALITY; // default choice
        for (int i = 0; i < nConverters; i++)
        {
            if (strcmp(argv[kQuality], sr_quality_labels[i]) == 0)
            {
                arg->converter = i;
                break;
            }
        }
        
        return arg;
    }
    
    bool change_samplerate_process(const char* pathin, const char* pathout, void* args)
    {
        bool success = false;
        if (args == 0) return success;
        Input* input = (Input*)args;
        
        // open file to read
        SF_INFO  sfinfo = setup_sfinfo();
        SNDFILE* filein = sf_open(pathin, SFM_READ, &sfinfo);
        if (filein == 0) return success;
        
        // get sample rate ratio, unique output name, and update sfinfo
        const double src_ratio = input->sampleRate / (double)sfinfo.samplerate;
        char* unique_pathout = 0;
        if (!src_is_valid_ratio(src_ratio) ||
            !setup_format_out(input, sfinfo) ||
            (unique_pathout = unique_path(pathout)) == 0)
        {
            sf_close(filein);
            free(unique_pathout);
            return success;
        }
        
        // do dsp
        if (!double_equals(src_ratio, 1.)) // do we actually need to convert sample rate?
        {
            // convert sample rate
            double gain = 0.5;
            double maxGain = 0.;
            bool clipped = false;
            while (true)
            {
                // 1) open output file
                // 2) auto-update the file header after every write
                // 3) make sure fileout is clipped by libsamplerate
                SNDFILE* fileout = sf_open(unique_pathout, SFM_WRITE, &sfinfo);
                if (fileout == 0 ||
                    sf_command(fileout, SFC_SET_UPDATE_HEADER_AUTO, 0, SF_TRUE) != SF_TRUE ||
                    sf_command(fileout, SFC_SET_CLIPPING, 0, SF_TRUE) != SF_TRUE)
                {
                    sf_close(fileout);
                    break;
                }
                
                // do the conversion
                maxGain = do_src(filein, fileout, input->converter, src_ratio, sfinfo.channels, gain);
                sf_close(fileout);
                
                // did we clip? if not, let's move on.
                if (maxGain < 1.) break;
                gain = 1. / maxGain;
                clipped = true;
            }
            
            // how'd we do?
            success = maxGain > 0.;
            
            // normalize audio file out to match audio file in, if we didn't fail and we didn't clip
            if (success && !clipped)
            {
                sf_close(filein);
                success = tq_apply_gain(unique_pathout, unique_pathout, get_max_gain(pathin) / maxGain, input);
            }
        }
        else // don't need to convert sample rate. just copy the samples over
        {
            sf_close(filein);
            success = tq_apply_gain(pathin, unique_pathout, 1., input);
        }
        
        // cleanup
        sf_close(filein);
        free(unique_pathout);
        return success;
    }
    
#ifdef __cplusplus
}
#endif // def __cplusplus
