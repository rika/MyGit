/* Ep1 - MAC0420 Computacao grafica
 *
 * Nome: Ricardo Juliano Mesquita Silva Oda
 * NUSP: 6514223
 *
 * Objetivo:
 * Familiar o aluno com as funcionalidades basicas do OpenGL.
 *
 * Programa:
 * O jogo se passa em uma piscina povoada com objetos chamados trecos.
 * O objetivo do jogo é "matar" todos os trecos ruins, sem "matar" os trecos bons.
 * Os trecos ruins ficam no fundo da piscina (tem a cor mais escura) e os bons
 * ficam na superfície. Bombas sao utilizadas para cumprir tal objetivo.
 *
 * Operação:
 *
 * MOUSE
 * botão esquerdo: solta uma bomba
 * botão direito: pause o jogo / roda um passo de debug
 *
 * TECLADO
 * q: termina o programa
 * r: reseta o jogo
 * p: pausa / despausa
 * espaço: roda um passo de debug
 *
 * nota: a impressao de debug so ocorre com o jogo pausado
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
#include <string>
#include <sstream>
#include <list>

using namespace std;

// Defines
#define LEMBA 480
#define T_SIDE 0.085        // Lado de um treco em LEMBAs
#define B_SIDE 0.015        // Lado de uma bomba em LEMBAs
#define VMIN 0.005          // Velocidade minima de um treco em LEMBA/frames
#define VMAX 0.012          // Velocidade maxima de um treco em LEMBA/frames
#define FALL_TIME 11        // Tempo de queda de uma bomba em frames
#define BLOW_TIME 30        // Tempo de explosao de um treco em frames
#define IDLE_INIT_TIME 33   // Tempo de sleep entre frames em milisegundos

// Devolve um numero aleatorio entre 'low' e 'high'
#define RANDOM(low, high) ((float) (low + ((float) rand () / ((float) RAND_MAX + 1) * ((high) - (low)))))


// Variaveis globais
int width  = LEMBA;
int height = LEMBA;

float ftL = 0;
float ftB = 0;
float ftR= 1;
float ftT= 1;

float steptw, stepth;

int score;

bool paused, over;

float ColorDepthTablef[10][3]={
    // cores dos trecos
    {0.00, 1.00, 1.00},
    {0.00, 0.85, 0.85},
    {0.00, 0.70, 0.70},
    {0.00, 0.25, 0.25},
    {0.00, 0.15, 0.15},

    // cores dos trecos explodindo
    {1.00, 0.00, 0.00},
    {0.90, 0.00, 0.00},
    {0.80, 0.00, 0.00},
    {0.70, 0.00, 0.00},
    {0.60, 0.00, 0.00}
};

// tabela de pontuacao
int ScoreTable[5]={
    -4000,
    -2000,
    -1000,
    +1000,
    +2000
};


/* **************************************************************************
    CLASSES
 ************************************************************************** */


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
    }
    int get_depth() {
        return depth%5; // a profundidade eh dada por depth%5
                        // depth >= 5 indica que o treco esta explodindo
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
    bool dead;
public:
    Treco(float x, float y, int d): Square(x, y, d) {
        dead = false;
        side = T_SIDE;
        vx = RANDOM(VMIN, VMAX) * ((RANDOM(0, 1)>0.5)?1:-1);
        vy = RANDOM(VMIN, VMAX) * ((RANDOM(0, 1)>0.5)?1:-1);
    }
    void move() {
        if      (x + vx > ftR - (T_SIDE/2)) vx *= -1;
        else if (x + vx < ftL + (T_SIDE/2)) vx *= -1;
        else x += vx;

        if      (y + vy > ftT - (T_SIDE/2)) vy *= -1;
        else if (y + vy < ftB + (T_SIDE/2)) vy *= -1;
        else y += vy;
    }
    bool is_dead() {
        return dead;
    }
    bool blow() {
        if(depth >=5) {
            it++;
            if(it == BLOW_TIME)
                return dead = true;
        }
        return false;
    }
    void hit() {
        depth += 5;
    }
    void debug(int id) {
        printf("        Treco: %d      ", id);
        if (dead)
            printf("MORTO\n");
        else
            printf("Pos: (%.3f, %.3f) Vel: (%.5f, %.5f)\n", x, y, vx, vy);
    }
    friend class Bomb;
};


class Bomb: public Square {
public:
    Bomb(float x, float y): Square(x, ftT-y, 0) {
        side = B_SIDE;
        if (paused)
            printf("Bomba criada em: (%.3f, %.3f)\n", x, y);
    }
    bool fall() {
        it = (it + 1)%FALL_TIME;
        if (it == 0) {
            depth++;
            if (depth >= 5)
                return true;
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
    void debug() {
        printf("        BOMBA          Pos: (%.3f, %.3f)\n", x, y);
    }

};


// listas globais dos trecos e bombas
list<Treco> t_list[5];
list<Bomb> b_list;


/* **************************************************************************
    FUNCOES
 ************************************************************************** */


void init();
void reset();
void debug();
void display_str(float, float, string);
void display_score();
void display();
bool game_over();
void step(int);
void mouse(int, int, int, int);
void keyboard(unsigned char, int, int);


/* **************************************************************************
    INICIALIZACAO E RESET
 ************************************************************************** */


void init() {
    Treco* p;

    srand(time(NULL));
    score = 0;
    over = false;
    paused = false;

    for(int i = 0; i < 5; i++) {
        for(int j = 0; j < 4; j++) {
            p = new Treco(RANDOM(ftL+T_SIDE/2, ftR-(T_SIDE/2)), RANDOM(ftB+T_SIDE/2, ftT-(T_SIDE/2)), i);
            t_list[i].push_front(*p);
        }
    }
    steptw = (ftR-ftL)/width;
    stepth = (ftT-ftB)/height;
}


void reset() {
    for (int i = 0; i < 5; i++)
        t_list[i].clear();
    b_list.clear();
    if(paused) {
        paused = false;
        glutTimerFunc(0, step, IDLE_INIT_TIME);
    }
    init();
}


/* **************************************************************************
   DEBUG
 ************************************************************************** */

void debug() {
	for(int d = 4; d >= 0; d--) {
		printf("\nNivel: %d\n", d);
		int i = 0;
		for(list<Treco>::iterator t = t_list[d].begin(); t != t_list[d].end(); t++) {
			t->debug(i++);
		}
		for(list<Bomb>::iterator b = b_list.begin(); b != b_list.end(); b++)
			if((*b).get_depth() == d)
				b->debug();
	}
}


/* **************************************************************************
   FUNCOES DE DISPLAY E RESHAPE
 ************************************************************************** */


void display_str(float x, float y, string s) {
    void * font = GLUT_BITMAP_9_BY_15;
    glPushMatrix();
    glLoadIdentity();
    glColor3f(1.0, 1.0, 1.0);
    glRasterPos2f(x, y);
    for (string::iterator i = s.begin(); i != s.end(); i++) {
        char c = *i;
        glutBitmapCharacter(font, c);
    }
    glPopMatrix();
}


void display_score() {
    stringstream ss;
    ss << "Pontos: " << score;
    display_str(0.1, 0.1, ss.str());
}


void display() {
    glClearColor(0.05, 0.05, 0.2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(ftL, ftR, ftB, ftT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    for(int d = 4; d >= 0; d--) {
        for(list<Treco>::iterator t = t_list[d].begin(); t != t_list[d].end(); t++) {
            if (!t->is_dead())
                t->display();
        }
        for(list<Bomb>::iterator b = b_list.begin(); b != b_list.end(); b++)
            if((*b).get_depth() == d)
                b->display();
    }
    display_score();
    if (over) {
        display_str(0.3, 0.5, "Fim de Jogo");
        display_str(0.3, 0.4, "Aperte 'r' para recomeçar.");
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


/* **************************************************************************
   CHECAGEM DE FIM DE JOGO
 ************************************************************************** */


bool game_over() {
    over = true;
    for(int i = 3; i < 5; i++)
        for (list<Treco>::iterator p = t_list[i].begin(); p != t_list[i].end(); p++)
            return over = false;
    paused = false;
    return over;
}


/* **************************************************************************
   FUNCAO DE ANIMACAO
 ************************************************************************** */


void step(int t) {
    list<Treco>::iterator p;
    list<Bomb>::iterator b;

    if (paused)
    	debug();
    if (!game_over()) {
        for (int d = 0; d < 5; d++) {
            for (p = t_list[d].begin(); p != t_list[d].end(); p++) {
                if (!p->is_dead()) {
                    if (p->blow())
                        score += ScoreTable[p->get_depth()];
                    else {
                        p->move();
                        for (b=b_list.begin(); b != b_list.end(); b++) {
                            if(b->touch(*p)) {
                            	// a bomba some quando acerta um alvo
                                b = b_list.erase(b);
                                b--;
                                p->hit();
                            }
                        }
                    }
                }
            }
        }
        for(b = b_list.begin(); b != b_list.end(); b++)
            if(b->fall()) {
                b = b_list.erase(b);
                b--;
            }
    }
    glutPostRedisplay();
    if (!paused)
        glutTimerFunc(t, step, t);
}


/* **************************************************************************
   FUNCOES DE INTERFACE COM O USUARIO
 ************************************************************************** */


void mouse(int btn, int st, int x, int y) {
    Bomb* b;
    float fcx, fcy;
    if(!over && btn==GLUT_LEFT_BUTTON) {
        if(st==GLUT_DOWN) {
            fcx = ftL + x * steptw;
            fcy = ftB + y * stepth;
            b = new Bomb(fcx, fcy);
            b_list.push_front(*b);
        }
    }
    else if(btn==GLUT_RIGHT_BUTTON) {
        if(st==GLUT_DOWN) {
            if(paused)
                glutTimerFunc(0, step, 0);
            else
                paused = true;
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
                if(paused) {
                    paused = false;
                    glutTimerFunc(0, step, IDLE_INIT_TIME);
                }
                else
                    paused = true;
            }
            break;

        case 'P':
            if(!over) {
                if(paused) {
                    paused = false;
                    glutTimerFunc(0, step, IDLE_INIT_TIME);
                }
                else
                    paused = true;
            }
            break;

        case ' ':
            if(paused)
                glutTimerFunc(0, step, 0);
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
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(width, height);
    glutCreateWindow(argv[0]);
    glutDisplayFunc(display);
    glutTimerFunc(IDLE_INIT_TIME, step, IDLE_INIT_TIME);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);

    init();

    glutMainLoop();

    return 0;
}
