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
#include <pthread.h>
GLenum rgb;					// for tkmap.c
#include "tkmap.c"

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
void initMenu();
void menu(int menuVal);
void printInstructions();

class Scene{

public:
	 
	 Scene () {wireframe = false; smoothShading = true;};  // constructor

	 // void zRotation(int direction);

	 vector< vector<float> > vertices;
	 bool textures;
	 bool wireframe;
	 bool smoothShading;
	 int zoom;
};

Scene scene;

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



	alCapone = glmReadOBJ("al.obj");

	glmUnitize(alCapone);
	glmFacetNormals(alCapone);
	glmVertexNormals(alCapone, 90.0);

	// looks better but doesn't rotate together
	// back wall
	glPushMatrix();
		glColor3f(1.0, 0.0, 0.0);
		glTranslatef(0.0, 0.0, 2.0);
		glRectf(-1.0, 1.0, 1.0, -0.45);
		glTranslatef(0.0, 0.0, -2.0);
	glPopMatrix();

	// floor needs work
	glPushMatrix();
		glColor3f(0.5, 0.35, 0.05);
		glTranslatef(0.0, 0.0, 2.0);
		glRotatef(-45, 1, 0, 0);
		glRectf(-1.0, -0.45, 1.0, -1.0);
		glTranslatef(0.0, 0.0, -2.0);
		glRotatef(45, 1, 0, 0);
	glPopMatrix();

	// Al Capone
	glPushMatrix();
		glColor3f(1.0, 1.0, 1.0);
		glTranslatef(1.0, -0.5, 0.0);
		glScalef(0.5, 0.5, 0.5);
		glRotatef(-90, 0, 1, 0);
		if (scene.smoothShading)
			glmDraw(alCapone, GLM_SMOOTH | GLM_MATERIAL);
		else
			glmDraw(alCapone, GLM_FLAT | GLM_MATERIAL);
		glTranslatef(-1.0, 0.5, 0.0);
		glScalef(-0.5, -0.5, -0.5);
		glRotatef(90, 0, 1, 0);
	glPopMatrix();


	// rotate both together
	//
	// glPushMatrix();
	// 	glColor3f(1.0, 0.0, 0.0);
	// 	glRectf(-1.0, 1.0, 1.0, -0.45);

	// 	glPushMatrix();
	// 		glColor3f(1.0, 1.0, 1.0);
	// 		glTranslatef(1.0, -0.5, 0.0);
	// 		glScalef(0.5, 0.5, 0.5);
	// 		glRotatef(-90, 0, 1, 0);
	// 		if (scene.smoothShading)
	// 			glmDraw(alCapone, GLM_SMOOTH | GLM_MATERIAL);
	// 		else
	// 			glmDraw(alCapone, GLM_FLAT | GLM_MATERIAL);
	// 	glPopMatrix();
	// glPopMatrix();
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

void initMenu()
// INPUT: none	OUTPUT: none
// initiate the menu on mouse click
{
	// create menu and add options to it
	glutCreateMenu(menu);						// call menu function
	glutAddMenuEntry("Toggle Textures On/Off", 0);
	glutAddMenuEntry("Toggle Wireframe On/Off", 1);
	glutAddMenuEntry("Toggle Smooth/Flat Shading", 2);
	glutAddMenuEntry("Quit", 3);
	glutAttachMenu(GLUT_LEFT_BUTTON);			// attach menu to left click action
}

void menu (int menuVal)
// INPUT: value of menu option clicked	OUTPUT: none
// what each item in the menu does based on assigned value
{
	// toggle objects based on menu value selected
	switch (menuVal) {
		case 0: if (scene.textures) // cout << "toggle smooth/flat shading function call\n";
				{
					scene.textures = false;
					glutPostRedisplay();
				}
				else
				{
					scene.textures = true;
					glutPostRedisplay();
				}
				break;
		case 1: if (scene.wireframe)
				{
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					glutPostRedisplay();
					scene.wireframe = false;
				}
				else
				{
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					glutPostRedisplay();
					scene.wireframe = true;
				}
				break;
		case 2: if (scene.smoothShading) // cout << "toggle smooth/flat shading function call\n";
				{
					scene.smoothShading = false;
					glutPostRedisplay();
				}
				else
				{
					scene.smoothShading = true;
					glutPostRedisplay();
				}
				break;
		case 3: exit(1);
	}
}

void keyboard (unsigned char key, int x, int y)
{
	switch (key) {
		case '8': yCam += 0.5;
					 glutPostRedisplay();
					 // cout << "rotate up\n";
					 break;
		case '2': yCam -= 0.5;
					 glutPostRedisplay();
					 // cout << "rotate down\n";
					 break;
		case '4': xCam -= 0.5;
					 glutPostRedisplay();
					 // cout << "rotate left\n";
					 break;
		case '6': xCam += 0.5;
					 glutPostRedisplay();
					 // cout << "rotate right\n";
					 break;
		case '0': if (scene.zoom != maxZoom)
				  {
					z -= 0.5;
					scene.zoom += 1;
					// cout << "Zoom in\n";
				  }
				  glutPostRedisplay();
				  break;
		case '1': if (scene.zoom != minZoom)
				  {
					z += 0.5;
					scene.zoom -= 1;
					// cout << "Zoom out\n";
				  }
				  glutPostRedisplay();
				  break;
		case 'r': xCam = 0.0;
				  yCam = 0.0;
				  z = 4.75;
				  scene.zoom = 0;
				  glutPostRedisplay();
				  // cout << "reset\n";
				  break;
		case 'q': exit (1);
	}
	// scene.myDraw();
}

void printInstructions()
{
	cout << "\nLeft click for menu\n";
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
	initMenu();
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
	glutKeyboardFunc (keyboard);
	glutMainLoop();

	return 0;
}