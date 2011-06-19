#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include "control.h"
using namespace std;

Control::Control (int argc, char* argv[]) {
    string d_opt = "-d";
	if (argc < 5 || (argc >5 && d_opt.compare(argv[5]) != 0)) {
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
	    cout << endl << "=========================================" << endl << endl;
		cout << "DEBUG ON" << endl << endl;
	}

	if (DEBUG) {
		cout << " Infile:  " << infile_n << endl;
		cout << " Outfile: " << outfile_n << endl;
		cout << " Width:   " << width << endl;
		cout << " Height:  " << height << endl;
        cout << endl << "=========================================" << endl << endl;
	}
}


void Control::usage(char* argv[]) {
	cout << "USAGE: " << argv[0] << " INFILE OUTFILE WIDTH HEIGHT -d" << endl << endl;
	cout << "       INFILE   input file name" << endl;
	cout << "       OUTFILE  output file name" << endl;
	cout << "       WIDTH    output image width" << endl;
	cout << "       HEIGTH   output image height" << endl;
	cout << "       -d       turns debbuger on" << endl << endl;
}

