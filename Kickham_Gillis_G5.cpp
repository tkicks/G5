/*

Names: Austin Gillis Tyler Kickham
Program Name: Assignment G5- What You See
Purpose: 
Input:	 

Output: 



TO TURN OFF LIGHTING: comment out in drawScene (two places), setup

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

// lighting ========================================
static float amb[] =  {1.0, 1.0, 1.0, 0.0};
static float dif[] =  {1.0, 1.0, 1.0, 0.0};
static float spec[] = { 1.0 , 1.0 , 1.0 , 1.0 };
float light_diffuse[] = {100.0, 0.0, 100.0, 0.0}; 
float light_position[] = {-100.0, 100.0, -2.0, 0.0};
// =================================================

/*************************************************************/


void* simpleFunc(void*) { return NULL; }
void forcePThreadLink() { pthread_t t1; pthread_create(&t1, NULL, &simpleFunc, NULL); }
void initMenu();
void menu(int menuVal);
void printInstructions();

class Scene{

public:
	 
	Scene () {wireframe = false; smoothShading = true; bRadius=0.15; sRadius=bRadius/1.478261; height=bRadius*2.235294;};  // constructor

	 // void zRotation(int direction);
	void drawCup();
	void enableLighting();
	void disableLighting();


	vector< vector<float> > vertices;
	
	bool textures;
	bool wireframe;
	bool smoothShading;
	
	int zoom;
    
	float bRadius;
	float sRadius;
	float height;

};

Scene scene;
/************************Class Methods***************************/

void Scene::drawCup(){

	glBegin (GL_QUADS);

		glColor3f(1.0,0,0);
		
		for (float i = 0; i < 2*PI; i+=(PI/32)){
			glVertex3f(sRadius*cos(i),-height,sRadius*sin(i));
			glVertex3f(sRadius*cos(i+(PI/32)),-height,sRadius*sin(i+(PI/32)));
			glVertex3f(bRadius*cos(i),0,bRadius*sin(i));
			glVertex3f(bRadius*cos(i+(PI/32)),0,bRadius*sin(i+(PI/32)));
		}     

	glEnd();

}

void Scene::enableLighting()
{
	// lighting ==========================================
 	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);;
	glEnable(GL_NORMALIZE);
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	glMaterialfv( GL_FRONT , GL_SPECULAR , spec);
	// ===================================================
}

void Scene::disableLighting()
{
	// lighting
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);
	glDisable (GL_COLOR_MATERIAL);
}

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

		// scene.enableLighting();

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
			glTranslatef(0.0, -0.2, -2.1);
			// glTranslatef(1.4, -0.2, 0.0);	// sideways
			glScalef(0.9, 0.9, 0.9);
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

		// scene.disableLighting();

	glPopMatrix();
	
	glPushMatrix();
		
		scene.enableLighting();


		glTranslatef(0.0, -1.0, 1.25);
		glScalef(0.666,0.666,0.666);
		glPushMatrix();
			glColor3f(0.545, 0.271, 0.075);
			glRotatef(5,1,0,0);
			glScalef(1.333,0.1,3.55);
			glutSolidCube(1);
		glPopMatrix();

		// Legs
		

		glPushMatrix();

			glTranslatef(0.35,0,1.25);
			scene.drawCup();
			
		glPopMatrix();

		glPushMatrix();
			
			glTranslatef(-0.35,0,1.25);
			scene.drawCup();

		glPushMatrix();

			glTranslatef(.35,0,-.75);
			scene.drawCup();

		glPopMatrix();

		scene.disableLighting();


	glPopMatrix();

	

	glutSwapBuffers();
}

// Initialization routine.
void setup(void) 
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel (GLM_SMOOTH);
	// lighting
	// glEnable(GL_LIGHTING)
	// end lighting
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
	glutInitWindowSize(600, 600);
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