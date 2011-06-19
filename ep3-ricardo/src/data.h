#ifndef _DATA_H_
#define _DATA_H_

#include <fstream>
#include <list>
#include <string>
#include <vector>
#include "vector.h"
#include "control.h"
using namespace std;

#define RGB Vector
#define Point Vector

class Data;
class Camera;
class Light;
class Pigment;
class Finish;
class Object;
class Sphere;
class Triangle;


class Data {
public:
    bool DEBUG;
    int width, height;
    ifstream infile;
    ofstream outfile;
	Camera* camera;	
	list<Light> light_list; 
	vector<Pigment> pigment_array;
	vector<Finish> finish_array;
	list<Object> object_list;
	unsigned char *r, *g, *b;


	Data(Control* control);
	void open_files(char* infile_n, char* outfile_n);
	void store_ppm();
};


class Camera {
public:
    Point* position;
	Vector* view;
	Vector* up;
	Vector* right;
	double fovy;
	Camera(Point* position, Vector* view, Vector* up, double visual_field);
	void debug();
};


class Light {
public:
    int id;
	Point* position;
	RGB* rgb;
	double a;
	double b;
	double c;

	Light(int id, Point* posistion, RGB* rgb, double a, double b, double c);
	void debug();
};


class Pigment {
public:
    int id;
	string type;
	RGB* rgb;
	// supondo somente SOLI
	Pigment(int id, string type, RGB* rgb);
	void debug();
};


class Finish {
public:
    int id;
	double ka, kd, ks, alpha, kr, kt, ior;
	Finish(int id, double ka, double kd, double ks, double alpha, double kr, double kt, double ior);
	void debug();
};


class Object {
public:
    int id;
	string type;
	Pigment * pigment;
	Finish * finish;
	// sphere
	Point* position;
	double radius;
	Object(int id, string type, Pigment* pigment, Finish* finish, Point* position, double radius);
	// triangle
	Point * p0, * p1, * p2;
	Object(int id, string type, Pigment* pigment, Finish* finish, Point* p0, Point* p1, Point* p2);
	void debug();
};


#endif // _DATA_H_

