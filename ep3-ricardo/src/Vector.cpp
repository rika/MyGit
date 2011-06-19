#include <stdio.h>
#include <cmath>
#include <iostream>
#include "vector.h"
using namespace std;


Vector::Vector(Vector* v) {
    x = v->x;
    y = v->y;
    z = v->z;
}

Vector::Vector(double x, double y, double z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

Vector::Vector(double x0, double y0, double z0, double x1, double y1, double z1) {
    this->x = x1 - x0;
    this->y = y1 - y0;
    this->z = z1 - z0;
}

void Vector::debug() {
    printf("%.6lf %.6lf %.6lf\n", x, y, z);
}

Vector* Vector::add(Vector* v) {
    return new Vector(x + v->x, y + v->y, z + v->z);
}

Vector* Vector::sub(Vector* v) {
    return new Vector(x - v->x, y - v->y, z - v->z);
}

Vector* Vector::mul(double d) {
    return new Vector(d*x, d*y, d*z);
}

double Vector::dot_product(Vector* v) {
    return this->x * v->x + this->y * v->y + this->z * v->z;
}

Vector* Vector::cross_product(Vector* v) {
    return new Vector(y*v->y - z*v->y, z*v->x - x*v->z, x*v->y - y*v->x);
}

double Vector::abs() {
    return sqrt(this->dot_product(this));
}

void Vector::normalize() {
    double a = abs(); 
    x /= a;
    y /= a;
    z /= a;
}


