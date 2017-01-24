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
#include "test_deinterleave.h"

using std::cout;
using std::endl;
using std::vector;

int main(int argc, const char * argv[])
{
    vector<Parameter> v;
    
    int field = 0;
    while (true)
    {
        Parameter p;
        init_parameter(p);
        deinterleave_setup(field, p);

        const int lastField = field;
        field += p.description != 0;
        field += p.nLabels != 0;

        if (field == lastField) break;
        v.emplace_back(p);
    }
    
//    cout << "default value = " << v[0].defaultValue << endl;
//    cout << "labels = ";
//    if (!v.empty()) for (int i = 0; i < v[0].nLabels; ++i) cout << v[0].labels[i] << ", ";
//    cout << endl << "description = " << v[0].description << endl;
    
    for (Parameter& p : v) free_parameter(p);
    
    const char* responses[] = {"-3.0", "dB."};
    const size_t nResponses = sizeof(responses) / sizeof(char*);
    
    cout << nResponses << " responses" << endl;
    void* input = deinterleave_handleInput(2, responses);
    bool success = deinterleave_process("/Users/demo/Desktop/testin/in.wav",
                                       "/Users/demo/Desktop/testout/in.wav",
                                       input);
    
    if (input != 0) free(input);
    
    cout << (success ? "it worked!" : "it failed :-(") << endl;
    
    return 0;
}
