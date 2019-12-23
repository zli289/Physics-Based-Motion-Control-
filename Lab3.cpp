// window
#include "stdafx.h"

// standard
#include <assert.h>
#include <math.h>

// glut
#include <GL/glut.h>

#include "vec3.h"

// screen size
int g_screenWidth = 0;
int g_screenHeight = 0;

// Total number of balls
static int number = 20;

vec3 velocity[20], velocity_Next[20];
vec3 position[20], position_Next[20];
// time
GLfloat t = 0.05f;
//acceleration
vec3 a(0, -2.0, 0 ); 

//the matrix for balls
GLfloat Matrix[20][16]; 

//coefficient of collision
GLfloat e = 0.8f; 
//floor 
GLfloat edge= -12.0f;

void init()
{
	for (int j = 0; j<number; j++)
	{
		position[j]=vec3(RandomFloat(-5.0,5.0),RandomFloat(8.0,10.0),RandomFloat(0.0,1.0));
		velocity[j]=vec3(RandomFloat(-1.0,1.5),0,0);
		for (int i = 0; i<3; i++)
		{
			Matrix[j][i*5]=1.0f;
			Matrix[j][12 + i] = position[j][i];
		}
		Matrix[j][15] = 1.0f;
	}
}

void ballcollision(int index) 
{
	// detect collision between current ball and all the other balls
	for (int i = index + 1; i < number; i++)
	{
		if ((position[index]-position[i]).magnitude()<0.6) 
		{
			// calculate the x_axis for current ball
			vec3 x_axis= (position[i]-position[index]).normalize();
		
			// calculate x1 y1
			vec3 x1 = x_axis*x_axis.dot(velocity[index]);
			vec3 y1 = velocity[index]-x1;

			// calculate the x_axis for the other ball
			x_axis = (position[index]-position[i]).normalize();

			// calculate x2 y2
			vec3 x2=x_axis* x_axis.dot(velocity[i]);
			vec3 y2=velocity[i]-x2;

			velocity[index] = x2 + y1;
			velocity[i] = x1 + y2;
		}
	}
}

void floorcollision(int index) 
{
	if (position[index].y<0.5f) // Floor Collision Happens
		velocity[index].y *=-e; 	
	if(position[index].z<edge)
			velocity[index].z*=-e;
}

void ballfalling(int index)
{
	floorcollision(index);
	ballcollision(index);
	velocity_Next[index] = velocity[index] + a*t;
	velocity[index] = velocity_Next[index];

	position_Next[index] = position[index] + velocity_Next[index]*t;
	position[index] = position_Next[index];

	Matrix[index][12] = position_Next[index].x;
	Matrix[index][13] = position_Next[index].y;
	Matrix[index][14] = position_Next[index].z;
}

void floor(){
	glBegin(GL_LINES);
	for (GLfloat x = -100; x < 100; x += 2.0f)
	{
		glVertex3f(x, 0, -100);
		glVertex3f(x, 0, 100);

		glVertex3f(x, -100, edge);
		glVertex3f(x, 100, edge);
	}
	for (GLfloat z = -150; z < 100; z += 2.0f)
	{
		glVertex3f(-150, 0, z); 
		glVertex3f(100, 0, z);

		glVertex3f(-100, z, edge);
		glVertex3f(100, z, edge);
	}


	glEnd();
}

void timer(int value) {
	// render
	glutPostRedisplay();
	// reset timer
	glutTimerFunc(16, timer, 0);
}

void render(void) {
	// clear buffer
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// render state
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);

	// enable lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// light source attributes
	GLfloat LightAmbient[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat LightDiffuse[] = { 0.4f, 0.4f, 0.4f, 1.0f };
	GLfloat LightSpecular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat LightPosition[] = { 5.0f, 5.0f, 5.0f, 1.0f };

	glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, LightSpecular);
	glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);

	// surface material attributes
	GLfloat material_Ka[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	GLfloat material_Kd[] = { 0.43f, 0.47f, 0.54f, 1.0f };
	GLfloat material_Ks[] = { 0.33f, 0.33f, 0.52f, 1.0f };
	GLfloat material_Ke[] = { 0.1f, 0.0f, 0.1f, 1.0f };
	GLfloat material_Se = 10;

	glMaterialfv(GL_FRONT, GL_AMBIENT, material_Ka);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_Kd);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_Ks);
	glMaterialfv(GL_FRONT, GL_EMISSION, material_Ke);
	glMaterialf(GL_FRONT, GL_SHININESS, material_Se);

	// modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 15.0, 15.0,0.0, 0.0, 0.0,0.0, 1.0, 0.0);

	// animation	
	floor();
	for (int i = 0; i < number; i++)
	{
		glPushMatrix();
		ballfalling(i);
		glMultMatrixf(Matrix[i]);
		glutSolidSphere(0.5, 20, 20);
		glPopMatrix();
	}

	// disable lighting
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);

	// swap back and front buffers
	glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) 
{
}

void reshape(int w, int h) {
	// screen size
	g_screenWidth = w;
	g_screenHeight = h;
	// viewport
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	// projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(65.0, (GLfloat)w / (GLfloat)h, 1.0, 30.0);
}

int main(int argc, char** argv) {
	// create opengL window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Lab 3");
	// user initialization
	init();
	// set callback functions
	glutDisplayFunc(render);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutTimerFunc(16, timer, 0);
	// main loop
	glutMainLoop();

	return 0;
}