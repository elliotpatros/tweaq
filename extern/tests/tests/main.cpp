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
//#include "test_fade_in.h"
//#include "test_fade_out.h"
//#include "test_fade_in_and_out.h"
#include "test_convert.h"

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
//    // TEST _SETUP...
//    vector<Parameter> v;
//    int field = 0;
//    while (true)
//    {
//        Parameter p;
//        init_parameter(p);
//        change_gain_setup(field, p);
//        
//        const int lastField = field;
//        field += p.defaultValue != 0;
//        field += p.nLabels != 0;
//        
//        if (field == lastField) break;
//        v.emplace_back(p);
//    }
//    
//    // ...printout results
//    print_parameterlist(v);
//    
//    
//    // TEST INPUT HANDLING...
//    const char* responses[] = {"-3", "dB."};
//    void* input = change_gain_handleInput(sizeof(responses) / sizeof(char*), responses);
//    
//    
//    // TEST PROCESSING...
//    bool success = change_gain_process("/Users/demo/Desktop/testin/quad.wav",
//                                "/Users/demo/Desktop/testout/quad.wav",
//                                input);
//    
//    // TEST CLEANUP...
//    if (input != 0) free(input);
//    cout << (success ? "it worked!" : "oh no... :-(") << endl;

    cout << "done\n";
    
    return 0;
}
