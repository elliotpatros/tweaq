//#include "tweaqapi.h"
//#include "ExternalHelpers.h"
//
//// data
//enum ParameterFields{
//    kSampleRate = 0,
//    kQuality,
//    kBitDepth,
//    kFormat,
//    kNumParameters
//};
//
//struct Input {
//    // sample rate conversion
//    double sampleRate;
//    int converter;
//    
//    // bit depth conversion
//    int bitDepth;
//    
//    // format conversion
//    int format;
//};
//
//static const char* sr_labels[] = {"44100", "48000", "96000", "11025", "22050", "88200", "176400", "192000"};
//static const char* sr_quality_labels[] = {"sinc (best quality)", "sinc (medium quality)", "sinc (fast)", "sample/hold", "linear (fastest)"};
//static const char* bit_depth_labels[] = {"32-bit float", "32-bit PCM", "24-bit PCM", "16-bit PCM", "8-bit PCM"};
//static const char* format_labels[] = {"wav", "aiff", "flac"};
//
//// apply gain
//static void
//apply_gain(float* samples, const long nSamples, double gain, double& peakGain)
//{
//    for (long i = 0; i < nSamples; i++)
//    {
//        samples[i] *= gain;
//        if (fabs(samples[i]) > peakGain)
//        {
//            peakGain = fabs(samples[i]);
//        }
//    }
//}
//
//// convert format
//static void
//convert_format(Input* input, SF_INFO& sfinfo)
//{
//    sfinfo.samplerate = (int)input->sampleRate;
//    sfinfo.format = input->format | input->bitDepth;
//}
//
//// convert sample rate
//static double
//convert_samplerate(SNDFILE* filein, SNDFILE* fileout, int converter, double src_ratio, int nChannels, double gain)
//{
//    // setup read and write buffers
//    float* bufferin = (float*)malloc(nChannels * TQ_BUFFERSIZE * sizeof(float));
//    float* bufferout = (float*)malloc(nChannels * TQ_BUFFERSIZE * sizeof(float));
//    if (bufferin == 0 || bufferout == 0)
//    {
//        free(bufferin);
//        free(bufferout);
//        return 0;
//    }
//    
//    // setup converter
//    SRC_STATE* src_state;
//    SRC_DATA   src_data;
//    int        error = SF_ERR_NO_ERROR;
//    double     peakGain = 0.;
//    sf_count_t nFramesWritten = 0;
//    
//    sf_seek(filein, 0, SEEK_SET);
//    sf_seek(fileout, 0, SEEK_SET);
//    
//    // initialize the sample rate converter
//    if ((src_state = src_new(converter, nChannels, &error)) == 0)
//    {
//        std::cout << "sample rate converter could not be initialized\n";
//        free(bufferin);
//        free(bufferout);
//        return -1.;
//    }
//    
//    src_data.end_of_input = 0;
//    src_data.input_frames = 0;
//    src_data.data_in = bufferin;
//    src_data.data_out = bufferout;
//    src_data.src_ratio = src_ratio;
//    src_data.output_frames = TQ_BUFFERSIZE;
//    
//    // do convertion
//    while (true)
//    {
//        // if the input buffer is empty, refill it.
//        if (src_data.input_frames == 0)
//        {
//            src_data.input_frames = sf_readf_float(filein, bufferin, TQ_BUFFERSIZE);
//            src_data.data_in = bufferin;
//            
//            // the last read will not fill the buffer, so check on that
//            if (src_data.input_frames < TQ_BUFFERSIZE)
//            {
//                src_data.end_of_input = SF_TRUE;
//            }
//        }
//        
//        // check errors
//        if ((error = src_process(src_state, &src_data)) != 0)
//        {
//            std::cout << "\nerror: " << src_strerror(error) << std::endl;
//            peakGain = -1.;
//            break;
//        }
//        
//        // check if we're done
//        if (src_data.end_of_input && src_data.output_frames_gen == 0) break;
//        
//        // apply gain
//        apply_gain(src_data.data_out, src_data.output_frames_gen * nChannels, gain, peakGain);
//        
//        // write output
//        sf_writef_float(fileout, bufferout, src_data.output_frames_gen);
//        nFramesWritten += src_data.output_frames_gen;
//        src_data.data_in += src_data.input_frames_used * nChannels;
//        src_data.input_frames -= src_data.input_frames_used;
//    }
//    
//    // clean up
//    free(bufferin);
//    free(bufferout);
//    src_delete(src_state);
//    
//    return peakGain;
//}
//
//// normalize
//static bool apply_af_gain(const char* pathin, const char* pathout, const double gain, Input* input)
//{
//    // should we convert the audio file in place?
//    const bool convert_in_place = strcmp(pathin, pathout) == 0;
//    
//    // setup file in
//    SF_INFO sfinfo = setup_sfinfo();
//    SNDFILE* filein = sf_open(pathin, convert_in_place ? SFM_RDWR : SFM_READ, &sfinfo);
//    if (filein == 0) return false;
//
//    // setup soundfile buffer
//    const size_t buffersize = TQ_BUFFERSIZE * sfinfo.channels;
//    double* buffer = (double*)malloc(buffersize * sizeof(double));
//    if (buffer == 0)
//    {
//        sf_close(filein);
//        return false;
//    }
//
//    // setup file out
//    sf_seek(filein, 0, SEEK_SET);
//    SNDFILE* fileout = filein;
//    if (!convert_in_place)
//    {
//        convert_format(input, sfinfo);
//        fileout = setup_fileout(pathout, &sfinfo);
//        if (fileout == 0)
//        {
//            sf_close(filein);
//            free(buffer);
//            return false;
//        }
//    }
//    
//    // do dsp
//    size_t samplesread;
//    while ((samplesread = sf_read_double(filein, buffer, buffersize)) != 0)
//    {
//        for (size_t i = 0; i < samplesread; i++)
//        {
//            buffer[i] *= gain;
//        }
//        
//        sf_write_double(fileout, buffer, samplesread);
//    }
//    
//    // clean up
//    sf_close(filein);
//    if (!convert_in_place) sf_close(fileout);
//    free(buffer);
//    
//    return true;
//}
//
//#ifdef __cplusplus
//extern "C"
//{
//#endif // def __cplusplus
//    void convert_setup(const int fieldNumber, Parameter& p)
//    {
//        switch (fieldNumber)
//        {
//            case kSampleRate:
//                set_parameter_labels(p, 8, sr_labels);
//                set_parameter_description(p, "convert to sample rate...");
//                break;
//            case kQuality:
//                set_parameter_labels(p, 5, sr_quality_labels);
//                set_parameter_description(p, "pick the conversion type. (higher quality conversions are slower)");
//                break;
//            case kBitDepth:
//                set_parameter_labels(p, 5, bit_depth_labels);
//                set_parameter_description(p, "convert to bit depth...");
//                break;
//            case kFormat:
//                set_parameter_labels(p, 3, format_labels);
//                set_parameter_description(p, "convert to file format...");
//                break;
//            default: break;
//        }
//    }
//    
//    void* convert_handleInput(int argc, const char* argv[])
//    {
//        if (argc != kNumParameters) return 0;
//        
//        Input* arg = (Input*)malloc(sizeof(Input));
//        if (arg == 0) return arg;
//        
//        // get sample rate
//        arg->sampleRate = string_to_double(argv[kSampleRate]);
//        
//        // get converter
//        const int nConverters = 5;
//        arg->converter = SRC_SINC_MEDIUM_QUALITY; // default choice
//        for (int i = 0; i < nConverters; i++)
//        {
//            if (strcmp(argv[kQuality], sr_quality_labels[i]) == 0)
//            {
//                arg->converter = i;
//                break;
//            }
//        }
//        
//        // get bit depth
//        if      (strcmp(argv[kBitDepth], "32-bit float") == 0) arg->bitDepth = SF_FORMAT_FLOAT;
//        else if (strcmp(argv[kBitDepth], "32-bit PCM") == 0)   arg->bitDepth = SF_FORMAT_PCM_32;
//        else if (strcmp(argv[kBitDepth], "16-bit PCM") == 0)   arg->bitDepth = SF_FORMAT_PCM_16;
//        else if (strcmp(argv[kBitDepth], "8-bit PCM") == 0)    arg->bitDepth = SF_FORMAT_PCM_S8;
//        else                                                   arg->bitDepth = SF_FORMAT_PCM_24; // default choice (24 bit)
//        
//        // get file format
//        if      (strcmp(argv[kFormat], "aiff") == 0) arg->format = SF_FORMAT_AIFF;
//        else if (strcmp(argv[kFormat], "flac") == 0) arg->format = SF_FORMAT_FLAC;
//        else                                         arg->format = SF_FORMAT_WAV; // default choice (wav)
//        
//        return arg;
//    }
//    
//    bool convert_process(const char* pathin, const char* pathout, void* args)
//    {
//        if (args == 0) return false;
//        Input* input = (Input*)args;
//        
//        // open file to read
//        SF_INFO  sfinfo  = setup_sfinfo();
//        SNDFILE* filein  = setup_filein(pathin, &sfinfo);
//        if (filein == 0) return false;
//        
//        // get sample rate ratio
//        const double src_ratio = input->sampleRate / (double)sfinfo.samplerate;
//        if (!src_is_valid_ratio(src_ratio))
//        {
//            std::cout << "sample rate conversion ratio, " << src_ratio << ", is invalid \n";
//            sf_close(filein);
//            return false;
//        }
//        
//        // setup output sfinfo
//        sfinfo.samplerate = (int)input->sampleRate;
//        sfinfo.format = input->format | input->bitDepth;
//        
//        // correct the output file's extension
//        char* newPathout = change_extension(pathout, input->format);
//        if (newPathout == 0)
//        {
//            sf_close(filein);
//            return false;
//        }
//        
//        // do dsp
//        bool success = false;
//        if (!double_equals(src_ratio, 1.)) // do we actually need to convert sample rate?
//        {
//            std::cout << "converting sample rate\n";
//            
//            // convert sample rate
//            double gain = 0.5;
//            double peakGain = 0.;
//            bool clipped = false;
//            while (true)
//            {
//                // open output file
//                SNDFILE* fileout = setup_fileout(newPathout, &sfinfo);
//                if ((fileout = setup_fileout(newPathout, &sfinfo)) == 0)
//                {
//                    sf_close(filein);
//                    free(newPathout);
//                    return false;
//                }
//                
//                // update the file header after every write
//                sf_command(fileout, SFC_SET_UPDATE_HEADER_AUTO, 0, SF_TRUE);
//                
//                // make sure fileout is clipped by libsamplerate
//                sf_command(fileout, SFC_SET_CLIPPING, 0, SF_TRUE);
//                
//                // do the conversion
//                peakGain = convert_samplerate(filein, fileout, input->converter, src_ratio, sfinfo.channels, gain);
//                sf_close(fileout);
//                
//                // did we clip? if not, let's move on.
//                if (peakGain < 1.) break;
//                
//                gain = 1. / peakGain;
//                clipped = true;
//                std::cout << "we clipped. trying again with gain: " << gain << std::endl;
//            }
//            
//            // how'd we do?
//            success = peakGain > 0.;
//            
//            // normalize audio file out to match audio file in, if we didn't fail and we didn't clip
//            if (!clipped && success)
//            {
//                sf_close(filein);
//                const double peakin = get_max_gain(pathin);
//                const double fix =  peakin / peakGain;
//                std::cout << "normalizing converted file by " << fix << std::endl;
//                std::cout << "... peak was " << peakin << std::endl;
//                std::cout << "... and is now " << peakGain << std::endl;
//                
//                success = apply_af_gain(newPathout, newPathout, fix, input);
//            }
//        }
//        else // don't need to convert sample rate. just copy the samples over
//        {
//            std::cout << "no sample rate conversion needed. copying data.\n";
//            sf_close(filein);
//            success = apply_af_gain(pathin, newPathout, 1., input);
//        }
//
//        // cleanup
//        free(newPathout);
//        
//        return success;
//    }
//    
//#ifdef __cplusplus
//}
//#endif // def __cplusplus