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
#include <list>

using namespace std;

#define WIDTH 640
#define HEIGHT 480
#define T_SIDE 40
#define B_SIDE 6
#define VMIN 1
#define VMAX 10
#define IDLE_INIT_TIME 33

// Devolve um numero aleatorio entre 'low' e 'high'
#define RANDOM(low, high) ((int)(low + ((double) rand () / ((double) RAND_MAX + 1)) * (high - low + 1)))

bool paused = false;

float ColorDepthTablef[10][3]={
    {0.00, 1.00, 1.00},
    {0.00, 0.85, 0.85},
    {0.00, 0.70, 0.70},
    {0.00, 0.25, 0.25},
    {0.00, 0.15, 0.15},
    // exploding
    {1.00, 0.00, 0.00},
    {0.90, 0.00, 0.00},
    {0.80, 0.00, 0.00},
    {0.70, 0.00, 0.00},
    {0.60, 0.00, 0.00}
};

class Square {
protected:
    int side, depth, x, y;
public:
    Square(int x, int y, int d) {
        this->x = x;
        this->y = y;
        this->depth = d;
    }
    int get_depth() {
        return depth%5;
    }
    void display() {
        glPushMatrix();
        glTranslatef(x, y, 0);
        glColor3fv(ColorDepthTablef[depth]);
        glRectf(-side/2, -side/2, side/2, side/2);
        glPopMatrix();
    }
};

class Treco: public Square {
    int vx, vy;
public:
    Treco(int x, int y, int d): Square(x, y, d) {
        side = T_SIDE;
        vx = (VMIN + RANDOM(0, VMAX - VMIN)) * (RANDOM(0, 1)?1:-1);
        vy = (VMIN + RANDOM(0, VMAX - VMIN)) * (RANDOM(0, 1)?1:-1);
    }
    void move() {
        if      (x + vx > WIDTH-(side/2)) vx *= -1;
        else if (x + vx < (side/2))       vx *= -1;
        else x += vx;

        if      (y + vy > HEIGHT-(side/2)) vy *= -1;
        else if (y + vy < (side/2))        vy *= -1;
        else y += vy;
    }
    void blow() {
        depth += 5;
    }
    friend class Bomb;
};

class Bomb: public Square {
    int it;
public:
    Bomb(int x, int y): Square(x, y, 0) {
        side = B_SIDE;
        it = 0;
    }
    bool fall() {
        it = (it + 1)%15; // mudar
        if (it == 0) {
            depth++;
            if (depth >= 5)
                return true;
            else
                return false;
        }
        return false;
    }
    bool touch(Treco t) {
        if (depth == t.depth &&
                x >= t.x - t.side/2 &&
                x <= t.x + t.side/2 &&
                y >= t.y - t.side/2 &&
                y <= t.y + t.side/2)
            return true;
    return false;
    }
};

list<Treco> t_list;
list<Bomb> b_list;

void init() {
    Treco* p;
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 4; j++) {
            p = new Treco(RANDOM(T_SIDE/2, WIDTH-(T_SIDE/2)), RANDOM(T_SIDE/2, HEIGHT-(T_SIDE/2)), i);
            t_list.push_front(*p);
        }
    }
}

void reset() {
    t_list.clear();
    b_list.clear();
    init();
}


void display () {

    glClearColor(0.0, 0.0, 0.2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WIDTH, 0, HEIGHT);
    glMatrixMode(GL_MODELVIEW);

    list<Treco>::iterator t;
    list<Bomb>::iterator b;
    for (int d = 4; d >= 0; d--) {
        for (t=t_list.begin(); t != t_list.end(); t++)
            if((*t).get_depth() == d)
                t->display();
        for (b=b_list.begin(); b != b_list.end(); b++)
            if((*b).get_depth() == d)
                b->display();
    }

   glutSwapBuffers();
}

void time_out(int t) { // called if timer event
    list<Treco>::iterator p;
    list<Bomb>::iterator b;
    for (p=t_list.begin(); p != t_list.end(); p++) {
        p->move();
        for (b=b_list.begin(); b != b_list.end(); b++) {
            if(b->touch(*p)) {
                b = b_list.erase(b);
                p->blow();
            }
        }
    }
    for (b=b_list.begin(); b != b_list.end(); b++)
        if(b->fall())
            b = b_list.erase(b);

    // ...advance the state of animation incrementally...
    glutPostRedisplay(); // request redisplay
    if (!paused)
        glutTimerFunc(t, time_out, t); // request next timer event
}

void mouse (int btn, int st, int x, int y) {
    Bomb* b;
    if (btn==GLUT_LEFT_BUTTON) {
        if (st==GLUT_DOWN) {
            printf("left button pressed\n");
            printf("Coords: x= %i y= %i\n", x, y);
            b = new Bomb(x, HEIGHT-1 - y);
            b_list.push_front(*b);
        }
    }
    glutPostRedisplay();
}

void keyboard (unsigned char key, int x, int y) {
    switch (key) {
        case 'q':
            exit(0);
            break;

        case 'Q':
            exit(0);
            break;

        case 'R':
            exit(0);
            break;

        case 'r':
            reset();
            break;

        case 'p':
            if (paused) {
                paused = false;
                glutTimerFunc(IDLE_INIT_TIME, time_out, IDLE_INIT_TIME);
            }
            else
                paused = true;
            break;

        case 'P':
            if (paused) {
                paused = false;
                glutTimerFunc(IDLE_INIT_TIME, time_out, IDLE_INIT_TIME);
            }
            else
                paused = true;
            break;
        case ' ':
            if (paused)
                glutTimerFunc(0, time_out, 0);
            break;

        default:
            break;
    }
}
//void reshape(int w, int h) {
//    glViewport(0, 0, w, h);
//}

int main (int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow(argv[0]);

    glutDisplayFunc(display);
    glutTimerFunc(IDLE_INIT_TIME, time_out, IDLE_INIT_TIME);
//    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);

    //  glutCreateMenu(mymenu);
    //  glutAddMenuEntry("Reset", 0);
    //  glutAddMenuEntry("Quit", 1);
    //  glutAttachMenu(GLUT_MIDDLE_BUTTON);

    /* initializacoes */
    init();

    glutMainLoop();

    //finish();
    return 0;
}
