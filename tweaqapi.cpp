#include "tweaqapi.h"
#include <stdio.h> // for sscanf

// helpers
double string_to_double(const char* str)
{
    double d;
    sscanf(str, "%lf", &d);
    return d;
}

void dB_to_gain(double& dB)
{
    dB = pow(10.0, dB * 0.05);
}

double gain_from_dB(const double dB)
{
    return pow(10.0, dB * 0.05);
}

double get_max_gain(const char* path)
{
    // setup libsndfile stuff
    SF_INFO sfinfo;
    SNDFILE *file;
    memset(&sfinfo, 0, sizeof(SF_INFO));
    
    // open sound file to read
    if ((file = sf_open(path, SFM_READ, &sfinfo)) == 0)
    {
        return 1.;
    }
    
    // setup audio buffer
    const size_t nChannels = sfinfo.channels;
    const size_t buffersize = TQ_BUFFERSIZE * nChannels;
    double* buffer;
    if ((buffer = (double*)calloc(buffersize, sizeof(double))) == 0)
    {
        sf_close(file);
        return 1.;
    }
    
    // find max gain
    double maxGain = 0.;
    size_t samplesread;
    while ((samplesread = sf_read_double(file, buffer, buffersize)) != 0)
    {
        for (size_t i = 0; i < samplesread; i++)
        {
            const double sampleGain = fabs(buffer[i]);
            if (sampleGain > maxGain) maxGain = sampleGain;
        }
    }
    
    // clean up
    if (maxGain < 0.0000001) maxGain = 1.;
    sf_close(file);
    free(buffer);
    
    return maxGain;
}

// copies contents of source to destination
// if destination is a null ptr, will allocate memory
// returns 0 if failed
void* set_string(char*& dest, const char* src)
{
    if (dest == 0)
    {
        dest = (char*)calloc(strlen(src) + 1, sizeof(char));
    }
    else
    {
        dest = (char*)realloc(dest, sizeof(char) * (strlen(src) + 1));
    }
    
    return strcpy(dest, src);
}

void set_string_list_private(char**& dest, const int nStrings, va_list list)
{
    // allocate number of strings
    if (dest != 0) free(dest);
    dest = (char**)calloc(nStrings, sizeof(char*));
    if (dest == 0) return;

    // set strings
    for (int i = 0; i < nStrings; i++)
    {
        if (set_string(dest[i], va_arg(list, const char*)) == 0)
        {
            free_string_list(dest, i);
            return;
        }
    }
}

void set_string_list(char**& dest, const int nStrings, ...)
{   // todo: this function probably won't get used...
    // ... put set_string_list_private back into set_parameter_labels
    va_list list;
    va_start(list, nStrings);
    set_string_list_private(dest, nStrings, list);
    va_end(list);
}

void free_string(char*& str)
{
    if (str != 0)
    {
        free(str);
        str = 0;
    }
}

void free_string_list(char**& strings, int nStrings)
{
    if (strings == 0) return;
    while (nStrings-- > 0) free_string(strings[nStrings]);
    free(strings);
    strings = 0;
}

// parameter api
void set_parameter_description(Parameter& p, const char* description)
{
    set_string(p.description, description);
}

void set_parameter_labels(Parameter& p, const int nLabels, ...)
{
    // allocate number of labels
    if (p.labels != 0) free(p.labels);
    p.labels = (char**)calloc(nLabels, sizeof(char*));
    if (p.labels == 0) return;
    p.nLabels = nLabels;
    
    // set string list
    va_list list;
    va_start(list, nLabels);
    set_string_list_private(p.labels, nLabels, list);
    va_end(list);
}

void set_parameter_default(Parameter& p, const char* value)
{
    set_string(p.defaultValue, value);
}

void init_parameter(Parameter& p)
{
    memset(&p, 0, sizeof(Parameter));
}

void free_parameter(Parameter& p)
{
    free_string(p.description);
    free_string(p.defaultValue);
    free_string_list(p.labels, p.nLabels);
    p.nLabels = 0;
}
