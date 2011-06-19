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
    bool inner_flag;

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
            cout << endl;
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
                cout << "  Intesection with object " << target_obj->id << endl;
                cout << "  t = " << d_min << endl;
                cout << endl;
            }
            
            Vector* Q = p->add(ray->mul(d_min));
            
            Vector* norm = 0;
            if (target_obj->type.compare("ESFE") == 0) {
                norm = Q->sub(target_obj->position);
                norm->normalize();
            }


            // ambient
            RGB* ambient = new RGB(0, 0, 0);
            Finish* f = target_obj->finish;
            list<Light>::iterator light = (data->light_list).begin();
            if (light != (data->light_list).end()) {
                ambient = (ambient->add(light->rgb->mul(f->ka)))->mul(target_obj->pigment->rgb);
                if(DEBUG) {
                    cout << " LIGHT " << light->id << endl;
                    cout << "  Ka: " << f->ka << endl;
                    cout << "  Ambient light rgb: "; light->rgb->debug();
                    cout << "  Ambient light component: "; ambient->debug();
                    cout << endl;
                }
                light++;
            }
            
            // light dependent reflections
            RGB* spec_color = new RGB(0, 0, 0);
            RGB* difuse_color = new RGB(0, 0, 0);

            for (; light != (data->light_list).end(); light++) {
                Vector *lray = light->position->sub(Q);
                double ld = lray->abs();
                lray->normalize();

                bool visible = true;
	            list<Object>::iterator obj = (data->object_list).begin();
	            for(; obj != (data->object_list).end(); obj++) {
	                if (&(*obj) == target_obj) continue;
	                double d = intersect(Q, lray, &(*obj));
	                if (d < ld) visible = false;
                }


                if (visible) {
                    double atenuation = light->a + light->b * ld + light->c * ld * ld;

                    // difuse reflection
                    double nl = norm->dot_product(lray);
                    RGB* difuse_comp = 0;
                    if (nl > EPSILON) {
                        difuse_comp = (light->rgb->mul(target_obj->pigment->rgb))->mul(nl*f->kd/atenuation);
                        difuse_color = difuse_color->add(difuse_comp);
                    }

                    // specular reflection
                    Vector* half = lray->sub(ray);
                    half->normalize();

                    double nh = norm->dot_product(half);
                    double nh_alpha = pow(nh, f->alpha);
                    RGB* spec_comp = 0;
                    if (nh_alpha > EPSILON) {
                        double k = f->ks * nh_alpha / atenuation;
                        spec_comp = light->rgb->mul(k);
                        spec_color = spec_color->add(spec_comp);
                    }

                    if (DEBUG) {
                        cout << " LIGHT " << light->id << endl;
                        cout << "  distance: " << ld << "  atenuation: " << atenuation << endl;
                        cout << "  normal vector: "; norm->debug();
                        cout << "  light vector: "; lray->debug();
                        cout << "  light rgb: "; light->rgb->debug();
                        cout << endl;
                        cout << "  dot_product(n,l) = " << nl << endl;
                        if (difuse_comp != 0)
                            cout << "  difuse component:       "; difuse_comp->debug();
                        cout << "  partial sum (difuse)    "; difuse_color->debug();
                        cout << endl;
                        cout << "  half vector: "; half->debug();
                        cout << "  dot_product(n,h)^alpha = " << nh_alpha << endl;
                        cout << "  light rgb: "; light->rgb->debug();
                        if (spec_comp != 0)
                            cout << "  specular component:     "; spec_comp->debug();
                        cout << "  partial sum (specular): "; spec_color->debug();
                        cout << endl;
                    }
                }
            }

            RGB* color = ambient->add(spec_color)->add(difuse_color);
            if (DEBUG) {
                cout << " Total sum (difuse): "; difuse_color->debug();
                cout << " Total sum (spec):   "; spec_color->debug();
                cout << endl;
            }
            
            if (f->kr > EPSILON) {
                Vector* reflection_v = (norm->mul(ray->mul(-1)->dot_product(norm)*2))->sub(ray);

                if (DEBUG) {
                    cout << " Refraction" << endl;
                    cout << "  Kr: " << f->kr << endl;
                    cout << "  Ray vector: "; reflection_v->debug();
                    cout << endl;
                }

                RGB* reflection = (trace(Q, reflection_v, recursion+1))->mul(f->kr);
                color = color->add(reflection);
            }
/*
            RGB* refraction = (trace(Q, refraction_v, recursion+1))->mul(f->kt);
            color = color->add(reflection);
  */         
            if (DEBUG) {
                cout << " Color: "; color->debug();
            }
            return color;

        }
        else {
            RGB* bg = new RGB(0.5, 0.5, 0.5);
            if (DEBUG) {
                cout << " No intersection" << endl;
                cout << " Background: "; bg->debug();
                cout << endl;
            }
            return bg;
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
                inner_flag = true;
                return t;
            }

            t = up + sq_delta;
            if (t > 0) {
                inner_flag = true;
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
                double alpha1 = v->y;
                double alpha2 = v->z;
                if ( fabs(alpha1) < EPSILON || fabs(alpha2) < EPSILON || alpha1 + alpha2 > 1)
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
    new Tracer(data, control->DEBUG);

    return 0;
}

