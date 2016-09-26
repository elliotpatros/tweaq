#include "strip_silence_at_end.h"
#include "m_tweaq.h"

void test(int argc, const char* argv[])
{
    for (int i = 0; i < argc; ++i)
    {
        printf("%d: %s\n", i, argv[i]);
    }
}

#include <iostream>
#include <memory>
using std::cout;
using std::endl;
int main(int argc, const char* argv[])
{
    std::unique_ptr<t_parameter> p(new t_parameter);
    setParameterName(*p, "threshold");
    setParameterLabels(*p, 4, "label one", "a second label awakens the mind", "tr33labls", "farts");
    
    cout << p->name << endl;
    for (int i = 0; i < p->nLabels; ++i)
    {
        cout << p->labels[i] << endl;
    }
    
    return 0;
}
