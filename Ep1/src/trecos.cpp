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

#define LEMBA 480
#define T_SIDE 0.085
#define B_SIDE 0.015
#define VMIN 0.005
#define VMAX 0.012
#define FALL_TIME 12
#define BLOW_TIME 16
#define IDLE_INIT_TIME 33

// Devolve um numero aleatorio entre 'low' e 'high'
#define RANDOM(low, high) ((float) (low + ((float) rand () / ((float) RAND_MAX + 1) * ((high) - (low)))))

int width  = LEMBA;
int height = LEMBA;

float ftL = 0;
float ftB = 0;
float ftR= 1;
float ftT= 1;

float steptw, stepth;

bool paused;
bool over;

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

class Square;
class Treco;
class Bomb;

list<Treco> t_list;
list<Bomb> b_list;

class Square {
protected:
    int it, depth;
    float side, x, y;
public:
    Square(float x, float y, int d) {
    	it = 0;
        this->x = x;
        this->y = y;
        depth = d;
        printf("square (%.2f, %.2f)\n", x, y);
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
    float vx, vy;
public:
    Treco(float x, float y, int d): Square(x, y, d) {
        side = T_SIDE;
        vx = RANDOM(VMIN, VMAX) * ((RANDOM(0, 1)>0.5)?1:-1);
        vy = RANDOM(VMIN, VMAX) * ((RANDOM(0, 1)>0.5)?1:-1);
//        printf("vx %2.2f, vy %2.2f, t %f\n", vx, vy, RANDOM(VMIN, VMAX));
    }
    void move() {
        if      (x + vx > ftR - (T_SIDE/2)) vx *= -1;
        else if (x + vx < ftL + (T_SIDE/2)) vx *= -1;
        else x += vx;

        if      (y + vy > ftT - (T_SIDE/2)) vy *= -1;
        else if (y + vy < ftB + (T_SIDE/2)) vy *= -1;
        else y += vy;
//        printf("%2d, %2d | %2d, %2d\n", x, y, vx, vy);
    }
    bool blow() {
    	if(depth >=5) {
    		it++;
    		if(it == BLOW_TIME)
    			return true;
    	}
    	return false;

    }
    void hit() {
        depth += 5;
    }
    friend class Bomb;
};

class Bomb: public Square {
public:
    Bomb(float x, float y): Square(x, ftT-y, 0) {
        side = B_SIDE;
    }
    bool fall() {
        it = (it + 1)%FALL_TIME;
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

void init() {
    Treco* p;

    srand(time(NULL));

    over = false;
    paused = false;

    for(int i = 0; i < 5; i++) {
        for(int j = 0; j < 4; j++) {
            p = new Treco(RANDOM(ftL+T_SIDE/2, ftR-(T_SIDE/2)), RANDOM(ftB+T_SIDE/2, ftT-(T_SIDE/2)), i);
            t_list.push_front(*p);
            if(i > 2) break; // TIRAR!
        }
    }

	steptw = (ftR-ftL)/width;
    stepth = (ftT-ftB)/height;
    printf("steptw: %f, stepth %f\n", steptw, stepth);
}

void reset() {
    t_list.clear();
    b_list.clear();
    init();
}


void display () {

    glClearColor(0.05, 0.05, 0.2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(ftL, ftR, ftB, ftT);
    glMatrixMode(GL_MODELVIEW);

    list<Treco>::iterator t;
    list<Bomb>::iterator b;
    for(int d = 4; d >= 0; d--) {
        for(t=t_list.begin(); t != t_list.end(); t++)
            if((*t).get_depth() == d)
                t->display();
        for(b=b_list.begin(); b != b_list.end(); b++)
            if((*b).get_depth() == d)
                b->display();
    }

   glutSwapBuffers();
}

void reshape(int w, int h) {

   width = w;
   height = h;
   stepth = (ftT-ftB)/h;
   steptw = (ftR-ftL)/w;

   glViewport(0,0,width,height);
}

bool game_over() {
	list<Treco>::iterator p;
	over = true;
	for (p=t_list.begin(); p != t_list.end(); p++) {
		int d = p->get_depth();
		if (d == 3 || d == 4)
			over = false;
	}
	return over;
}

void time_out(int t) { // called if timer event
    list<Treco>::iterator p;
    list<Bomb>::iterator b;

    if (!game_over()) {
    	for (p=t_list.begin(); p != t_list.end(); p++) {
    		if (p->blow()) {
    			p = t_list.erase(p);
    			p--;
    		}
    		else {
    			p->move();
    			for (b=b_list.begin(); b != b_list.end(); b++) {
    				if(b->touch(*p)) {
    					b = b_list.erase(b);
    					b--;
    					p->hit();
    				}
    			}
    		}
    	}
    	for(b=b_list.begin(); b != b_list.end(); b++)
    		if(b->fall())
    			b = b_list.erase(b);
    }

    // ...advance the state of animation incrementally...
    glutPostRedisplay(); // request redisplay
    if (!paused)
        glutTimerFunc(t, time_out, t); // request next timer event
}

void mouse(int btn, int st, int x, int y) {
    Bomb* b;
    float fcx, fcy;
    if(!over && !paused) {
    	if(btn==GLUT_LEFT_BUTTON) {
    		if(st==GLUT_DOWN) {
    			fcx = ftL + x * steptw;
    			fcy = ftB + y * stepth;
    			printf("left button pressed\n");
    			printf("Coords:   x= %4i   y= %4i\n", x, y);
    			printf("      : fcx= %.3f fcy= %.3f\n", fcx, fcy);
    			b = new Bomb(fcx, fcy);
    			b_list.push_front(*b);
    		}
    	}
    	else if(btn==GLUT_RIGHT_BUTTON) {
    		if(st==GLUT_DOWN) {
    			if(paused)
    				glutTimerFunc(0, time_out, 0);
    			else
    				paused = true;
    		}
    	}
    }
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
    switch(key) {
        case 'q':
            exit(0);
            break;

        case 'Q':
            exit(0);
            break;

        case 'R':
            reset();
            break;

        case 'r':
            reset();
            break;

        case 'p':
        	if(!over) {
        		if(paused)
        			glutTimerFunc(0, time_out, IDLE_INIT_TIME);
        		paused = !paused;
        	}
            break;

        case 'P':
        	if(!over) {
        		if(paused)
        			glutTimerFunc(0, time_out, IDLE_INIT_TIME);
        		paused = !paused;
        	}
            break;
        case ' ':
            if(paused)
                glutTimerFunc(0, time_out, 0);
            break;

        default:
            break;
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(width, height);
    glutCreateWindow(argv[0]);

    glutDisplayFunc(display);
    glutTimerFunc(IDLE_INIT_TIME, time_out, IDLE_INIT_TIME);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);

    /* initializacoes */
    init();

    glutMainLoop();

    //finish();
    return 0;
}
