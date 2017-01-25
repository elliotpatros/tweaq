//
//  main.cpp
//  tests
//
//  Created by demo on 1/17/17.
//  Copyright (c) 2017 demo. All rights reserved.
//

#include <iostream>
#include <vector>

#include "tweaqapi.h"
//#include "test_change_gain.h"
//#include "test_mix_to_mono.h"
//#include "test_normalize.h"
//#include "test_deinterleave.h"
#include "test_fade_in.h"

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
        fade_in_setup(field, p);
        
        const int lastField = field;
        field += p.defaultValue != 0;
        field += p.nLabels != 0;
        
        if (field == lastField) break;
        v.emplace_back(p);
    }
    
    // ...printout results
    print_parameterlist(v);
    
    
    // TEST INPUT HANDLING...
    const char* responses[] = {"100", "milliseconds", "'S' curve"};
    void* input = fade_in_handleInput(3, responses);
    
    
    // TEST PROCESSING...
    bool success = fade_in_process("/Users/demo/Desktop/testin/quad.wav",
                                   "/Users/demo/Desktop/testout/quad.wav",
                                   input);
    
    // TEST CLEANUP...
    if (input != 0) free(input);
    cout << (success ? "it worked!" : "oh no... :-(") << endl;
    
    
    return 0;
}
