#include "change_samplerate.h"

static const char* sr_labels[] = {"44100", "48000", "96000", "11025", "22050", "88200", "176400", "192000"};
static const char* sr_quality_labels[] = {"sinc (best quality)", "sinc (medium quality)", "sinc (fast)", "sample/hold", "linear (fastest)"};

void src_apply_gain(float* samples, const long nSamples, double gain, double& maxGain)
{
    for (long i = 0; i < nSamples; i++)
    {
        samples[i] *= gain;
        if (fabs(samples[i]) > maxGain)
            maxGain = fabs(samples[i]);
    }
}

// convert format
bool validate_format_out(Input* input, SF_INFO& sfinfo)
{
    sfinfo.samplerate = (int)input->samplerate;
    return sf_format_check(&sfinfo) == SF_TRUE;
}

double do_src(const char* pathin, const char* pathout, const double gain, Input* input)
{
    double maxGain = -1.;
    
    // open filein
    SF_INFO sfinfo = setup_sfinfo();
    SNDFILE* filein = sf_open(pathin, SFM_READ, &sfinfo), *fileout = 0;
    if (filein == 0) return maxGain;
    
    // open fileout w/ new samplerate
    const double old_samplerate = sfinfo.samplerate;
    if (validate_format_out(input, sfinfo))
        fileout = sf_open(pathout, SFM_WRITE, &sfinfo);
    
    // setup converter data
    int        error = SF_ERR_NO_ERROR;
    sf_count_t nFramesWritten = 0;
    SRC_STATE* src_state = src_new(input->converter, sfinfo.channels, &error);
    
    // setup read and write buffers
    float* bufferin = (float*)malloc(sfinfo.channels * TQ_BUFFERSIZE * sizeof(float));
    float* bufferout = (float*)malloc(sfinfo.channels * TQ_BUFFERSIZE * sizeof(float));
    
    // setup src data
    SRC_DATA src_data;
    memset(&src_data, 0, sizeof(SRC_DATA));
    src_data.data_in = bufferin;
    src_data.data_out = bufferout;
    src_data.src_ratio = sfinfo.samplerate / old_samplerate;
    src_data.output_frames = TQ_BUFFERSIZE;
    
    // 1) check that filein and fileout opened ok
    // 2) check that src converter was setup ok
    // 3) check that bufferin and bufferout were setup ok
    // 4) check that src ratio is valid
    // 5) auto-update the file header after every write
    // 6) make sure fileout is clipped by libsamplerate
    if (filein == 0 || fileout == 0 ||
        src_state == 0 || error != SF_ERR_NO_ERROR ||
        bufferin == 0 || bufferout == 0 ||
        !src_is_valid_ratio(src_data.src_ratio) ||
        !sf_command(fileout, SFC_SET_UPDATE_HEADER_AUTO, 0, SF_TRUE) ||
        !sf_command(fileout, SFC_SET_CLIPPING, 0, SF_TRUE))
    {
        goto clean_up_do_src;
    }
    
    sf_seek(filein, 0, SEEK_SET);
    sf_seek(fileout, 0, SEEK_SET);
    
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
                src_data.end_of_input = SF_TRUE;
        }
        
        // check for errors
        if (src_process(src_state, &src_data) != SF_ERR_NO_ERROR)
        {
            maxGain = -1.;
            break;
        }
        
        // check if we're done
        if (src_data.end_of_input && src_data.output_frames_gen == 0)
            break;
        
        // apply gain to the block of data we just processed and keep track of maxGain
        src_apply_gain(src_data.data_out, src_data.output_frames_gen * sfinfo.channels, gain, maxGain);
        
        // write output
        sf_writef_float(fileout, bufferout, src_data.output_frames_gen);
        nFramesWritten += src_data.output_frames_gen;
        src_data.data_in += src_data.input_frames_used * sfinfo.channels;
        src_data.input_frames -= src_data.input_frames_used;
    }
    
    // clean up
clean_up_do_src:
    sf_close(filein);
    sf_close(fileout);
    free(bufferin);
    free(bufferout);
    src_delete(src_state);
    
    return maxGain;
}

// normalize
bool post_apply_gain(const char* pathin, const char* pathout, const double gain, Input* input)
{
    bool success = false;
    
    // should we convert the audio file in place?
    const bool pathin_equals_out = strcmp(pathin, pathout) == 0;
    
    // setup file in
    SF_INFO  sfinfo = setup_sfinfo();
    SNDFILE *filein = sf_open(pathin, pathin_equals_out ? SFM_RDWR : SFM_READ, &sfinfo), *fileout = 0;
    if (filein == 0) return success;
    
    // setup soundfile buffer
    const size_t buffersize = TQ_BUFFERSIZE * sfinfo.channels;
    double* buffer = (double*)malloc(buffersize * sizeof(double));
    if (buffer == 0) goto clean_up_apply_gain;
    
    // setup file out
    if (pathin_equals_out)
        fileout = filein;
    else if (validate_format_out(input, sfinfo))
        fileout = sf_open(pathout, SFM_WRITE, &sfinfo);
    
    if (fileout == 0) goto clean_up_apply_gain;
    sf_seek(filein, 0, SEEK_SET);
    sf_seek(fileout, 0, SEEK_SET);
    
    // adjust gain
    size_t samplesread;
    while ((samplesread = sf_read_double(filein, buffer, buffersize)) != 0)
    {
        for (size_t i = 0; i < samplesread; i++)
            buffer[i] *= gain;
        
        if (sf_write_double(fileout, buffer, samplesread) != (sf_count_t)samplesread)
            goto clean_up_apply_gain;
    }
    
    // done
    success = true;
clean_up_apply_gain:
    sf_close(filein);
    sf_close(fileout);
    free(buffer);
    
    return success;
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
        
        Input* input = (Input*)calloc(1, sizeof(Input));
        if (input == 0) return input;
        
        // get sample rate
        input->samplerate = string_to_double(argv[kSampleRate]);
        
        // get converter
        const int nConverters = sizeof(sr_quality_labels) / sizeof(char*);
        input->converter = SRC_SINC_MEDIUM_QUALITY; // default choice
        for (int i = 0; i < nConverters; i++)
            if (strcmp(argv[kQuality], sr_quality_labels[i]) == 0)
            {
                input->converter = i;
                break;
            }
        
        return input;
    }
    
    bool change_samplerate_process(const char* pathin, const char* pathout, void* args)
    {
        bool success = false;
        if (args == 0) return success;
        Input* input = (Input*)args;
        
        // get sample rate of pathin and unique output name
        double old_samplerate = get_samplerate(pathin);
        char* unique_pathout = unique_path(pathout);
        if (unique_pathout == 0) goto clean_up;
        
        // dsp (convert samplerate)
        if (double_equals(old_samplerate, input->samplerate))
        {   // no need to convert sample rate. just copy
            if (!post_apply_gain(pathin, unique_pathout, 1., input))
                goto clean_up;
        }
        else
        {   // we do need to convert the sample rate.
            double gain = 0.5;
            double maxGain = -1.;
            bool clipped = false;
            
            while (true)
            {
                // convert
                maxGain = do_src(pathin, unique_pathout, gain, input);
                
                // did we fail? if so, let's clean up
                if (maxGain < 0.) goto clean_up;
                
                // did we clip? if not, let's move on. if so, try again.
                if (maxGain <= 1.) break;
                gain = 1. / maxGain;
                clipped = true;
            }
            
            // to minimize the chance of clipping during src, we arbitrarily
            // attenuate during src, while intending to fix it later (aka now).
            // it's hacky but faster than calling src twice, sue me. however, if
            // we did clip during src, src gets called again anyway. And in that
            // case, the output's max gain will == 1 and we'll skip this step.
            if (!clipped)
            {
                if (!post_apply_gain(unique_pathout, unique_pathout, get_max_gain(pathin) / maxGain, input))
                    goto clean_up;
            }
        }
        
        // done
        success = true;
    clean_up:
        free(unique_pathout);
        
        return success;
    }
    
#ifdef __cplusplus
}
#endif // def __cplusplus
