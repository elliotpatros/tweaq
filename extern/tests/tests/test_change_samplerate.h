//#include "tweaqapi.h"
//#include <samplerate.h>
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
//// format pathout
//static char* change_extension(const char* pathout, const int format)
//{
//    // check that we actually have an input and insert
//    if (pathout == 0) return 0;
//    
//    // get the extension of this format
//    SF_FORMAT_INFO info;
//    memset(&info, 0, sizeof(SF_FORMAT_INFO));
//    info.format = format;
//    
//    if (sf_command(NULL, SFC_GET_FORMAT_INFO, &info, sizeof(SF_FORMAT_INFO)) != SF_ERR_NO_ERROR)
//    {
//        return 0;
//    }
//    
//    // find index from which the extension will be appended
//    const size_t sizeof_pathout = strlen(pathout);
//    const size_t sizeof_extension = strlen(info.extension);
//    size_t insertFrom = sizeof_pathout;
//    while (insertFrom-- > 0) if (pathout[insertFrom] == '.') break;
//    insertFrom++; // include the '.' from the original pathout
//    
//    // allocate memory for output
//    char* newPathout = (char*)calloc(insertFrom + sizeof_extension + 1, sizeof(char));
//    if (newPathout == 0) return 0;
//    
//    if (strncpy(newPathout, pathout, insertFrom) == 0 ||
//        strcat(newPathout, info.extension) == 0)
//    {
//        free_string(newPathout);
//    }
//    
//    return newPathout;
//}
//
//// apply gain
//static double
//apply_gain(float* data, long nFrames, int nChannels, double maxGain, double gain)
//{
//    const long nSamples = nFrames * nChannels;
//    for (long i = 0; i < nSamples; i++)
//    {
//        data[i] *= gain;
//        if (fabs(data[i]) > maxGain)
//        {
//            maxGain = fabs(data[i]);
//        }
//    }
//    
//    return maxGain;
//}
//
//// convert sample rate
//static sf_count_t
//do_convertion(SNDFILE* filein, SNDFILE* fileout, int converter, double src_ratio, int nChannels, double& gain)
//{
//    // setup read and write buffers
//    float* bufferin = (float*)malloc(nChannels * TQ_BUFFERSIZE);
//    float* bufferout = (float*)malloc(nChannels * TQ_BUFFERSIZE);
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
//    double     maxGainOut = 0.0;
//    sf_count_t nFramesOut = 0;
//    sf_count_t nFramesIn = TQ_BUFFERSIZE / nChannels;
//    
//    sf_seek(filein, 0, SEEK_SET);
//    sf_seek(fileout, 0, SEEK_SET);
//    
//    // initialize the sample rate converter
//    if ((src_state = src_new(converter, nChannels, &error)) == 0)
//    {
//        std::cout << "sample rate converter could not be initialized\n";
//        src_delete(src_state);
//        return 0;
//    }
//    
//    src_data.end_of_input = 0;
//    src_data.input_frames = 0;
//    src_data.data_in = bufferin;
//    src_data.data_out = bufferout;
//    src_data.src_ratio = src_ratio;
//    src_data.output_frames = nFramesIn;
//    
//    // do convertion
//    while (true)
//    {
//        // if the input buffer is empty, refill it.
//        if (src_data.input_frames == 0)
//        {
//            src_data.input_frames = sf_readf_float(filein, bufferin, nFramesIn);
//            src_data.data_in = bufferin;
//            
//            // the last read will not fill the buffer, so check on that
//            if (src_data.input_frames < nFramesIn)
//            {
//                src_data.end_of_input = SF_TRUE;
//            }
//        }
//        
//        // check errors
//        if ((error = src_process(src_state, &src_data)) != 0)
//        {
//            std::cout << "\nerror: " << src_strerror(error) << std::endl;
//            src_delete(src_state);
//            return 0;
//        }
//        
//        // check if we're done
//        if (src_data.end_of_input && src_data.output_frames_gen == 0)
//        {
//            break;
//        }
//        
//        // apply gain
//        maxGainOut = apply_gain(src_data.data_out, src_data.output_frames_gen, nChannels, maxGainOut, gain);
//        
//        // write output
//        sf_writef_float(fileout, bufferout, src_data.output_frames_gen);
//        nFramesOut += src_data.output_frames_gen;
//        src_data.data_in += src_data.input_frames_used * nChannels;
//        src_data.input_frames -= src_data.input_frames_used;
//    }
//    
//    // clean up
//    src_delete(src_state);
//    
//    if (maxGainOut > 1.)
//    {
//        std::cout << "clipped output... peak: " << maxGainOut << ". trying again w/ gain: " << gain << std::endl;
//        gain = 1. / maxGainOut;
//        return -1;
//    }
//    
//    return nFramesOut;
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
//        // convert audio file
//        sf_count_t nFramesOut = 0;
//        double gain = 1.;
//        do
//        {
//            // close (if it exists) and reopen output file
//            SNDFILE* fileout = setup_fileout(newPathout, &sfinfo);
//            if ((fileout = setup_fileout(newPathout, &sfinfo)) == 0)
//            {
//                nFramesOut = 0;
//                break;
//            }
//            
//            // update the file header after every write
//            sf_command(fileout, SFC_SET_UPDATE_HEADER_AUTO, 0, SF_TRUE);
//            
//            // make sure fileout is clipped by libsamplerate
//            sf_command(fileout, SFC_SET_CLIPPING, 0, SF_TRUE);
//            
//            nFramesOut = do_convertion(filein, fileout, input->converter, src_ratio, sfinfo.channels, gain);
//            sf_close(fileout);
//            
//        } while (nFramesOut < 0);
//        
//        // cleanup
//        free(newPathout);
//        sf_close(filein);
//        
//        return nFramesOut != 0;
//    }
//    
//#ifdef __cplusplus
//}
//#endif // def __cplusplus
