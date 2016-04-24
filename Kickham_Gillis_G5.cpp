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
unsigned char* image = NULL;

static GLUquadricObj *qobj;

const int maxZoom = 6;
const int minZoom = -14;
int wallFloorHeight = 3.0;
int wallFloorWidth = 3.0;

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



// Drawing routine.
void drawScene(void)
{

	static float amb[] =  {0.4, 0.4, 0.4, 0.0};
    static float dif[] =  {1.0, 1.0, 1.0, 0.0};
    
    float light_diffuse[] = {100.0, 0.0, 100.0, 100.0}; 
    float light_position[] = {-100.0, 100.0, 1.0, 0.0};

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity ();
	glFrustum (-1, 1, -1, 1, 1.5, 20.0);
	gluLookAt (xCam, yCam, z, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();

	// lighting???
	// GLfloat ambientColor[] = {0.2f, 0.2f, 0.2f, 1.0f}; //Color(0.2, 0.2, 0.2)
 	// glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

 	// GLfloat lightColor0[] = {0.5f, 0.5f, 0.5f, 1.0f}; //Color (0.5, 0.5, 0.5)
 	// GLfloat lightPos0[] = {4.0f, 0.0f, 8.0f, 1.0f}; //Positioned at (4, 0, 8)
 	// glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
 	// glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);

 	// GLfloat lightColor1[] = {0.5f, 0.2f, 0.2f, 1.0f}; //Color (0.5, 0.2, 0.2)
 	// //Coming from the direction (-1, 0.5, 0.5)
 	// GLfloat lightPos1[] = {-1.0f, 0.5f, 0.5f, 0.0f};
 	// glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor1);
 	// glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);

 	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHTING);
    glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);


	// set up for al capone object
	alCapone = glmReadOBJ("al.obj");
	glmUnitize(alCapone);
	glmFacetNormals(alCapone);
	glmVertexNormals(alCapone, 90.0);

	// set up for wall/floor texture
	free(image);	// clear out image?
	image = glmReadPPM("wallFloor.ppm", &wallFloorWidth, &wallFloorHeight);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, wallFloorWidth, wallFloorHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glTexParameteri(GL_TEXTURE_2D, GL_REPEAT, GL_NEAREST);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	


	glPushMatrix();
		glTranslatef(0.0, -0.8, 0.0);

		// floor and walls and ceiling
		glPushMatrix();
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);
			glTranslatef(0.0, 0.8, 0.0);
			glScalef(1.2, 1.2, 1.2);
			glColor3f(0.5, 0.35, 0.05);
			glEnable(GL_TEXTURE_2D);	// for textures
			glutSolidCube(3.0);
			glColor3f(1.0, 1.0, 1.0);
			glutWireCube(3.0);
			glDisable(GL_CULL_FACE);
		glPopMatrix();

		// Al Capone
		glPushMatrix();
			glColor3f(1.0, 1.0, 1.0);
			glTranslatef(0.0, -0.2, -1.1);
			// glTranslatef(1.4, -0.2, 0.0);	// sideways
			glScalef(0.8, 0.8, 0.8);
			// glRotatef(-90, 0, 1, 0);		// sideways
			if (scene.smoothShading)
				glmDraw(alCapone, GLM_SMOOTH | GLM_MATERIAL);
			else
				glmDraw(alCapone, GLM_FLAT | GLM_MATERIAL);
			glTranslatef(0.0, 0.2, 1.1);
			// glTranslatef(-1.4, 0.2, 0.0);	// sideways
			glScalef(-0.8, -0.8, -0.8);
			// glRotatef(90, 0, 1, 0);		// sideways
		glPopMatrix();
	glPopMatrix();


	glDisable(GL_LIGHT0);
    glDisable(GL_LIGHT1);
    glDisable(GL_LIGHTING);

	glutSwapBuffers();
}

// Initialization routine.
void setup(void) 
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel (GLM_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);
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