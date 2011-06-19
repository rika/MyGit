#include <cmath>
#include <iostream>
#include <fstream>
#include "control.h"
#include "data.h"
using namespace std;

#define PI 3.14159265
#define EPSILON 0.0001

class Tracer {
public:
    bool DEBUG;
    Data* data;
    RGB* rgb;

    Tracer(Data* data, bool DEBUG) {
        this->DEBUG = DEBUG;
        this->data = data;
        
        rgb = new RGB(0, 0, 0);

        int skip = 0;
        bool debug = DEBUG;
        double h = 2 * tan(data->camera->fovy/2 * PI/180);
        double w = data->width / (double) data->height * h;

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
        data->store_ppm();
    }

    void ray_trace(int row, int column, double w, double h) {

        double r = 0.5 + (double) row;
        double c = 0.5 + (double) column;
        double ex = (w/2.0) - (c*w/data->width);
        double ey = (r*h/data->height) - (h/2.0);

        Vector* rx = data->camera->right->mul(ex);
        Vector* ry = data->camera->up->mul(ey);
        Vector* ray = data->camera->view->add(ry)->add(rx);
        ray->normalize();

        rgb->x = rgb->y = rgb->z = 0;
        rgb = rgb->add(trace(data->camera->position, ray, 0));

        if (DEBUG) {
            cout << "  Color: "; rgb->debug();
        }

        int index = row*data->width + column;
        data->r[index] = (int)(255*rgb->x);
        data->g[index] = (int)(255*rgb->y);
        data->b[index] = (int)(255*rgb->z);
    }

    RGB* trace(Point* p, Vector* ray, int recursion) {
        if (recursion >= 5) return new RGB(0, 0, 0);

        if (DEBUG) {
            cout << " Recursion: " << recursion << endl;
            cout << " Origin: "; p->debug();
            cout << " Ray:    "; ray->debug();
        }
        
        Object * target_obj = 0;
        double d = -1;
        double d_min = 10000000;
	    list<Object>::iterator obj = (data->object_list).begin();
	    for(; obj != (data->object_list).end(); obj++) {
	        d = intersect(p, ray, &(*obj));

            if (d != -1 && d < d_min) {
                d_min = d;
                target_obj = &(*obj);
            }
        }

        if (target_obj != 0) {
            if (DEBUG) {
                cout << "  with object " << target_obj->id << endl;
            }
            return new RGB(target_obj->pigment->rgb);
        }
        else {
            if (DEBUG) {
                cout << "  No intersection" << endl;
            }
            return new RGB(0.5, 0.5, 0.5); // BACKGROUND
        }
    }

    double intersect(Point* origin, Vector* ray, Object *obj) {
	    if (obj->type.compare("ESFE") == 0) {
            Vector * p;
            double up, pp, r, sq_delta, t;
            p = obj->position->sub(origin);
            up = ray->dot_product(p);
            pp = p->dot_product(p);
            r =  obj->radius;
            sq_delta = sqrt(up * up - pp + r*r);
            
            t = up - sq_delta;
            if (t > 0) {
                if (DEBUG) {
                    cout << "  Outter intersection" << endl;
                    cout << "  t: " << t << endl;
                }
                return t;
            }

            t = up + sq_delta;
            if (t > 0) {
                if (DEBUG) {
                    cout << "  Inner intersection" << endl;
                    cout << "  t: " << t << endl;
                }
                return t;
            }

            return -1;
        }
        else if (obj->type.compare("TRIA") == 0) {
            Matrix* M = new Matrix(-ray->x, obj->p1->x - obj->p0->x, obj->p2->x - obj->p0->x,
                                   -ray->y, obj->p1->y - obj->p0->y, obj->p2->y - obj->p0->y,
                                   -ray->z, obj->p1->z - obj->p0->z, obj->p2->z - obj->p0->z);

            if (M->inverse()) {
                Vector* v = M->pos_mul(obj->p0);
                double alfa1 = v->y;
                double alfa2 = v->z;
                if ( fabs(alfa1) < EPSILON || fabs(alfa2) < EPSILON || alfa1 + alfa2 > 1)
                    return -1;

                return v->x;
            }
        }
        return -1;
    }
};

int main(int argc, char* argv[]) {

    Control* control = new Control(argc, argv);
    Data* data = new Data(control);
    Tracer* tracer = new Tracer(data, control->DEBUG);


    return 0;
}

