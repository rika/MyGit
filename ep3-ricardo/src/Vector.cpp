#include <stdio.h>
#include <cmath>
#include <iostream>
#include "vector.h"
using namespace std;

#define EPSILON 0.0001

Vector::Vector() {
    x = y = z = 0;
}

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

Matrix::Matrix(double a11, double a12, double a13,
               double a21, double a22, double a23,
               double a31, double a32, double a33) {
    row[0].x = a11; row[0].y = a12; row[0].z = a13;
    row[1].x = a21; row[1].y = a22; row[1].z = a23;
    row[2].x = a31; row[2].y = a32; row[2].z = a33;
}

Vector* Matrix::pos_mul(Vector* t) {
    return new Vector(row[0].dot_product(t), row[1].dot_product(t), row[2].dot_product(t));
}

bool Matrix::inverse() {
    double det = row[0].x * row[1].y * row[2].z - (row[0].x * row[1].z * row[2].y) +
                 row[0].y * row[1].z * row[2].x - (row[0].y * row[1].x * row[2].z) +
                 row[0].z * row[1].x * row[2].y - (row[0].z * row[1].y * row[2].x);

    if (fabs(det) < EPSILON) return false;

    row[0].x = (row[1].y * row[2].z - row[1].z * row[2].y)/det;
    row[0].y = (row[0].z * row[2].y - row[0].y * row[2].z)/det;
    row[0].z = (row[0].y * row[1].z - row[0].z * row[1].y)/det;

    row[1].x = (row[1].z * row[2].x - row[1].x * row[2].z)/det;
    row[1].y = (row[0].x * row[2].z - row[0].z * row[2].x)/det;
    row[1].z = (row[0].z * row[1].x - row[0].x * row[1].z)/det;

    row[2].x = (row[1].x * row[2].y - row[1].y * row[2].x)/det;
    row[2].y = (row[0].y * row[2].x - row[0].x * row[2].y)/det;
    row[2].z = (row[0].x * row[1].y - row[0].y * row[1].x)/det;

    return true;
}
