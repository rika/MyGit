/*
 * Projeto 2: simulador de voo 
 *
 * Descricao: Exercicio programa 2
 * Objetivo uso de geometria 3D no OpenGL

 * Author: Hitoshi (hitoshi@ime.usp.br)
 * last update: abril de 2011
 */

#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "plane.h"

/* ================================================================= */
/* compilation and system options */

#define DEBUG 1
#define FileIn "input.txt"

/* ================================================================= */
/* frustum */

double FrustumFOV = 50.0;
double FrustumASPECT = (4.0 / 3.0);
double FrustumNEAR = 10.0;
double FrustumFAR = 1000.0;

/* ================================================================= */
/* Velociy increments for translations and rotations */

double DeltaVTrans = 5.0;
double DeltaVRoll  = 2.0;

/* ================================================================= */
/* Global Variables */

int iWidth=400, iHeight=300;	    // viewport
int iMinX, iMaxX, iMinY, iMaxY;	    // sea rectangle
int iMinLX, iMaxLX, iMinLY, iMaxLY; // island rectangle
int *map;			    // elevation map
double curTime, oldTime;	    // time interval
int maxElev;                        // max elevation
int idle = 1;                       // pause the simulator

/* ================================================================= */
/* Material properties for the sea */

GLfloat mat_diffuse_water[] = {(float) 0.2,(float) 0.4, (float)1.0, (float)1.0 };
GLfloat mat_specular_water[] = {(float) 0.3, (float)0.5, (float)0.9, (float)1.0 };
GLfloat mat_shininess_water[] = {(float) 60.0 };

/* ================================================================= */
/* Material properties for the island */

GLfloat mat_diffuse_land[5][4] = {
  { (float)0.7, (float)0.7,(float) 0.7,(float) 1.0 },
  { (float)0.8, (float)0.8, (float)0.3,(float) 1.0 },
  { (float)0.4, (float)1.0, (float)0.3, (float)1.0 },
  {(float) 0.5, (float)0.9, (float)0.5, (float)1.0 },
  {(float) 1.0, (float)1.0, (float)1.0, (float)1.0 }
};
GLfloat mat_specular_land[5][4] = {
  {(float) 0.8, (float)0.8, (float)0.8, (float)1.0 },
  { (float)0.8, (float)0.8, (float)0.3, (float)1.0 },
  { (float)0.4, (float)0.9, (float)0.3, (float)1.0 },
  { (float)0.5, (float)0.9, (float)0.5, (float)1.0 },
  { (float)1.0, (float)1.0, (float)1.0, (float)1.0 }
};
GLfloat mat_shininess_land[5][1] = {
  { (float)60.0 },
  {(float) 50.0 },
  {(float) 40.0 },
  { (float)70.0 },
  { (float)95.0 }
};

/* ================================================================= */
/* Material properties for the planes */

GLfloat mat_diffuse_plane[] = { (float)0.95,(float) 0.2, (float)0.1, (float)1.0 };
GLfloat mat_specular_plane[] = { (float)0.95, (float)0.2, (float)0.1, (float)1.0 };
GLfloat mat_shininess_plane[] = { (float)80.0 };

/* ================================================================= */
/* Background */

GLfloat light_position[] = { 0.0, 0.0, 1.0, 0.0 };
GLfloat backR = 1.0;
GLfloat backG = 0.6;
GLfloat backB = 0.6;
GLfloat backA = 1.0;

