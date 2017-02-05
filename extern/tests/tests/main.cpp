#include <iostream>
#include <vector>

//#include "change_gain.h"
//#include "fade_in_and_out.h"
//#include "fade_in.h"
//#include "fade_out.h"
#include "mix_to_mono.h"
//#include "normalize.h"
//#include "deinterleave.h"
//#include "change_samplerate.h"
//#include "convert.h" // do this next

using std::cout;
using std::endl;
using std::vector;

void print_parameterlist(vector<Parameter> v)
{
    for (auto& p : v)
    {
        if (p.description != 0) cout << "description: " << p.description << endl;
        if (p.defaultValue != 0) cout << "default value: " << p.defaultValue << endl;
        for (int i = 0; i < p.nLabels; i++) cout << "label " << i << ": " << p.labels[i] << endl;
        
        free_parameter(p);
        cout << endl;
    }
}

int main(int argc, const char * argv[])
{
    // TEST _SETUP...
    vector<Parameter> v;
    int field = 0;
    while (true)
    {
        Parameter p;
        init_parameter(p);
        mix_to_mono_setup(field, p);
        
        const int lastField = field;
        field += p.defaultValue != 0;
        field += p.nLabels != 0;
        
        if (field == lastField) break;
        v.emplace_back(p);
    }
    
    // ...printout results
    print_parameterlist(v);
    
    
    // TEST INPUT HANDLING...
    const char* responses[] = {"-3 dB."};
    void* input = mix_to_mono_handleInput(sizeof(responses) / sizeof(char*), responses);
    
    
    // TEST PROCESSING...
    bool success = mix_to_mono_process("/Users/demo/Desktop/desktop/testin/quad.wav",
                                   "/Users/demo/Desktop/desktop/testout/quad.wav",
                                   input);
    
    // TEST CLEANUP...
    free(input);
    cout << (success ? "it worked!" : "oh no... :-(") << endl;
    cout << "done\n";
    
    return 0;
}
