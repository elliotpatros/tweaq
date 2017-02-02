#ifndef TQ_CHANGE_GAIN_
#define TQ_CHANGE_GAIN_

#include "tweaqapi.h"

enum ParameterFields{
    kGain = 0,
    kGainLabel,
    kNumParameters
};

struct Input {
    double gain;
};

#ifdef __cplusplus
extern "C"
{
#endif // def __cplusplus

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
    void change_gain_setup(const int fieldNumber, Parameter& p);
    
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
    void* change_gain_handleInput(int argc, const char* argv[]);
    bool change_gain_process(const char* pathin, const char* pathout, void* args);
    
#ifdef __cplusplus
}
#endif // def __cplusplus

#endif // TQ_CHANGE_GAIN_ defined
