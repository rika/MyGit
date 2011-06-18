#ifdef _CONTROL_H_
#define _CONTROL_H_

class Control;

class Control {
    public:
    bool DEBUG;
    char* infile_n;
    char* outfile_n;
    ifstream infile;
    ofstream outfile;
    int width, height;

    Control (int argc, char* argv[]);
    void parse_args(int argc, char* argv[]);
    void open_files();
    void usage(char* argv[]);
};

#endif // _CONTROL_H_
