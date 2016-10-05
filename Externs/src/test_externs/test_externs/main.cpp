//#include "change_gain.h"
//#include "mix_to_mono.h"
//#include "normalize.h"
//#include "split_to_mono.h"
//#include "strip_silence_at_end.h"
//#include "strip_silence_at_start.h"
//#include "fade_in.h"
//#include "fade_out.h"
#include "time_stretch.h"

#include <iostream>
#include <memory>
using std::cout;
using std::endl;
int main(int argc, const char* argv[])
{
    bool itWorked;

    // change gain
    /*
    const char* args[128] =
    {
    "/Users/demo/Drews_things/In_Downloads/rain_12ch.aif",
    "/Users/demo/Desktop/testout/rain_12ch.aiff",
    "6",
    "dB."
    };
    itWorked = change_gain_process(4, args);
     */
    //\change gain
    
    // normalize
    /*
    const char* args[128] =
    {
        "/Users/demo/Drews_things/In_Downloads/rain_12ch.aif",
        "/Users/demo/Desktop/testout/rain_12ch.aiff",
        "0",
        "dB."
    };
    itWorked = normalize_process(4, args);
     */
    //\normalize
    
    // split to mono
    /*
    const char* args[128] =
    {
        "/Users/demo/Drews_things/In_Downloads/rain_12ch.aif",
        "/Users/demo/Desktop/testout/rain_12ch.aiff"
    };
    itWorked = split_to_mono_process(2, args);
     */
    //\split to mono
    
    // mix to mono
    /*
    const char* args[128] =
    {
        "/Users/demo/Drews_things/In_Downloads/rain_12ch.aif",
        "/Users/demo/Desktop/testout/rain_12ch.aiff",
        "0",
        "dB."
    };
    itWorked = mix_to_mono_process(4, args);
     */
    //\mix to mono
    
    // strip silence at end
    /*
    const char* args[128] =
    {
        "/Users/demo/Drews_things/In_Downloads/rain_12ch.aif",
        "/Users/demo/Desktop/testout/rain_12ch.aiff",
        "-60",
        "dB."
    };
    itWorked = strip_silence_at_end_process(4, args);
     */
    //\strip silence at end
    
    // strip silence at start
    /*
    const char* args[128] =
    {
        "/Users/demo/Drews_things/In_Downloads/rain_12ch.aif",
        "/Users/demo/Desktop/testout/rain_12ch.aiff",
        "-60",
        "dB."
    };
    itWorked = strip_silence_at_start_process(4, args);
     */
    //\strip silence at start
    
    // fade in
    /*
    const char* args[128] =
    {
        "/Users/demo/Desktop/testin/threeones.wav",
        "/Users/demo/Desktop/testout/threeones.wav",
        "50",
        "milliseconds",
        "equal gain"
    };
    itWorked = fade_in_process(5, args);
    */
    //\fade in
    
    // fade out
    /*
    const char* args[128] =
    {
        "/Users/demo/Desktop/testin/threeones.wav",
        "/Users/demo/Desktop/testout/threeones.wav",
        "500",
        "milliseconds",
        "equal gain"
    };
    itWorked = fade_out_process(5, args);
     */
    //\fade out
    
    // time stretch
    const char* args[128] =
    {
        
    };
    itWorked = time_stretch_process(0, args);
    //\time stretch
    
    cout << (itWorked ? "true" : "false") << endl;
    
    return 0;
}
