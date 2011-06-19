#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include "control.h"
#include "data.h"
#include "vector.h"
using namespace std;


// ==== DATA =====

Data::Data(Control* control) {
	DEBUG = control->DEBUG;
    width = control->width;
    height = control->height;
    r = new char[width * height];
    g = new char[width * height];
    b = new char[width * height];

    open_files(control->infile_n, control->outfile_n);

	double x0, y0, z0, x1, y1, z1;
	Point * position;
	Vector * view, * up;

	infile >> x0 >> y0 >> z0;
	position = new Point(x0, y0, z0); // position

	infile >> x1 >> y1 >> z1;
	view = new Vector(x0, y0, z0, x1, y1, z1); // view vector

	infile >> x1 >> y1 >> z1;
	up = new Vector(x0, y0, z0, x1, y1, z1); // up vector

    double fovy;
    infile >> fovy; // field of view in y axis

	camera = new Camera(position, view, up, fovy);

	if (DEBUG){
        cout << "CAMERA" << endl << endl;
        camera->debug();
	    cout << endl << "=========================================" << endl << endl;
        cout << "LIGHTS" << endl << endl;
    }

    int nl;
    infile >> nl;
    if (DEBUG) cout << "nl: " << nl << endl;
    for (int i = 0; i < nl ; i++) {
        double x, y, z, r, g, b, ka, kb, kc;
        Point* position;
        RGB* rgb;
	    infile >> x >> y >> z;
	    infile >> r >> g >> b;
	    infile >> ka >> kb >> kc;
	    position = new Point(x, y, z);
	    rgb = new RGB(r, g, b);
        Light* light = new Light(i, position, rgb, ka, kb, kc);
        if (DEBUG) light->debug();
        light_list.push_back(*light);
    }
    if (DEBUG) {
	    cout << endl << "=========================================" << endl << endl;
	    cout << "PIGMENTS" << endl << endl;
    }

	int np;
	infile >> np;
	if (DEBUG) cout << "np: " << np << endl;
	for (int i = 0; i < np; i++) {
	    string type;
	    infile >> type;
	    if (type.compare("SOLI") == 0) {
	        double r, g, b;
	        RGB* rgb;
	        infile >> r >> g >> b;
	        rgb = new RGB(r, g, b);
	        Pigment* pigment = new Pigment(i, type, rgb);
	        pigment_array.push_back(*pigment);
            if (DEBUG)  pigment->debug();
        }
    }
    if (DEBUG) {
	    cout << endl << "=========================================" << endl << endl;
	    cout << "FINISHES" << endl << endl;
    }

    int nf;
    infile >> nf;
	if (DEBUG) cout << "nf: " << nf << endl;
	for (int i = 0; i < nf; i++) {
	    double ka, kd, ks, alpha, kr, kt, ior;
	    infile >> ka >> kd >> ks >> alpha >> kr >> kt >> ior;
	    Finish *finish = new Finish(i, ka, kd, ks, alpha, kr, kt, ior);
	    finish_array.push_back(*finish);
	    if(DEBUG) finish->debug();
    }
    if (DEBUG) {
	    cout << endl << "=========================================" << endl << endl;
	    cout << "OBJECTS" << endl << endl;
    }

    int no;
    infile >> no;
    if (DEBUG) cout << "no: " << no << endl;
    for (int i = 0; i < no; i++) {
        int pigment_id, finish_id;
        Pigment * pigment;
        Finish * finish;
        string type;
        infile >> pigment_id >> finish_id >> type;
        pigment = &(pigment_array[pigment_id]);
        finish = &(finish_array[finish_id]);
        if(type.compare("ESFE") == 0) {
            double x, y, z, radius;
            infile >> x >> y >> z >> radius;
            Point* p = new Point(x, y, z);
            Object *sphere = new Object(i, type, pigment, finish, p, radius);
            object_list.push_back(*sphere);
            if(DEBUG) sphere->debug();
        }
        else if (type.compare("TRIA") == 0) {
            double x0, y0, z0, x1, y1, z1, x2, y2, z2;
            Point * p0, * p1, * p2;
            infile >> x0 >> y0 >> z0 >> x1 >> y1 >> z1 >> x2 >> y2 >> z2;
            p0 = new Point(x0, y0, z0);
            p1 = new Point(x1, y1, z1);
            p2 = new Point(x2, y2, z2);
            Object *triangle = new Object(i, type, pigment, finish, p0, p1, p2);
            object_list.push_back(*triangle);
            if(DEBUG) triangle->debug();
        }
    }
    if (DEBUG)
	    cout << endl << "=========================================" << endl << endl;

	infile.close();
}

void Data::open_files(char* infile_n, char* outfile_n) {
	infile.open(infile_n);
	if (!infile) {
		cout << " ERROR: Could not open: " << infile_n << endl;
		exit(1);
	}
	outfile.open(outfile_n);
	if (!outfile) {
		cout << " ERROR: Could not open/create: " << outfile_n << endl;
		exit(1);
	}
}

void Data::store_ppm()
{
    int size = width * height;

    outfile << "P6" << endl;
    outfile << width << " " << height << endl;
    outfile << 255 << endl;

    for (int i = 0; i < size; i++)
        outfile << r[i] << g[i] << b[i];
    
    outfile.close();
    
    if (DEBUG)
        cout << "Image done" << endl;
}

// ==== CAMERA ====

Camera::Camera(Point* position, Vector* view, Vector* up, double fovy) {
    view->normalize();
    up->normalize();
    this->position = position;
    this->view = view;
	this->up = view->mul(up->dot_product(view));
	this->up = up->sub(this->up);
	this->up->normalize();
	this->right = view->cross_product(this->up);
	this->fovy = fovy;
}

void Camera::debug() {
	cout << " Position: "; this->position->debug();
	cout << " View:     "; this->view->debug();
	cout << " Up:       "; this->up->debug();
	cout << " Right:    "; this->right->debug();
	cout << " FOVY:     " << this->fovy << endl;
	cout << endl;
}

// ==== LIGHT ====

Light::Light(int id, Point* position, RGB* rgb, double a, double b, double c) {
    this->id = id;
    this->position = position;
    this->rgb = rgb;
    this->a = a;
    this->b = b;
    this->c = c;
}

void Light::debug() {
    cout << " Light " << id << endl;
	cout << "  Position: "; this->position->debug();
	cout << "  RGB:      "; this->rgb->debug();
	cout << "  a b c:    ";	printf("%.6lf %.6lf %.6lf\n", a, b, c);
	cout << endl;
}

// ==== PIGMENT ====

Pigment::Pigment(int id, string type, RGB* rgb) {
    this->id = id;
    this->type = type;
    this->rgb = rgb;
}

void Pigment::debug() {
    cout << " Pigment " << id << endl;
    cout << "  Type: " << type << endl;
	cout << "  RGB: "; this->rgb->debug();
	cout << endl;
}

// ==== FINISH ====

Finish::Finish(int id, double ka, double kd, double ks, double alpha, double kr, double kt, double ior) {
    this->id = id;
    this->ka = ka;
    this->kd = kd;
    this->ks = ks;
    this->alpha = alpha;
    this->kr = kr;
    this->kt = kt;
    this->ior = ior;
}

void Finish::debug() {
    cout << " Finish  " << id << endl;
    cout << "  ka:    " << ka << endl;
    cout << "  kd:    " << kd << endl;
    cout << "  ks:    " << ks << endl;
    cout << "  alpha: " << alpha << endl;
    cout << "  kr:    " << kr << endl;
    cout << "  kt:    " << kt << endl;
    cout << "  ior:   " << ior << endl;
    cout << endl;
}

// ==== OBJECT ====

void Object::debug() {
    cout << " Object " << id << endl;
    cout << "  Type: " << type << endl;
    cout << "  Pigment: " << pigment->id << endl;
    cout << "  Finish: " << finish->id << endl;

    if (type.compare("ESFE") == 0) {
        cout << "  Position: "; this->position->debug();
        cout << "  Radius: " << radius << endl;
    }
    if (type.compare("TRIA") == 0) {
        cout << "  p0: "; this->p0->debug();
        cout << "  p1: "; this->p1->debug();
        cout << "  p2: "; this->p2->debug();
    }
    cout << endl;
}

// ==== SPHERE ====

Object::Object(int id, string type, Pigment* pigment, Finish* finish, Point* position, double radius) {
    this->id = id;
    this->type = type;
    this->pigment = pigment;
    this->finish = finish;
    this->position = position;
    this->radius = radius;
}

// ==== TRIANGLE ====

Object::Object(int id, string type, Pigment* pigment, Finish* finish, Point* p0, Point* p1, Point* p2) {
    this->id = id;
    this->type = type;
    this->pigment = pigment;
    this->finish = finish;
    this->p0 = p0;
    this->p1 = p1;
    this->p2 = p2;
}

