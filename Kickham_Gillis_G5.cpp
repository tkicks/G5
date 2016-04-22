/*

Names: Austin Gillis Tyler Kickham
Program Name: Assignment G5- What You See
Purpose: 
Input:	 

Output: 

*/

#include <GL/glut.h>
#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <math.h>
#include "glm.h"

using namespace std;

#define PI 3.14159265

/************************Globals******************************/

static GLsizei width, height; // OpenGL window size.

float xCam = 0.0;
float yCam = 0.0;
float z = 4.75;

GLMmodel* alCapone = NULL;

static GLUquadricObj *qobj;

const int maxZoom = 6;
const int minZoom = -14;

/*************************************************************/


void* simpleFunc(void*) { return NULL; }
void forcePThreadLink() { pthread_t t1; pthread_create(&t1, NULL, &simpleFunc, NULL); }

class Scene{

public:
	 
	 Scene () {};  // constructor

	 // void zRotation(int direction);
	 
	 void drawVertical();
	 void drawAngled(int direction);

	 vector< vector<float> > vertices;
	 vector<char> plant;
	 vector<float>currP;
	 float objHeight;    // height of cylinders (branches)
	 float objRadius;    // radius of cylinders (branches)
	 float angle;        // angle to rotate branches by
	 int n;					// number of iterations
	 int grammarNum;		// which grammar
	 int zoom;
	 bool leafing;			// if drawing a leaf
	 bool branching;		// if drawing a branch

	 float startY;   //Coords for the top of the first cylinder drawn (0,startY,0) so only need var for startY 

	 stack<vector<float> > knots; //Coords for the places to pop back to
};

Scene fractal;

/************************Class Methods***************************/

// void Scene::zRotation(int direction){

// 	float xCoord,yCoord,xP,yP,fX,fY;

// 	xCoord=currP[0];
// 	yCoord=startY+currP[1]+objHeight;

// 	fX= currP[0];
// 	fY= currP[1]+startY;

// 	xP= xCoord*cos(direction*angle) - yCoord*sin(direction*angle) +(-1*fX*cos(direction*angle)+fY*sin(direction*angle));
// 	yP= xCoord*sin(direction*angle) + yCoord*cos(direction*angle) +(-1*fX*sin(direction*angle)-fY*cos(direction*angle));


// 	currP[0]+=xP;
// 	currP[1]+=yP;

// }




/*****************************************************************/

// Drawing routine.
void drawScene(void)
{

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity ();
	glFrustum (-1, 1, -1, 1, 1.5, 20.0);
	gluLookAt (xCam, yCam, z, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();


	//--------------------------END Fractal VIEWPORT---------------------------

	alCapone = glmReadOBJ("al.obj");

	glmUnitize(alCapone);
	glmFacetNormals(alCapone);
	glmVertexNormals(alCapone, 90.0);

	glPushMatrix();
		glTranslatef(1.0, -0.5, 0.0);
		glScalef(0.5, 0.5, 0.5);
		glRotatef(-90, 0, 1, 0);
		glmDraw(alCapone, GLM_SMOOTH | GLM_MATERIAL);
	glPopMatrix();
	glutSwapBuffers();
}

// Initialization routine.
void setup(void) 
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel (GL_FLAT);
	glEnable (GL_DEPTH_TEST);
	qobj = gluNewQuadric();
}

// OpenGL window reshape routine.
void resize(int w, int h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum (-1, 1, -1, 1, 1.5, 20.0);
	glMatrixMode(GL_MODELVIEW);

	// Pass the size of the OpenGL window to globals.
	width = w;
	height = h;
}

// Mouse input processing routine.
void mouse (int button, int state, int x, int y)
{
	// if (state == GLUT_DOWN) {
	// 	cout << "mouse click do menu\n";
	// }
}

void keyboard (unsigned char key, int x, int y)
{
	switch (key) {
		case '8': yCam += 0.5;
					 glutPostRedisplay();
					 cout << "rotate up\n";
					 break;
		case '2': yCam -= 0.5;
					 glutPostRedisplay();
					 cout << "rotate down\n";
					 break;
		case '4': xCam -= 0.5;
					 glutPostRedisplay();
					 cout << "rotate left\n";
					 break;
		case '6': xCam += 0.5;
					 glutPostRedisplay();
					 cout << "rotate right\n";
					 break;
		case '0': if (fractal.zoom != maxZoom)
				  {
					z -= 0.5;
					fractal.zoom += 1;
					cout << "Zoom in\n";
				  }
				  glutPostRedisplay();
				  break;
		case '1': if (fractal.zoom != minZoom)
				  {
					z += 0.5;
					fractal.zoom -= 1;
					cout << "Zoom out\n";
				  }
				  glutPostRedisplay();
				  break;
		case 'r': xCam = 0.0;
				  yCam = 0.0;
				  z = 4.75;
				  fractal.zoom = 0;
				  glutPostRedisplay();
				  cout << "reset\n";
				  break;
		case 'q': exit (1);
	}
	// fractal.myDraw();
}

void printInstructions()
{
	cout << "Keypresses:\n";
	cout << "0: zoom in\n";
	cout << "1: zoom out\n";
	cout << "2: rotate down\n";
	cout << "4: rotate left\n";
	cout << "6: rotate right\n";
	cout << "8: rotate up\n";
	cout << "r: reset scene\n";
	cout << "q: quit\n";
}

// Main routine.
int main(int argc, char **argv) 
{
	printInstructions();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(1125, 1125);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Al Capone Pong");
	setup();
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
	glutMouseFunc(mouse);
	glutKeyboardFunc (keyboard);
	glutMainLoop();

	return 0;
}