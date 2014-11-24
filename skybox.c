#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define SKYBOX_RADIUS 200
#define NUM_TILES  5

#define PI 3.14159265





void glut_setup(void);
void gl_setup(void);
void my_setup(void);
void texture_setup(void);
void my_display(void);
void my_reshape(int w, int h);
void my_keyboard(unsigned char key, int x, int y);
void my_timer(int val);

int otheta;
int xtheta;
float xpos;
float ypos;
float zpos;
GLubyte img1[1024 * 1024 * 3];
GLubyte img2[1024 * 1024 * 3];
GLuint tex_name1;


void bmp2rgb(GLubyte img[], int size) {
	int i;
	GLubyte temp;

	for (i = 0; i<size; i += 3) {
		temp = img[i + 2];
		img[i + 2] = img[i + 1];
		img[i + 1] = temp;

	}
}

void load_bmp(FILE *fp, GLubyte img[], int width, int height, GLuint *ptname) {


	fseek(fp, 8, SEEK_SET);
	fread(img, width*height * 3, 1, fp);
	bmp2rgb(img, width*height * 3);

	if (ptname) {

		glBindTexture(GL_TEXTURE_2D, *ptname);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
			width, height,
			0, GL_RGB, GL_UNSIGNED_BYTE, img);
	}

}

int main(int argc, char **argv) {

	
	glutInit(&argc, argv);

	
	glut_setup();
	gl_setup();
	my_setup();

	glutMainLoop();

	return(0);
}


void glut_setup(void) {

	
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	
	glutInitWindowSize(400, 400);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Asteroids: Skybox");

	
	glutDisplayFunc(my_display);
	glutReshapeFunc(my_reshape);
	glutKeyboardFunc(my_keyboard);
	return;
}


void gl_setup(void) {

	
	glClearColor(0, 0, 0, 0);

	
	glEnable(GL_DEPTH_TEST);

	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	gluPerspective(30.0, 1.0, 1.0, 410.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glShadeModel(GL_SMOOTH);
	texture_setup();

	return;
}

void my_setup(void) {
	xpos = 0;
	ypos = 0;
	zpos = 0;
	otheta = 0;
	xtheta = 0;
	return;
}

void my_reshape(int w, int h) {

	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(30.0, w / h, 1.0, 2 * SKYBOX_RADIUS);
	
	return;
}

void texture_setup() {
	glGenTextures(1, &tex_name1);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	load_bmp(fopen("6.bmp", "rb"), img1, 1024, 1024, &tex_name1);
	

}

void my_keyboard(unsigned char key, int x, int y) {

	switch (key) {
	case 'y':
		otheta = (otheta + 5) % 360;
		break;
	case 'x':
		xtheta = (xtheta + 5) % 360;
		break;
	case 'q':
	case 'Q':
		exit(0);
	default: break;
	}
	glutTimerFunc(100, my_timer, 0);
	return;
}

void make_skybox()
//Draws a cube of length SKYBOX_RADIUS centered at the player's position (designated by the triple (xpos, ypos, zpos))
//There are (NUM_TILES)^2 tiles on each face of the cube
{

	float z;
	float x;

	float dx = SKYBOX_RADIUS / NUM_TILES;
	int r = 17;
	int t;
	
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	
	

		
		for (x = (-.5 * SKYBOX_RADIUS); x < (.5 * SKYBOX_RADIUS); x += dx)
			for (z = (-.5 * SKYBOX_RADIUS); z < (.5 * SKYBOX_RADIUS); z += dx)
			{
				glBegin(GL_POLYGON);
				r = (r * r + 23) % 15;    //psuedo-random sequence; determines texture map orientation
				t = r % 3;		  		
				if (t == 0)
					glTexCoord2f(0, 0);
				else if (t == 1)
					glTexCoord2f(0, 1);
				else
					glTexCoord2f(1, 1);
				glVertex3f(xpos + x, ypos + -.5 * SKYBOX_RADIUS, zpos + z);
					
				if (t == 0)
					glTexCoord2f(1, 0);
				else if (t == 1)
					glTexCoord2f(1, 1);
				else
					glTexCoord2f(0, 1);
				glVertex3f(xpos + x + dx, ypos + -.5 * SKYBOX_RADIUS, zpos + z);
					
				if (t == 0)
					glTexCoord2f(1, 1);
				else if (t == 1)
					glTexCoord2f(1, 0);
				else
					glTexCoord2f(0, 0);
				glVertex3f(xpos + x + dx, ypos + -.5 * SKYBOX_RADIUS, zpos + z + dx);
					
				if (t == 0)
					glTexCoord2f(0, 1);
				else if (t == 1)
					glTexCoord2f(0, 0);	
				else
					glTexCoord2f(1, 0);
				glVertex3f(xpos + x, ypos + -.5 * SKYBOX_RADIUS, zpos + z + dx);
				glEnd();
///////////////////////////////////////////////////////////////////////
				glBegin(GL_POLYGON);
				r = (r * r + 23) % 15;
				t = r % 3;
				
				if (t == 0)
					glTexCoord2f(0, 0);
				else if (t == 1)
					glTexCoord2f(0, 1);
				else
					glTexCoord2f(1, 1);
				glVertex3f(xpos + x, ypos + .5 * SKYBOX_RADIUS, zpos + z);

				if (t == 0)
					glTexCoord2f(1, 0);
				else if (t == 1)
					glTexCoord2f(1, 1);
				else
					glTexCoord2f(0, 1);
				glVertex3f(xpos + x + dx, ypos + .5 * SKYBOX_RADIUS, zpos + z);

				if (t == 0)
					glTexCoord2f(1, 1);
				else if (t == 1)
					glTexCoord2f(1, 0);
				else
					glTexCoord2f(0, 0);
				glVertex3f(xpos + x + dx, ypos + .5 * SKYBOX_RADIUS, zpos + z + dx);

				if (r == 0)
					glTexCoord2f(0, 1);
				else if (r == 1)
					glTexCoord2f(0, 0);
				else
					glTexCoord2f(1, 0);
				glVertex3f(xpos + x, ypos + .5 * SKYBOX_RADIUS, zpos + z + dx);
				glEnd();
/////////////////////////////////////////////////////////////
				glBegin(GL_POLYGON);
				r = (r * r + 23) % 15;
				t = r % 3;
				
				if (t == 0)
					glTexCoord2f(0, 0);
				else if (t == 1)
					glTexCoord2f(0, 1);
				else
					glTexCoord2f(1, 1);
				glVertex3f(xpos + x, ypos + z, zpos + -.5 * SKYBOX_RADIUS);

				if (t == 0)
					glTexCoord2f(1, 0);
				else if (t == 1)
					glTexCoord2f(1, 1);
				else
					glTexCoord2f(0, 1);
				glVertex3f(xpos + x + dx, ypos + z, zpos + -.5 * SKYBOX_RADIUS);

				if (t == 0)
					glTexCoord2f(1, 1);
				else if (t == 1)
					glTexCoord2f(1, 0);
				else
					glTexCoord2f(0, 0);
				glVertex3f(xpos + x + dx, ypos + z + dx, zpos + -.5 * SKYBOX_RADIUS);

				if (t == 0)
					glTexCoord2f(0, 1);
				else if (t == 1)
					glTexCoord2f(0, 0);
				else
					glTexCoord2f(1, 0);
				glVertex3f(xpos + x, ypos + z + dx, zpos + -.5 * SKYBOX_RADIUS);
				glEnd();
//////////////////////////////////////////////////////////////
				glBegin(GL_POLYGON);
				r = (r * r + 23) % 15;
				t = r % 3;
				if (t == 0)
					glTexCoord2f(0, 0);
				else if (t == 1)
					glTexCoord2f(0, 1);
				else
					glTexCoord2f(1, 1);
				glVertex3f(xpos + x, ypos + z, zpos + .5 * SKYBOX_RADIUS);

				if (t == 0)
					glTexCoord2f(1, 0);
				else if (t == 1)
					glTexCoord2f(1, 1);
				else
					glTexCoord2f(0, 1);
				glVertex3f(xpos + x + dx, ypos + z, zpos + .5 * SKYBOX_RADIUS);

				if (t == 0)
					glTexCoord2f(1, 1);
				else if (t == 1)
					glTexCoord2f(1, 0);
				else
					glTexCoord2f(0, 0);
				glVertex3f(xpos + x + dx, ypos + z + dx, zpos + .5 * SKYBOX_RADIUS);

				if (t == 0)
					glTexCoord2f(0, 1);
				else if (t == 1)
					glTexCoord2f(0, 0);
				else
					glTexCoord2f(1, 0);
				glVertex3f(xpos + x, ypos + z + dx, zpos + .5 * SKYBOX_RADIUS);
				glEnd();
///////////////////////////////////////////////////////////////////////////////////
				glBegin(GL_POLYGON);
				r = (r * r + 23) % 15;
				t = r % 3;
				
				if (t == 0)
					glTexCoord2f(0, 0);
				else if (t == 1)
					glTexCoord2f(0, 1);
				else
					glTexCoord2f(1, 1);
				glVertex3f(xpos + .5 * SKYBOX_RADIUS, ypos + x, zpos + z);

				if (t == 0)
					glTexCoord2f(1, 0);
				else if (t == 1)
					glTexCoord2f(1, 1);
				else
					glTexCoord2f(0, 1);
				glVertex3f(xpos + .5 * SKYBOX_RADIUS, ypos + x + dx, zpos + z);

				if (t == 0)
					glTexCoord2f(1, 1);
				else if (t == 1)
					glTexCoord2f(1, 0);
				else
					glTexCoord2f(0, 0);
				glVertex3f(xpos + .5 * SKYBOX_RADIUS, ypos + x + dx, zpos + z + dx);

				if (t == 0)
					glTexCoord2f(0, 1);
				else if (t == 1)
					glTexCoord2f(0, 0);
				else
					glTexCoord2f(1, 0);
				glVertex3f(xpos + .5 * SKYBOX_RADIUS, ypos + x, zpos + z + dx);
				glEnd();
/////////////////////////////////////////////////////////////////
				glBegin(GL_POLYGON);
				r = (r * r + 23) % 15;
				t = r % 3;
				if (t == 0)
					glTexCoord2f(0, 0);
				else if (t == 1)
					glTexCoord2f(0, 1);
				else
					glTexCoord2f(1, 1);
				glVertex3f(xpos + -.5 * SKYBOX_RADIUS, ypos + x, zpos + z);

				if (t == 0)
					glTexCoord2f(1, 0);
				else if (t == 1)
					glTexCoord2f(1, 1);
				else
					glTexCoord2f(0, 1);
				glVertex3f(xpos + -.5 * SKYBOX_RADIUS, ypos + x + dx, zpos + z);

				if (t == 0)
					glTexCoord2f(1, 1);
				else if (t == 1)
					glTexCoord2f(1, 0);
				else
					glTexCoord2f(0, 0);
				glVertex3f(xpos + -.5 * SKYBOX_RADIUS, ypos + x + dx, zpos + z + dx);

				if (t == 0)
					glTexCoord2f(0, 1);
				else if (t == 1)
					glTexCoord2f(0, 0);
				else
					glTexCoord2f(1, 0);
				glVertex3f(xpos + -.5 * SKYBOX_RADIUS, ypos + x, zpos + z + dx);
				glEnd();
			}


		
		
}




void my_display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(xpos, ypos, zpos, 100.0, 0.0, 0.0, 0.0, 1.0, 0.0); 

	glRotatef(otheta, 0, 1, 0);
	glRotatef(xtheta, 0, 0, 1);
	make_skybox();


	glutSwapBuffers();

	return;
}

void my_timer(int val) {

	glutPostRedisplay();
	return;
}
