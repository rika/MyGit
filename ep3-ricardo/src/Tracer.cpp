#include <cmath>
#include <iostream>
#include <fstream>
#include "control.h"
#include "data.h"
using namespace std;

#define PI 3.14159265

class Tracer {
public:
    bool DEBUG;
    Data* data;
    char * r, * g, * b;

    Tracer(Data* data, bool DEBUG) {
        this->DEBUG = DEBUG;
        this->data = data;
        
        r = new char[data->width * data->height];
        g = new char[data->width * data->height];
        b = new char[data->width * data->height];

        int skip = 0;
        bool debug = DEBUG;
        double h = 2 * tan(data->camera->fovy/2 * PI/180);
        double w = data->width / (double) data->height * h;
        cout << "w: "<< w << " h: "<< h << endl;
	    cout << endl << "=========================================" << endl << endl;

        for (int row = 0; row < data->height; row++) {
            for (int col = 0; col < data->width; col++) {
                if (debug && skip <= 0)
                    DEBUG = true;
                else {
                    DEBUG = false;
                    skip--;
                }
                if (DEBUG) {
                    if (row != 0 || col != 0) {
                        cout << "How many debug steps would you liek to skip: ";
                        cin >> skip;
                        cout << endl;
                    }
                    cout << "PIXEL (" << row << ", " << col << ")" << endl;
                }

                ray_trace(row, col, w, h); 

                if (DEBUG)
	                cout << endl << "=========================================" << endl << endl;

            }
        }
    }

    void ray_trace(int row, int column, double w, double h) {

        double r = 0.5 + (double) row;
        double c = 0.5 + (double) column;
        double ex = (c*w/data->width) - (w/2.0);
        double ey = (h/2.0) - (r*h/data->height);

        Vector* rx = data->camera->right->mul(ex);
        Vector* ry = data->camera->up->mul(ey);
        Vector* ray = data->camera->view->add(ry)->add(rx);
        ray->normalize();

        trace(data->camera->position, ray, 0);
    }

    void trace(Point* p, Vector* ray, int recursion) {
        if (recursion >= 5) return;

        if (DEBUG) {
            cout << "Recursion: " << recursion << endl;
            cout << "Origin: "; p->debug();
            cout << "Ray:    "; ray->debug();
        }
        
        int id = -1;
        double d_min = 10000000;
	    list<Sphere>::iterator sphere = (data->sphere_list).begin();
	    for(; sphere != (data->sphere_list).end(); sphere++) {
            double d = intersect_sphere(p, ray, &(*sphere));
            if (d != -1 && d < d_min) {
                d_min = d;
                id = sphere->id;
            }
        }

	    list<Triangle>::iterator triangle = (data->triangle_list).begin();
	    for(; triangle != (data->triangle_list).end(); triangle++) {
            double d = intersect_triangle(p, ray, &(*triangle));
            if (d != -1 && d < d_min) {
                d_min = d;
                id = triangle->id;
            }
        }

        if (id != -1) {
            // calc + trace rec
        }
        else {
            if (DEBUG) {
                cout << "No intersection" << endl;
            }
            return; // fundo
        }
    }

    double intersect_sphere(Point* origin, Vector* ray, Sphere* sphere) {
        double up, pp, sq_delta, t;
        cout << "esfera! " << sphere->radius << endl;
        up = ray->dot_product(sphere->position);
        pp = sphere->position->dot_product(sphere->position);
        sq_delta = sqrt(up * up - pp + sphere->radius*sphere->radius);
        
        t = up - sq_delta;
        if (t > 0) {
            if (DEBUG) {
                cout << "Outter intersection" << endl;
                cout << "t: " << t << endl;
            }
            return t;
        }

        t = up + sq_delta;
        if (t > 0) {
            if (DEBUG) {
                cout << "Inner intersection" << endl;
                cout << "t: " << t << endl;
            }
            return t;
        }

        return -1;
    }

    double intersect_triangle(Point* origin, Vector* ray, Triangle* triangle) {
        return -1;
    }

    void StorePPM (int w, int h, char *r, char *g, char *b, char *file_name)
    {
        int size = w * h;

        FILE *file_fp;
        if ((file_fp =  fopen(file_name,"w")) == NULL) {
            printf("Can't open file %s \n",file_name);
            exit (0);
        }

        fprintf(file_fp,"%s\n%i %i\n%i\n", "P6",w,h,255);

        for (int i=0; i < size; i++) {
            putc(*r++, file_fp);
            putc(*g++, file_fp);
            putc(*b++, file_fp);
        }
        fclose(file_fp);
    }
};

int main(int argc, char* argv[]) {

    Control* control = new Control(argc, argv);
    Data* data = new Data(control->width, control->height, control->infile_n, control->outfile_n, control->DEBUG);
    Tracer* tracer = new Tracer(data, control->DEBUG);


    return 0;
}

