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
static float amb[] =  {0.3, 0.3, 0.3, 0.0};
static float dif[] =  {0.6, 0.6, 0.6, 0.6};
static float spec[] = { 1.0 , 1.0 , 1.0 , 1.0 };
float light_diffuse[] = {0.8, 0.8, 0.8, 200.0}; 
// float light_position[] = {0.0, 10.0, 0.0, 0.0};
float light_position2[] = {-10.0, 10.0, -2.0, 0.0};
float light_position3[] = {10.0, 10.0, -2.0, 0.0};
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
	void makeCup();
	void drawCup();
	void enableLighting();
	void disableLighting();

	vector<float> getNormal(vector<float> in1, vector<float> in2, vector<float> in3);
	vector<float> avgVec(vector<float> in1, vector<float> in2);

	void findVerts();
	void genSurfNorms();
	void genVertNorms();
	
	bool textures;
	bool wireframe;
	bool smoothShading;
	
	int zoom;
    
	float bRadius;
	float sRadius;
	float height;

	vector<vector<float> >outterVerts;
	vector<vector<float> >innerVerts;

	vector<vector<float> >surfaceNormals;
	vector<vector<float> >vertexNormals;

};

Scene scene;
/************************Class Methods***************************/
void Scene::makeCup(){

	glBegin (GL_QUADS);

		for (int i=0;i<innerVerts.size()-1;i++){
			glVertex3f(innerVerts[i+1][0],innerVerts[i+1][1],innerVerts[i+1][2]);
			
			glNormal3f(surfaceNormals[i][0],surfaceNormals[i][1],surfaceNormals[i][2]);
			
			glVertex3f(outterVerts[i+1][0],outterVerts[i+1][1],outterVerts[i+1][2]);
			
			glNormal3f(surfaceNormals[i][0],surfaceNormals[i][1],surfaceNormals[i][2]);
			
			glVertex3f(outterVerts[i][0],outterVerts[i][1],outterVerts[i][2]);

			glNormal3f(surfaceNormals[i][0],surfaceNormals[i][1],surfaceNormals[i][2]);
			
			glVertex3f(innerVerts[i][0],innerVerts[i][1],innerVerts[i][2]);
			
			glNormal3f(surfaceNormals[i][0],surfaceNormals[i][1],surfaceNormals[i][2]);
		
		}
	
	glEnd();

}

void Scene::drawCup(){

    glPushMatrix();

		glEnable(GL_DEPTH_TEST| GL_DEPTH_BUFFER_BIT);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glColor3f(1.0,1.0,1.0);
		makeCup();

		glDisable(GL_CULL_FACE);

    glPopMatrix();
    glPushMatrix();

		glEnable(GL_DEPTH_TEST| GL_DEPTH_BUFFER_BIT);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		glColor3f(1.0,0.0,0.0);
		makeCup();

		glDisable(GL_CULL_FACE);
	   
	glPopMatrix();

}

vector<float> Scene::getNormal(vector<float> in1, vector<float> in2, vector<float> in3){

	vector<float> result;
	vector<float> v1(3,0.0);
	vector<float> v2(3,0.0);

	for (int i = 0; i < 3; i++){
		v1[i]=in2[i]-in1[i];
		v2[i]=in3[i]-in1[i];
	}

	float newX, newY, newZ;
	
	newX=(v1[1]*v2[2])-(v1[2]*v2[1]);
	result.push_back(newX);

	newY=(v1[2]*v2[0])-(v1[0]*v2[2]);
	result.push_back(newY);

	newZ=(v1[0]*v2[1])-(v1[1]*v2[0]);
	result.push_back(newZ);	


	float mag= sqrt((result[0]*result[0])+(result[1]*result[1])+(result[2]*result[2]));

	for (int j = 0; j < 3; j++){
		result[j]=result[j]/mag;
	}


	return result;
}

vector<float> Scene::avgVec(vector<float> in1, vector<float> in2){

	vector<float> result(3,0.0);

	for (int i = 0; i < 3; i++){
		result[i]=(in1[i]+in2[i])/2;
	}

	return result;
}

void Scene::findVerts(){

	vector<float> tempA;
	vector<float> tempB;

	for (float i = 0; i <= 2*PI; i+=(PI/32)){
		tempA.push_back(sRadius*cos(i));
		tempA.push_back(-height);
		tempA.push_back(sRadius*sin(i));
		tempB.push_back(bRadius*cos(i));
		tempB.push_back(0);
		tempB.push_back(bRadius*sin(i));
		
		innerVerts.push_back(tempA);
		outterVerts.push_back(tempB);

		tempA.clear();
		tempB.clear();
	}		

}

void Scene::genSurfNorms(){

	vector<float> tempA;
	for (int i = 0; i<innerVerts.size()-1; i++){

		tempA=getNormal(innerVerts[i+1],innerVerts[i],outterVerts[i]);
		surfaceNormals.push_back(tempA);
		tempA.clear();

	} 

}

void Scene::genVertNorms(){

	

	vector<float> tempA;
	for (int i= 0; i < surfaceNormals.size(); i++){
		
		if(i==0){
			tempA=avgVec(surfaceNormals[i],surfaceNormals[(surfaceNormals.size()-1)]);
			
		}
		else{
			tempA=avgVec(surfaceNormals[i],surfaceNormals[i-1]);
			
		}

		vertexNormals.push_back(tempA);
		tempA.clear();
	}

}

void Scene::enableLighting()
{
	// lighting ==========================================
 	// glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	// glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position2);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT2, GL_POSITION, light_position3);
	// glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);
	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	glMaterialfv( GL_FRONT , GL_SPECULAR , spec);
	glMaterialfv(GL_BACK, GL_AMBIENT, amb);
	glMaterialfv(GL_BACK, GL_DIFFUSE, dif);
	glMaterialfv( GL_BACK , GL_SPECULAR , spec);
	glEnable(GL_COLOR_MATERIAL);

	
	// ===================================================
}

void Scene::disableLighting()
{
	// lighting
	// glDisable(GL_LIGHT0);
	glDisable(GL_LIGHT1);
	glDisable(GL_LIGHT2);
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
	gluLookAt (xCam, yCam, z, xCam, yCam, 0.0, 0.0, 1.0, 0.0);

	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();


	// glTranslatef(xCam,yCam,0);


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
		scene.enableLighting();

		glPushMatrix();

			// scene.enableLighting();

			glTranslatef(0.0, -0.8, 0.0);

			// floor and walls and ceiling
			glPushMatrix();
				glEnable(GL_DEPTH_TEST| GL_DEPTH_BUFFER_BIT);
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
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			glPushMatrix();
				glColor3f(1.0, 1.0, 1.0);
				glTranslatef(0.0, -0.2, -2.1);
				// glTranslatef(1.4, -0.2, 0.0);	// sideways
				glScalef(0.9, 0.9, 0.9);
				if (scene.smoothShading)
					glmDraw(alCapone, GLM_SMOOTH | GLM_MATERIAL);
				else
					glmDraw(alCapone, GLM_FLAT | GLM_MATERIAL);
				glTranslatef(0.0, 0.2, 1.1);
				// glTranslatef(-1.4, 0.2, 0.0);	// sideways
				glScalef(-0.8, -0.8, -0.8);
			glPopMatrix();
			glDisable(GL_CULL_FACE);
		glPopMatrix();
		
		glPushMatrix();
			// scene.enableLighting();

			glTranslatef(0.0, -1.0, 1.25);
			glPushMatrix();
				//Table
				glColor3f(0.545, 0.271, 0.075);
				glPushMatrix();
					glTranslatef(.35,0,-.88);
					glRotatef(90.0, 1.0, 0.0, 0.0);
					gluQuadricNormals(qobj, GLU_SMOOTH);
					gluCylinder(qobj, .1, .1, 0.7, 15.0, 5.0);
				glPopMatrix();
				glPushMatrix();
					glTranslatef(-.35,0,-.88);
					glRotatef(90.0, 1.0, 0.0, 0.0);
					gluQuadricNormals(qobj, GLU_SMOOTH);
					gluCylinder(qobj, .1, .1, 0.7, 15.0, 5.0);
				glPopMatrix();
				glPushMatrix();
					glTranslatef(-.35,0,0.2);
					glRotatef(90.0, 1.0, 0.0, 0.0);
					gluQuadricNormals(qobj, GLU_SMOOTH);
					gluCylinder(qobj, .1, .1, 0.7, 15.0, 5.0);
				glPopMatrix();
				glPushMatrix();
					glTranslatef(.35,0,0.2);
					glRotatef(90.0, 1.0, 0.0, 0.0);
					gluQuadricNormals(qobj, GLU_SMOOTH);
					gluCylinder(qobj, .1, .1, 0.7, 15.0, 5.0);
				glPopMatrix();
				glPushMatrix();
					glScalef(0.666,0.666,0.666);
					glColor3f(0.545, 0.271, 0.075);
					glScalef(1.333,0.1,3.55);
					glutSolidCube(1);
				glPopMatrix();
				//Cups
				glPushMatrix();
					glTranslatef(0.35,0,0);
					scene.drawCup();
				glPopMatrix();
				glPushMatrix();
					glTranslatef(-0.35,0,0);
					scene.drawCup();
				glPopMatrix();
				glPushMatrix();
					glTranslatef(0,0,-.43);
					scene.drawCup();
				glPopMatrix();
				glPushMatrix();
					glTranslatef(0,0,-2.00);
					scene.drawCup();
				glPopMatrix();
			    //Ping pong ball
				glPushMatrix();
	      			glColor3f(1.0,0.6,0.2);
	      			glTranslatef(0,0.7,-.25);
	      			gluQuadricNormals(qobj, GLU_SMOOTH);
	      			gluSphere(qobj, .1, 15.0, 5.0);
	   			glPopMatrix(); 

			glPopMatrix();	
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
	glEnable (GL_DEPTH_TEST| GL_DEPTH_BUFFER_BIT);
	qobj = gluNewQuadric();
	scene.findVerts();
	scene.genSurfNorms();
	scene.genVertNorms();
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