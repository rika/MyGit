#ifndef _CONTROL_H_
#define _CONTROL_H_

#include <fstream>
using namespace std;

class Control;

class Control {
public:
    bool DEBUG;
    char* infile_n;
    char* outfile_n;
    int width, height;

    Control (int argc, char* argv[]);
    void usage(char* argv[]);
};

#endif // _CONTROL_H_

