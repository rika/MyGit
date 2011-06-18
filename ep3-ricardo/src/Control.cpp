#include <iostream>
#include <fstream>

#include "control.h"

using namespace std;

class Control {
    public:
    bool DEBUG;
    char* infile_n;
    char* outfile_n;
    ifstream infile;
    ofstream outfile;
    int width, height;

    Control (int argc, char* argv[]) {
        parse_args(argc, argv);
        open_files();
    }

    void parse_args(int argc, char* argv[]) {
        if (argc < 5 || (argc >5 && strcmp(argv[5], "-d") != 0)) {
            usage(argv);
            exit(1);
        }

        infile_n  = argv[1];
        outfile_n = argv[2];
        width  = atoi(argv[3]);
        height = atoi(argv[4]);

        DEBUG = false;
        if (argc > 5) {
            DEBUG = true;
            cout << "debug on" << endl;
        }

        if (DEBUG) {
            cout << "Arquivo de entrada: " << infile_n << endl;
            cout << "Arquivo de saida: " << outfile_n << endl;
            cout << "Largura da imagem: " << width << endl;
            cout << "Altura da imagem: " << height << endl;
        }
    }

    void open_files() {
        if (DEBUG) cout << "Abrindo: " << infile_n << endl;
        infile.open(infile_n);
        if (!infile) {
            cout << "Nao foi possivel abrir: " << infile_n << endl;
            exit(1);
        }
        if (DEBUG) cout << "Abrindo/criando: " << outfile_n << endl;
        outfile.open(outfile_n);
        if (!outfile) {
            cout << "Nao foi possivel abrir/criar: " << outfile_n << endl;
            exit(1);
        }
    }

    void usage(char* argv[]) {
        cout << "USAGE: " << argv[0] << " INFILE OUTFILE WIDTH HEIGHT -d" << endl << endl;
        cout << "       INFILE   input file name" << endl;
        cout << "       OUTFILE  output file name" << endl;
        cout << "       WIDTH    output image width" << endl;
        cout << "       HEIGTH   output image height" << endl;
        cout << "       -d       turns debbuger on" << endl << endl;
    }
};

