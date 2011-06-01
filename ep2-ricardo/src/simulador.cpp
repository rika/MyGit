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
#include "bmp.h"

using namespace std;

// Defines
#define PI 3.14159265
#define IDLE_INIT_TIME 33   // Tempo de sleep entre frames em milisegundos
#define COS(a) cos(PI*(a)/180)
#define SIN(a) sin(PI*(a)/180)
#define IME_MAP "mapa_ime.bmp"

// Variavel global
bool paused;
int view_mode;

// textura do ime
GLuint texture;

/* **************************************************************************
    CLASSES
 ************************************************************************** */

void print_v(double v[16]) {
    int i = 0;
    for(int j = 0; j < 16; j++) {
        printf("%.2lf ", v[i]);
        i += 4;
        if(i >= 16) {
            printf("\n");
            i = i%4 + 1;
        }
    }
    printf("pos        %.2lf    %.2lf    %.2lf\n", v[12], v[13], v[14]);
    printf("z -direc   %.2lf:    %.2lf    %.2lf\n", v[8], v[9], v[10]);
    printf("y up       %.2lf    %.2lf    %.2lf\n", v[4], v[5], v[6]);
    printf("x right    %.2lf    %.2lf    %.2lf\n", v[0], v[1], v[2]);
    printf("\n");
}   

class Frame {
private:
    void set_id(double v[16]) {
        for(int i = 0; i < 4; i++) {
            for(int j = 0; j < 4; j++)
                v[4*i+j] = 0;
            v[4*i+i] = 1;
        }
    }    
public:
    double v[16];
    
    Frame (double x, double y, double z, double rx, double ry, double rz) {
        set_id(v);
        translate(x, y, z);
        rotate(rx, ry, rz);
    }

    // pos mult
    void mult(double op[16]) {
        double vt[16];
        double sum;
        for(int i = 0; i < 4; i++) {
            for(int j = 0; j < 4; j++) {
                sum = 0;
                for(int k = 0; k < 4; k++)
                    sum += v[4*k+i]*op[4*j+k];
                vt[4*j+i] = sum;
            }
        }
        for(int i = 0; i < 16; i++) v[i] = vt[i];
    }
    
    // RzRyRx
    void rotate(double rx, double ry, double rz) {
        double op[16];
        op[0] = COS(rz)*COS(ry);
        op[1] = SIN(rz)*COS(ry);
        op[2] = -1*SIN(ry);
        op[3] = 0;

        op[4] = -1*SIN(rz)*COS(rx) + COS(rz)*SIN(ry)*SIN(rx);
        op[5] = COS(rz)*COS(rx) + SIN(rz)*SIN(ry)*SIN(rx);
        op[6] = COS(ry)*SIN(rx);
        op[7] = 0;

        op[8] = SIN(rz)*SIN(rx) + COS(rz)*SIN(ry)*COS(rx);
        op[9] = -1*COS(rz)*SIN(rx) + SIN(rz)*SIN(ry)*COS(rx);
        op[10] = COS(ry)*COS(rx);
        op[11] = 0;

        op[12] = op[13] = op[14] = 0;
        op[15] = 1;

        mult(op);
    }
    
    void translate(double x, double y, double z) {
        double op[16];
        set_id(op);
        op[12] = x;
        op[13] = y;
        op[14] = z;
        mult(op);
    }
    
    double determinant() {
        double value;
        value =
        v[3]*v[6]*v[9] *v[12] - v[2]*v[7]*v[9] *v[12] - v[3]*v[5]*v[10]*v[12] + v[1]*v[7]*v[10]*v[12] +
        v[2]*v[5]*v[11]*v[12] - v[1]*v[6]*v[11]*v[12] - v[3]*v[6]*v[8] *v[13] + v[2]*v[7]*v[8] *v[13] +
        v[3]*v[4]*v[10]*v[13] - v[0]*v[7]*v[10]*v[13] - v[2]*v[4]*v[11]*v[13] + v[0]*v[6]*v[11]*v[13] +
        v[3]*v[5]*v[8] *v[14] - v[1]*v[7]*v[8] *v[14] - v[3]*v[4]*v[9] *v[8]  + v[0]*v[7]*v[9] *v[14] +
        v[1]*v[4]*v[11]*v[14] - v[0]*v[5]*v[11]*v[14] - v[2]*v[5]*v[8] *v[15] + v[1]*v[6]*v[8] *v[15] +
        v[2]*v[4]*v[9] *v[15] - v[0]*v[6]*v[9] *v[15] - v[1]*v[4]*v[10]*v[15] + v[0]*v[5]*v[10]*v[15];
        return value;
    }

    void inverse(double inv[16]) {
        inv[0]  = v[6]*v[11]*v[13] - v[7]*v[10]*v[13] + v[7]*v[9]*v[14] - v[5]*v[11]*v[14] - v[6]*v[9]*v[15] + v[5]*v[10]*v[15];
        inv[1]  = v[3]*v[10]*v[13] - v[2]*v[11]*v[13] - v[3]*v[9]*v[14] + v[1]*v[11]*v[14] + v[2]*v[9]*v[15] - v[1]*v[10]*v[15];
        inv[2]  = v[2]*v[7] *v[13] - v[3]*v[6] *v[13] + v[3]*v[5]*v[14] - v[1]*v[7] *v[14] - v[2]*v[5]*v[15] + v[1]*v[6] *v[15];
        inv[3]  = v[3]*v[6] *v[9]  - v[2]*v[7] *v[9]  - v[3]*v[5]*v[10] + v[1]*v[7] *v[10] + v[2]*v[5]*v[11] - v[1]*v[6] *v[11];
        inv[4]  = v[7]*v[10]*v[12] - v[6]*v[11]*v[12] - v[7]*v[8]*v[14] + v[4]*v[11]*v[14] + v[6]*v[8]*v[15] - v[4]*v[10]*v[15];
        inv[5]  = v[2]*v[11]*v[12] - v[3]*v[10]*v[12] + v[3]*v[8]*v[14] - v[0]*v[11]*v[14] - v[2]*v[8]*v[15] + v[0]*v[10]*v[15];
        inv[6]  = v[3]*v[6] *v[12] - v[2]*v[7] *v[12] - v[3]*v[4]*v[14] + v[0]*v[7] *v[14] + v[2]*v[4]*v[15] - v[0]*v[6] *v[15];
        inv[7]  = v[2]*v[7] *v[8]  - v[3]*v[6] *v[8]  + v[3]*v[4]*v[10] - v[0]*v[7] *v[10] - v[2]*v[4]*v[11] + v[0]*v[6] *v[11];
        inv[8]  = v[5]*v[11]*v[12] - v[7]*v[9] *v[12] + v[7]*v[8]*v[13] - v[4]*v[11]*v[13] - v[5]*v[8]*v[15] + v[4]*v[9] *v[15];
        inv[9]  = v[3]*v[9] *v[12] - v[1]*v[11]*v[12] - v[3]*v[8]*v[13] + v[0]*v[11]*v[13] + v[1]*v[8]*v[15] - v[0]*v[9] *v[15];
        inv[10] = v[1]*v[7] *v[12] - v[3]*v[5] *v[12] + v[3]*v[4]*v[13] - v[0]*v[7] *v[13] - v[1]*v[4]*v[15] + v[0]*v[5] *v[15];
        inv[11] = v[3]*v[5] *v[8]  - v[1]*v[7] *v[8]  - v[3]*v[4]*v[9]  + v[0]*v[7] *v[9]  + v[1]*v[4]*v[11] - v[0]*v[5] *v[11];
        inv[12] = v[6]*v[9] *v[12] - v[5]*v[10]*v[12] - v[6]*v[8]*v[13] + v[4]*v[10]*v[13] + v[5]*v[8]*v[14] - v[4]*v[9] *v[14];
        inv[13] = v[1]*v[10]*v[12] - v[2]*v[9] *v[12] + v[2]*v[8]*v[13] - v[0]*v[10]*v[13] - v[1]*v[8]*v[14] + v[0]*v[9] *v[14];
        inv[14] = v[2]*v[5] *v[12] - v[1]*v[6] *v[12] - v[2]*v[4]*v[13] + v[0]*v[6] *v[13] + v[1]*v[4]*v[14] - v[0]*v[5] *v[14];
        inv[15] = v[1]*v[6] *v[8]  - v[2]*v[5] *v[8]  + v[2]*v[4]*v[9]  - v[0]*v[6] *v[9]  - v[1]*v[4]*v[10] + v[0]*v[5] *v[10];
        
        double det = determinant();
        for(int i = 0; i < 16; i++) inv[i] /= det;
    }

    void print() {
        int i = 0;
        for(int j = 0; j < 16; j++) {
            printf("%.2lf ", v[i]);
            i += 4;
            if(i >= 16) {
                printf("\n");
                i = i%4 + 1;
            }
        }
        printf("pos        %.2lf    %.2lf    %.2lf\n", v[12], v[13], v[14]);
        printf("z -direc   %.2lf    %.2lf    %.2lf\n", v[8], v[9], v[10]);
        printf("y up       %.2lf    %.2lf    %.2lf\n", v[4], v[5], v[6]);
        printf("x right    %.2lf    %.2lf    %.2lf\n", v[0], v[1], v[2]);
        printf("\n");
    }   
};


class Airplane {
public:
    int id;
    Frame* frame;
    double v; // velocidade
    // rotacoes
    double roll;
    double yaw;
    double pitch;

    Airplane(int id, double x, double y, double z, double rx, double ry, double rz, double v) {
        this->id = id;
        this->v = v;
        roll = yaw = pitch = 0;
        frame = new Frame(x, y, z, rx, ry, rz);
    }
    void display() {
        glPushMatrix();
        glMultMatrixd(frame->v);
        glBegin(GL_TRIANGLES);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse_plane);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular_plane);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess_plane);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glVertex3f(-1.0f, 0.0f, 0.0f); 
        glVertex3f( 1.0f, 0.0f, 0.0f);
        glVertex3f( 0.0f, 0.0f, -1.5f);
        glEnd();
        glPopMatrix();
    }
    void move() {
        frame->translate(0, 0, -v);
        frame->rotate(pitch, yaw, roll);
    }
    void look() {
        if(view_mode == 0)      gluLookAt(0, 0, 0, 0, 0, -1, 0, 1, -1);
        else if(view_mode == 1) gluLookAt(0, 10, 20, 0, 0, 0, 0, 1, 0);
        else                    gluLookAt(0, 20, 30, 0, 0, 0, 0, 1, 0);

        double inv[16];
        frame->inverse(inv);
        glMultMatrixd(inv);
    }
};


// lista globais das aeronaves
list<Airplane> ap_list;

// aeronave da visao atual
list<Airplane>::iterator AP;


/* **************************************************************************
    FUNCOES
 ************************************************************************** */

void init();
void display_elevation_map();
void display_map();
void look_and_project();
void display();
void reshape();
void step(int);
void mouse_motion(int, int);
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

    view_mode = 0;
    paused = false;

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

    AP = ap_list.begin();
    fclose(input);

    // mapa do ime
    Image* img;
    img = (Image *) malloc(sizeof(Image));
    if(ImageLoad(IME_MAP, img) == 0)
        printf("Problema ao gerar o mapa de %s.\n", IME_MAP);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, img->sizeX, img->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, img->data);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    if (img) {
        if (img->data) free(img->data);
        free(img);
    }
    
}


/* **************************************************************************
   FUNCOES DE DISPLAY E RESHAPE
 ************************************************************************** */


void display_elevation_map() {
    glPushMatrix();
    glTranslatef((float)iMinLX, (float)iMinLY, 0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse_land[2]);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular_land[2]);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess_land[2]);
    for(int i = 0; i < iMaxLX-iMinLX-1; i++) {
        for(int j = 0; j < iMaxLY-iMinLY-1; j++) {
            glBegin(GL_TRIANGLES);
            glNormal3f((float)(map[i][j]-map[i+1][j]), (float)(map[i][j]-map[i][j+1]), 1.0f);
            glVertex3f(i, j, map[i][j]);
            glVertex3f(i, j+1, map[i][j+1]);
            glVertex3f(i+1, j, map[i+1][j]);
            glEnd();

            glBegin(GL_TRIANGLES);
            glNormal3f((float)(map[i+1][j+1]-map[i][j+1]), (float)(map[i+1][j]-map[i+1][j+1]), 1.0f);
            glVertex3f(i+1, j, map[i+1][j]);
            glVertex3f(i, j+1, map[i][j+1]);
            glVertex3f(i+1, j+1, map[i+1][j+1]);
            glEnd();
        }
    }
    glPopMatrix();
}

void display_map() {
    glPushMatrix();
    glBegin(GL_QUADS);
    glBindTexture(GL_TEXTURE_2D, texture);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse_water);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular_water);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess_water);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f((float)iMinX, (float)iMinY, 0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f((float)iMaxX, (float)iMinY, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f((float)iMaxX, (float)iMaxY, 0.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f((float)iMinX, (float)iMaxY, 0.0f);
    glEnd();
    glPopMatrix();
}

void look_and_project() {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    AP->look();
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
    display_elevation_map();
    display_map();

    glutSwapBuffers();
}

void reshape(int w, int h) {
    iWidth = w;
    iHeight = h;
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

    for (a = ap_list.begin(); a != ap_list.end(); a++)
        a->move();
    glutPostRedisplay();
    if (!paused)
        glutTimerFunc(t, step, t);
}


/* **************************************************************************
   FUNCOES DE INTERFACE COM O USUARIO
 ************************************************************************** */


void mouse_motion (int x, int y) {
    if(x >= 0 && x < iWidth && y >= 0 && y < iHeight) { 
        float coef = 2.5;
        AP->pitch = coef/(float)iHeight * ((float)iHeight/2.0 - (float)y);
        AP->yaw = coef/(float)iWidth * ((float)iWidth/2.0 - (float)x);
    }
}

void keyboard(unsigned char key, int x, int y) {
    switch(key) {
        case 'q':
            exit(0);
        case 'Q':
            exit(0);
        case 'a':
            AP->v = AP->v + DeltaVTrans/IDLE_INIT_TIME;
            break;
        case 'A':
            AP->v = AP->v + DeltaVTrans/IDLE_INIT_TIME;
            break;
        case 's':
            AP->v = AP->v - DeltaVTrans/IDLE_INIT_TIME;
            break;
        case 'S':
            AP->v = AP->v - DeltaVTrans/IDLE_INIT_TIME;
            break;
        case 'd':
            AP->v = 0;
            break;
        case 'D':
            AP->v = 0;
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
            AP++;
            if(AP == ap_list.end())
                AP = ap_list.begin();
            break;
        case 'V':
            AP++;
            if(AP == ap_list.end())
                AP = ap_list.begin();
            break;
        case 'b':
            view_mode = (view_mode+1)%3;
            break;
        case 'B':
            view_mode = (view_mode+1)%3;
            break;
        case 'z':
            AP->roll += DeltaVRoll;
            break;
        case 'Z':
            AP->roll += DeltaVRoll;
            break;
        case 'c':
            AP->roll -= DeltaVRoll;
            break;
        case 'C':
            AP->roll -= DeltaVRoll;
            break;
        case 'x':
            AP->roll = AP->yaw = AP->pitch = 0;
            break;
        case 'X':
            AP->roll = AP->yaw = AP->pitch = 0;
            break;
        default:
            break;
    }
    glutPostRedisplay();
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
    glutPassiveMotionFunc(mouse_motion);

    glutMainLoop();

    return 0;
}

