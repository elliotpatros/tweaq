#include "tweaqapi.h"

#ifdef __cplusplus
extern "C"
{
#endif // def __cplusplus
    
    enum ParameterFields{
        kGain = 0,
        kGainLabel,
        kNumParameters
    };
    
    struct Input {
        double gain;
    };
    
    static const char* description = "enter a value by which to change the volume.";
    
    /*
     * this function (_setup) is called after the user presses the "process"
     * button.  here, we tell tweaq how many and what kind of Parameters the
     * user will need to enter in order to process their audio files. _setup
     * will be called until it fails to initialize the Parameter it's passed.
     *
     * Parameters have two kinds of fields, a text box that the user can type
     * in, and a drop down menu from which the user chooses an option. a
     * Parameter can have one or both kinds of fields.
     *
     * to tell tweaq that you want this Parameter to have a text box, call
     * set_Parameter_default, and specify a default value for the text box.
     * if you don't want this Parameter to have a default value, the second
     * argument of set_Parameter_default should be blank (e.g. "")
     *
     * to tell tweaq that you want this Parameter to have a drop down menu,
     * call set_Parameter_labels. the second argument is how many labels the
     * menu should have, the following n arguments (where n == the second
     * argument) are the options the user may pick.
     *
     * finally, you may give the Parameter a description, which the user can
     * read if they're confused.
     *
     * a tweaq process may have any number of Parameters. _setup is called with
     * a fieldNumber of 0 first, which is incremented by the number of fields
     * that are initialized in the Parameter.
     */
    void change_gain_setup(const int fieldNumber, Parameter& p)
    {
        switch (fieldNumber)
        {
            case kGain:
                set_parameter_labels(p, 2, "dB.", "gain");
                set_parameter_default(p, "0.0");
                set_parameter_description(p, description);
                break;
            default: break;
        }
    }
    
    /*
     *this function (_handleInput) is called after the user enters information
     * into the Parameter fields they were given. here we should take their
     * responses, which will apply to all files to be processed, and convert
     * them to be meaningful for the _process function. the users responses are
     * given in a list of strings, text boxes first, in order the order
     * specified in _setup.
     *
     * how the values are saved is up to the coder. but the idea is to allocate
     * memory for some data structure, fill it with useful data, and return a
     * generic pointer back to tweaq. that pointer will be passed to the
     * _process function, where you can cast it back to it's original type and
     * use it.
     */
    void* change_gain_handleInput(int argc, const char* argv[])
    {
        if (argc != kNumParameters) return 0;
        
        Input* arg = (Input*)malloc(sizeof(Input));
        if (arg == 0) return arg;
        
        arg->gain = string_to_double(argv[kGain]);
        if (strcmp(argv[kGainLabel], "dB.") == 0) dB_to_gain(arg->gain);
        
        return arg;
    }
    
    bool change_gain_process(const char* pathin, const char* pathout, void* args)
    {
        if (args == 0) return false;
        Input* input = (Input*)args;
        
        // open files to read and write
        SF_INFO sfinfo;
        SNDFILE *filein, *fileout;
        memset(&sfinfo, 0, sizeof(SF_INFO));
        
        if ((filein  = sf_open(pathin,  SFM_READ,  &sfinfo)) == 0)
        {   // open failed, quit
            return false;
        }
        if ((fileout = sf_open(pathout, SFM_WRITE, &sfinfo)) == 0)
        {   // open failed, clean up and quit
            sf_close(filein);
            return false;
        }
        
        // setup read buffer (size must be a power of 2 * number of channels)
        const int read_buffersize = TQ_BUFFERSIZE * sfinfo.channels;
        double* buffer;
        if ((buffer = (double*)calloc(read_buffersize, sizeof(double))) == 0)
        {   // allocation failed, clean up and quit
            sf_close(filein);
            sf_close(fileout);
            return false;
        }
        
        // dsp
        size_t samplesread;
        while ((samplesread = sf_read_double(filein, buffer, read_buffersize)) != 0)
        {   // process the read buffer and write it to disk
            for (size_t i = 0; i < samplesread; ++i) buffer[i] *= input->gain;
            sf_write_double(fileout, buffer, samplesread);
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
