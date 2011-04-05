//============================================================================
// Name        : Ep1.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C, Ansi-style
//============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <string>
#include <iostream>
#include <sstream>

using namespace std;

class Myx {
	int x;
public:
	void set_x (int vx) {
		x = vx;
	}
	int get_x () {
		return x;
	}
};

int main(void)
{
	Myx myx;
	myx.set_x(10);

	cout << myx.get_x() << endl;
	return EXIT_SUCCESS;
}
