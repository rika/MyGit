#ifndef _VECTOR_H_
#define _VECTOR_H_

#include "vector.h"

class Vector;

class Vector {
public:
    double x, y, z;

    Vector();
    Vector(Vector* v);
	Vector(double x, double y, double z);
	Vector(double x0, double y0, double z0, double x1, double y1, double z1);
	void debug();
	Vector* add(Vector* v);
	Vector* sub(Vector* v);
	Vector* mul(double d);
	Vector* mul(Vector* v);
	double dot_product(Vector* v);
	Vector* cross_product(Vector* v);
	double abs();
	void normalize();
};

class Matrix {
public:
    Vector row[3];
    Matrix(double a11, double a12, double a13,
           double a21, double a22, double a23,
           double a31, double a32, double a33);

    Vector* pos_mul(Vector* t);
    bool inverse();
};

#endif // _VECTOR_H_

