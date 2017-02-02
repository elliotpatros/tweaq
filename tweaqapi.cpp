#include "tweaqapi.h"
#include <stdio.h> // for sscanf, sprintf, etc
#include <fcntl.h> // for file open flags
#include <unistd.h> // for close(int file_descriptor)

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

void set_parameter_labels(Parameter& p, const int nLabels, const char** labels)
{
    // allocate number of labels
    if (p.labels == 0)
    {
        p.labels = (char**)calloc(nLabels, sizeof(char*));
    }
    else
    {
        p.labels = (char**)realloc(p.labels, nLabels * sizeof(char*));
        memset(p.labels, 0, nLabels * sizeof(char*));
    }
    
    if (p.labels == 0) return;
    
    // set string list
    for (int i = 0; i < nLabels; i++)
    {
        if (set_string(p.labels[i], labels[i]) == 0)
        {
            return free_string_list(p.labels, i);
        }
    }
    
    p.nLabels = nLabels;
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

// conversions
void clamp_double(double& val, const double min, const double max)
{
    val = (val > max) ? max : ((val < min) ? min : val);
}

bool double_equals(const double val, const double equals)
{
    return fabs(val - equals) < 0.00001;
}

// libsndfile stuff
SF_INFO setup_sfinfo()
{
    SF_INFO sfinfo;
    memset(&sfinfo, 0, sizeof(SF_INFO));
    return sfinfo;
}

double get_max_gain(const char* path)
{
    // setup libsndfile stuff
    SF_INFO sfinfo = setup_sfinfo();
    SNDFILE* file = sf_open(path, SFM_READ, &sfinfo);
    
    if (file == 0) return 1.;
    sf_seek(file, 0, SEEK_SET);
    
    // setup audio buffer
    const size_t nChannels = sfinfo.channels;
    const size_t buffersize = TQ_BUFFERSIZE * nChannels;
    double* buffer = (double*)calloc(buffersize, sizeof(double));
    if (buffer == 0)
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
            if (fabs(buffer[i]) > maxGain)
            {
                maxGain = fabs(buffer[i]);
            }
        }
    }
    
    // clean up
    sf_close(file);
    free(buffer);
    
    return (maxGain > 0.0000001) ? maxGain : 1.;
}

char* change_extension(const char* path, const int format)
{
    // check that we actually have an input and insert
    if (path == 0) return 0;
    
    // get the extension of this format
    SF_FORMAT_INFO info;
    memset(&info, 0, sizeof(SF_FORMAT_INFO));
    info.format = format;
    
    if (sf_command(NULL, SFC_GET_FORMAT_INFO, &info, sizeof(SF_FORMAT_INFO)) != SF_ERR_NO_ERROR)
    {
        return 0;
    }
    
    // find index from which the extension will be appended
    const size_t sizeof_path = strlen(path);
    const size_t sizeof_extension = strlen(info.extension);
    size_t insertFrom = sizeof_path;
    while (insertFrom-- > 0)
    {
        if (path[insertFrom] == '.')
        {
            insertFrom++; // include the '.' from the original path
            break;
        }
    }
    
    // allocate memory for output
    char* newPath = (char*)calloc(insertFrom + sizeof_extension + 1, 1);
    if (newPath == 0) return 0;
    
    // 1) copy old path through the final '.'
    // 2) append the new extension
    if (strncpy(newPath, path, insertFrom) == 0 ||
        strcat(newPath, info.extension) == 0)
    {
        free_string(newPath);
    }
    
    return newPath;
}

// file stuff
int file_exists(const char* path)
{
    return close(open(path, O_RDONLY | O_NOFOLLOW, S_IRUSR | S_IWUSR)) == 0;
}

char* unique_path(const char* path)
{
    if (path == 0) return 0;
    const size_t pathlen = strlen(path);
    
    char* newpath;
    if (!file_exists(path))
    {
        if ((newpath = (char*)calloc(pathlen + 1, 1)) == 0) return 0;
        strcpy(newpath, path);
        return newpath;
    }
    
    size_t insertFrom = pathlen; // index of the extension separator ('.')
    while (insertFrom-- > 0) if (path[insertFrom] == '.') break;
    
    if ((newpath = (char*)calloc(pathlen + strlen(" (999)") + 1, 1)) != 0 &&
        strncpy(newpath, path, insertFrom) != 0)
    {
        for (int nthcopy = 1; nthcopy < 999; nthcopy++)
        {
            if (sprintf(newpath + insertFrom, " (%i)%s", nthcopy, path + insertFrom) < 0)
            {
                break;
            }
            else if (!file_exists(newpath))
            {
                return newpath;
            }
        }
    }
    
    free(newpath);
    return 0;
}
