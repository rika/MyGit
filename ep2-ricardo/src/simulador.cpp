/* Ep1 - MAC0420 Computacao grafica
 *
 * Nome: Ricardo Juliano Mesquita Silva Oda
 * NUSP: 6514223
 *
 * Operação:
 *
 * TECLADO
 * a: acelera a nave atual
 * s: acelera negativamente a nave atual
 * d: para a nave atual
 * q: termina o programa
 * p: pausa / despausa
 * v: troca de aeronave
 *
 */


#ifdef LINUX
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

#ifdef MACOS
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string>
#include <sstream>
#include <list>
#include "ep2.h"

using namespace std;

// Defines
#define IDLE_INIT_TIME 33   // Tempo de sleep entre frames em milisegundos

// Variavel global
bool paused = false;

/* **************************************************************************
    CLASSES
 ************************************************************************** */


class Airplane {
public:
    int id;
    double x, y, z;
    double rx, ry, rz;
    double v;
    double frame[16];
    Airplane(int id, double x, double y, double z, double rx, double ry, double rz, double v) {
        this->id = id;
        this->x = x;
        this->y = y;
        this->z = z;
        this->rx = rx;
        this->ry = ry;
        this->rz = rz;
        this->v = v;
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        glRotated(-rx, 1, 0, 0);
        glRotated(-ry, 0, 1, 0);
        glRotated(-rz, 0, 0, 1);
        glTranslated(-x, -y, -z);
        glGetDoublev(GL_MODELVIEW_MATRIX, frame);
        glPopMatrix();
    }
    void display() {
        glBegin(GL_TRIANGLES);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse_plane);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular_plane);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess_plane);
        glColor3f(1.0f, 1.0f, 1.0f);
        glPushMatrix();
        glTranslated(x, y, z);
        glRotated(rx, 1, 0, 0);
        glRotated(ry, 0, 1, 0);
        glRotated(rz, 0, 0, 1);
        glNormal3f(0.0f, 0.0f, 1.0f);
        glVertex3f(-1.0f, 0.0f, 0.5f); 
        glVertex3f( 1.0f, 0.0f, 0.5f);
        glVertex3f( 0.0f, 0.0f, -0.5f);
        glEnd();
        glPopMatrix();
    }
    void move() {
        x -= frame[2]*v;
        y -= frame[6]*v;
        z -= frame[10]*v;
        
        //glPushMatrix();
        //glMatrixMode(GL_MODELVIEW);
        //glLoadMatrixd(frame);
        //glTranslated(x, y, z);
        //glGetDoublev(GL_MODELVIEW_MATRIX, frame);
        //glPopMatrix();
        //printf("MOVE:\n");
        //for(int i = 0; i < 16; i++) {printf("%.2lf ", frame[i]); if((i+1)%4==0) printf("\n");}
        //glPushMatrix();
    }
    void look() {
        GLdouble a[16] = {1, 0, 0, 0, 0, 0, 1, -150, 0, -1, 0, 50, 0, 0, 0, 1};

        gluLookAt(0, 0, 0, 0, 0, -1, 0, 1, -1);
        //gluLookAt(0, 0, 0, 0, 1, 0, 0, 1, 1);
        glGetDoublev(GL_MODELVIEW_MATRIX, a);
        printf("antes:\n");
        for(int i = 0; i < 16; i++) {printf("%.2lf ", a[i]); if((i+1)%4==0) printf("\n");}
        //glPushMatrix();
        glLoadIdentity();
        glRotated(-rx, 1, 0, 0);
        glRotated(-ry, 0, 1, 0);
        glRotated(-rz, 0, 0, 1);
        glTranslated(-x, -y, -z);
        //glGetDoublev(GL_MODELVIEW_MATRIX, frame);
        //glPopMatrix();
        //glMultMatrixd(frame);


        //glLoadIdentity();
        //GLdouble b[16] = {1, 0, 0, 0, 0, 0, -1, 0, 0, 1, 0, 0, 0, -50, -150, 1};
        //glGetDoublev(GL_MODELVIEW_MATRIX, a);
        //printf("trans:\n");
        //for(int i = 0; i < 16; i++) {printf("%.2lf ", a[i]); if((i+1)%4==0) printf("\n");}

        //glPushMatrix();
        //glLoadIdentity();
        //glMultMatrixd(frame);
        //glGetDoublev(GL_MODELVIEW_MATRIX, a);
        //printf("multi:\n");
        //for(int i = 0; i < 16; i++) {printf("%.2lf ", a[i]); if((i+1)%4==0) printf("\n");}
        //glPopMatrix();

        //glTranslated(0, 0, -25);
        //glGetDoublev(GL_MODELVIEW_MATRIX, a);
        //printf("trans: ");
        //for(int i = 0; i < 16; i++) printf("%.2lf ", a[i]); printf("\n");
        
    }
};


// lista globais das aeronaves
list<Airplane> ap_list;

// aeronave da visao atual
list<Airplane>::iterator actual_ap;


/* **************************************************************************
    FUNCOES
 ************************************************************************** */


void init();
void display();
void step(int);
void keyboard(unsigned char, int, int);


/* **************************************************************************
    INICIALIZACAO E RESET
 ************************************************************************** */


void init() {
    glClearColor(backR, backG, backB, backA);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_FLAT);

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);

    FILE* input = fopen(FileIn, "r");
    int val;
    // retangulo do mar
    val = fscanf(input, "%d %d", &iMinX, &iMaxX);
    val = fscanf(input, "%d %d", &iMinY, &iMaxY);
    // retangulo da ilha
    val = fscanf(input, "%d %d", &iMinLX, &iMaxLX);
    val = fscanf(input, "%d %d", &iMinLY, &iMaxLY);

    char file[128];
    val = fscanf(input, "%s", file);
    FILE* terrain = fopen(file, "r");

    // mapa de elevacao
    maxElev = 0;
    map = (int**) malloc((iMaxLX-iMinLX)*sizeof(int*));
    for(int i = 0; i < iMaxLX-iMinLX; i++) {
        map[i] = (int*) malloc((iMaxLY-iMinLY)*sizeof(int));
        for(int j = 0; j < iMaxLY-iMinLY; j++) {
            val = fscanf(terrain, "%d", &map[i][j]);
            if(map[i][j] > maxElev) maxElev = map[i][j];
        }
    }
    fclose(terrain);

    // avioes
    Airplane* a;
    int n;
    val = fscanf(input, "%d", &n);
    for(int i = 0; i < n; i++) {
        double x, y, z, rx, ry, rz, v;
        val = fscanf(input, "%lf %lf %lf %lf %lf %lf %lf", &x, &y, &z, &rx, &ry, &rz, &v);
        a = new Airplane(i, x, y, z, rx, ry, rz, v);
        ap_list.push_back(*a);
    }

    actual_ap = ap_list.begin();
    fclose(input);
}


/* **************************************************************************
   FUNCOES DE DISPLAY E RESHAPE
 ************************************************************************** */

void display_map() {
    glPushMatrix();
    glTranslatef((float)iMinLX, (float)iMinLY, 0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse_land[2]);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular_land[2]);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess_land[2]);
    for(int i = 0; i < iMaxLX-iMinLX-1; i++) {
        for(int j = 0; j < iMaxLY-iMinLY-1; j++) {
            glBegin(GL_TRIANGLES);
            glNormal3f((float)map[i+1][j]-map[i][j], (float)map[i][j+1]-map[i][j], -1.0);
            glColor3f(0, (float)map[i][j]/(float)maxElev, 0);
            glVertex3f(i, j, map[i][j]);
            glColor3f(0, (float)map[i][j+1]/(float)maxElev, 0);
            glVertex3f(i, j+1, map[i][j+1]);
            glColor3f(0, (float)map[i+1][j]/(float)maxElev, 0);
            glVertex3f(i+1, j, map[i+1][j]);
            glEnd();

            glBegin(GL_TRIANGLES);
            glNormal3f((float)map[i+1][j+1]-map[i][j+1], (float)map[i+1][j]-map[i+1][j+1], -1.0);
            glColor3f(0, (float)map[i+1][j]/(float)maxElev, 0);
            glVertex3f(i+1, j, map[i+1][j]);
            glColor3f(0, (float)map[i][j+1]/(float)maxElev, 0);
            glVertex3f(i, j+1, map[i][j+1]);
            glColor3f(0, (float)map[i+1][j+1]/(float)maxElev, 0);
            glVertex3f(i+1, j+1, map[i+1][j+1]);
            glEnd();
        }
    }
    glPopMatrix();
}

void display_ocean() {
    glBegin(GL_QUADS);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse_water);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular_water);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess_water);
    glColor3f(0.0, 0.0, 1.0);
    glNormal3f(0.0, 0.0, 1.0);
    glVertex3i(iMinX, iMinY, 0);
    glVertex3i(iMinX, iMaxY, 0);
    glVertex3i(iMaxX, iMaxY, 0);
    glVertex3i(iMaxX, iMinY, 0);
    glEnd();
}

void look_and_project() {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    actual_ap->look();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(FrustumFOV, FrustumASPECT, FrustumNEAR, FrustumFAR);
    glMatrixMode(GL_MODELVIEW);

}

void display() {
    glClearColor(backR, backG, backB, backA);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    look_and_project();
   
    list<Airplane>::iterator a;
    for(a = ap_list.begin(); a != ap_list.end(); a++)
        a->display();
    display_ocean();
    display_map();


    glutSwapBuffers();
}


void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(FrustumFOV, FrustumASPECT, FrustumNEAR, FrustumFAR);
            
}


/* **************************************************************************
   FUNCAO DE ANIMACAO
 ************************************************************************** */


void step(int t) {
    list<Airplane>::iterator a;

    for (a = ap_list.begin(); a != ap_list.end(); a++) {
        a->move();
    }
    glutPostRedisplay();
    if (!paused)
        glutTimerFunc(t, step, t);
    
    printf("AVIAO %d: vel: %.2f pos: (%.2f %.2f %.2f)\n",actual_ap->id, actual_ap->v, actual_ap->x, actual_ap->y, actual_ap->z);
}

/* **************************************************************************
   FUNCOES DE INTERFACE COM O USUARIO
 ************************************************************************** */


void keyboard(unsigned char key, int x, int y) {
    switch(key) {
        case 'q':
            exit(0);
        case 'Q':
            exit(0);
        case 'a':
            actual_ap->v = actual_ap->v + DeltaVTrans;
            glutPostRedisplay();
            break;
        case 'A':
            actual_ap->v = actual_ap->v + DeltaVTrans;
            glutPostRedisplay();
            break;
        case 's':
            actual_ap->v = actual_ap->v - DeltaVTrans;
            glutPostRedisplay();
            break;
        case 'S':
            actual_ap->v = actual_ap->v - DeltaVTrans;
            glutPostRedisplay();
            break;
        case 'd':
            actual_ap->v = 0;
            glutPostRedisplay();
            break;
        case 'D':
            actual_ap->v = 0;
            glutPostRedisplay();
            break;
        case 'p':
            if(paused) {
                paused = false;
                glutTimerFunc(0, step, IDLE_INIT_TIME);
            }
            else
                paused = true;
            break;
        case 'P':
            if(paused) {
                paused = false;
                glutTimerFunc(0, step, IDLE_INIT_TIME);
            }
            else
                paused = true;
            break;
        case 'v':
            actual_ap++;
            if(actual_ap == ap_list.end())
                actual_ap = ap_list.begin();
            break;
        case 'V':
            actual_ap++;
            if(actual_ap == ap_list.end())
                actual_ap = ap_list.begin();
            break;
        default:
            break;
    }
}


/* **************************************************************************
   MAIN
 ************************************************************************** */


int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(iWidth, iHeight);
    glutCreateWindow(argv[0]);

    init();

    glutDisplayFunc(display);
    glutTimerFunc(IDLE_INIT_TIME, step, IDLE_INIT_TIME);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);

    glutMainLoop();

    return 0;
}
