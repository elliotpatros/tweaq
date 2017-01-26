#include "tweaqapi.h"

void insertChannelNumber(const char* input, const size_t channel, char*& output)
{   // output should be uninitialized and null
    
    // check that we actually have an input and insert
    if (input == 0 || output != 0) return;
    
    // make string to insert
    char insert[8] = {'\0'};
    sprintf(insert, "-ch%02lu", channel + 1);
    
    // find index to separate input
    const size_t inputLength = strlen(input);
    const size_t insertLength = strlen(insert);
    
    size_t insertIndex = inputLength;
    while (insertIndex-- > 0) if (input[insertIndex] == '.') break;
    
    // allocate memory for output
    if ((output = (char*)calloc(inputLength + insertLength + 1, sizeof(char))) == 0) return;
    
    // 1) copy first part of input to output
    // 2) append string 'insert'
    // 3) append the rest of input
    if (strncpy(output, input, insertIndex) == 0 ||
        strcat(output, insert) == 0 ||
        strcat(output, input + insertIndex) == 0)
    {
        free_string(output);
        return;
    }
}

#ifdef __cplusplus
extern "C"
{
#endif // def __cplusplus
    
    void deinterleave_setup(const int fieldNumber, Parameter& p)
    {
        return; // no parameters
    }
    
    void* deinterleave_handleInput(int argc, const char* argv[])
    {
        return 0; // no parameters
    }
    
    bool deinterleave_process(const char* pathin, const char* pathout, void* args)
    {
        // setup libsndfile stuff
        SF_INFO sfinfo  = setup_sfinfo();
        SNDFILE* filein = setup_filein(pathin, &sfinfo);
        if (filein == 0) return false;
        
        // get pathin's channel count
        const size_t nChannels = sfinfo.channels;
        
        // setup input buffer
        const size_t buffersize = TQ_BUFFERSIZE * nChannels;
        double* bufferin = (double*)calloc(buffersize, sizeof(double));
        if (bufferin == 0)
        {
            sf_close(filein);
            return false;
        }
        
        // setup output buffer
        double bufferout[TQ_BUFFERSIZE];
        
        // do dsp
        for (size_t channel = 0; channel < nChannels; channel++)
        {
            // reopen input file
            if (filein == 0)
            {
                filein = setup_filein(pathin, &sfinfo);
                if (filein == 0)
                {
                    free(bufferin);
                    return false;
                }
            }
            
            // insert channel number in pathout
            char* mono_pathout = 0;
            if (nChannels > 1)
            {
                insertChannelNumber(pathout, channel, mono_pathout);
            }
            else
            {
                mono_pathout = (char*)calloc(strlen(pathout), sizeof(char));
                if (mono_pathout != 0) memcpy(mono_pathout, pathout, strlen(pathout) * sizeof(char));
            }
            
            if (mono_pathout == 0)
            {
                free(bufferin);
                sf_close(filein);
                return false;
            }
            
            // open output soundfile
            sfinfo.channels = 1;
            SNDFILE* fileout = setup_fileout(mono_pathout, &sfinfo);
            if (fileout == 0)
            {
                free(bufferin);
                sf_close(filein);
                return false;
            }
            
            // copy and deinterleave
            size_t samplesread;
            while ((samplesread = sf_read_double(filein, bufferin, buffersize)) != 0)
            {
                size_t sampleout = 0;
                for (size_t samplein = channel; samplein < samplesread; samplein += nChannels, sampleout++)
                {
                    bufferout[sampleout] = bufferin[samplein];
                }
                
                sf_write_double(fileout, bufferout, sampleout);
            }
            
            // clean up
            sf_close(filein);
            sf_close(fileout);
            filein = 0;
            free(mono_pathout);
        }
        
        // clean up
        return true;
    }
    
#ifdef __cplusplus
}
#endif // def __cplusplus
