#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define SKYBOX_RADIUS 200
#define NUM_SLICES	20
#define NUM_SHELLS	50
#define NUM_TILES  5
#define MOUSE_X_SENSITIVITY .001
#define MOUSE_Y_SENSITIVITY .001
#define DELTA_TIME 50
#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900


#define PI 3.14159265





void glut_setup(void);
void gl_setup(void);
void my_setup(void);
void texture_setup(void);
void mouse_motion(int x, int y);
void my_display(void);
void my_reshape(int w, int h);
void my_keyboard(unsigned char key, int x, int y);
void my_timer(int val);


float xpos;
float ypos;
float zpos;
float atx;
float aty;
float atz;
float upx;
float upy;
float upz;
GLubyte img1[1024 * 1024 * 3];
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


	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Asteroids: Skybox");
	glutFullScreen();
	glutSetCursor(GLUT_CURSOR_NONE);
	glutWarpPointer(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

	glutDisplayFunc(my_display);
	glutReshapeFunc(my_reshape);
	glutKeyboardFunc(my_keyboard);
	glutPassiveMotionFunc(mouse_motion);
	glutTimerFunc(DELTA_TIME, my_timer, 0);
	return;
}


void gl_setup(void) {


	glClearColor(0, 0, 0, 0);


	glEnable(GL_DEPTH_TEST);


	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(30.0, SCREEN_WIDTH / SCREEN_HEIGHT, 1.0, 410.0);

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
	atx = 1;
	aty = 0;
	atz = 0;
	upx = 0;
	upy = 1;
	upz = 0;
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
	case 'q':
	case 'Q':
		exit(0);
	default: break;
	}
	glutPostRedisplay();
	return;
}

void make_skybox(float radius, int num_tiles)
//Draws a cube of length radius centered at the player's position (designated by the triple (xpos, ypos, zpos))
//There are (num_tiles)^2 tiles on each face of the cube
{

	double z;
	double x;

	float dx = radius / num_tiles;
	int r = 17;
	int t;

	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);




	for (x = (-.5 * radius); x < (.5 * radius); x += dx)
		for (z = (-.5 * radius); z < (.5 * radius); z += dx)
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
		glVertex3f(xpos + x, ypos + -.5 * radius, zpos + z);

		if (t == 0)
			glTexCoord2f(1, 0);
		else if (t == 1)
			glTexCoord2f(1, 1);
		else
			glTexCoord2f(0, 1);
		glVertex3f(xpos + x + dx, ypos + -.5 * radius, zpos + z);

		if (t == 0)
			glTexCoord2f(1, 1);
		else if (t == 1)
			glTexCoord2f(1, 0);
		else
			glTexCoord2f(0, 0);
		glVertex3f(xpos + x + dx, ypos + -.5 * radius, zpos + z + dx);

		if (t == 0)
			glTexCoord2f(0, 1);
		else if (t == 1)
			glTexCoord2f(0, 0);
		else
			glTexCoord2f(1, 0);
		glVertex3f(xpos + x, ypos + -.5 * radius, zpos + z + dx);
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
		glVertex3f(xpos + x, ypos + .5 * radius, zpos + z);

		if (t == 0)
			glTexCoord2f(1, 0);
		else if (t == 1)
			glTexCoord2f(1, 1);
		else
			glTexCoord2f(0, 1);
		glVertex3f(xpos + x + dx, ypos + .5 * radius, zpos + z);

		if (t == 0)
			glTexCoord2f(1, 1);
		else if (t == 1)
			glTexCoord2f(1, 0);
		else
			glTexCoord2f(0, 0);
		glVertex3f(xpos + x + dx, ypos + .5 * radius, zpos + z + dx);

		if (r == 0)
			glTexCoord2f(0, 1);
		else if (r == 1)
			glTexCoord2f(0, 0);
		else
			glTexCoord2f(1, 0);
		glVertex3f(xpos + x, ypos + .5 * radius, zpos + z + dx);
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
		glVertex3f(xpos + x, ypos + z, zpos + -.5 * radius);

		if (t == 0)
			glTexCoord2f(1, 0);
		else if (t == 1)
			glTexCoord2f(1, 1);
		else
			glTexCoord2f(0, 1);
		glVertex3f(xpos + x + dx, ypos + z, zpos + -.5 * radius);

		if (t == 0)
			glTexCoord2f(1, 1);
		else if (t == 1)
			glTexCoord2f(1, 0);
		else
			glTexCoord2f(0, 0);
		glVertex3f(xpos + x + dx, ypos + z + dx, zpos + -.5 * radius);

		if (t == 0)
			glTexCoord2f(0, 1);
		else if (t == 1)
			glTexCoord2f(0, 0);
		else
			glTexCoord2f(1, 0);
		glVertex3f(xpos + x, ypos + z + dx, zpos + -.5 * radius);
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
		glVertex3f(xpos + x, ypos + z, zpos + .5 * radius);

		if (t == 0)
			glTexCoord2f(1, 0);
		else if (t == 1)
			glTexCoord2f(1, 1);
		else
			glTexCoord2f(0, 1);
		glVertex3f(xpos + x + dx, ypos + z, zpos + .5 * radius);

		if (t == 0)
			glTexCoord2f(1, 1);
		else if (t == 1)
			glTexCoord2f(1, 0);
		else
			glTexCoord2f(0, 0);
		glVertex3f(xpos + x + dx, ypos + z + dx, zpos + .5 * radius);

		if (t == 0)
			glTexCoord2f(0, 1);
		else if (t == 1)
			glTexCoord2f(0, 0);
		else
			glTexCoord2f(1, 0);
		glVertex3f(xpos + x, ypos + z + dx, zpos + .5 * radius);
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
		glVertex3f(xpos + .5 * radius, ypos + x, zpos + z);

		if (t == 0)
			glTexCoord2f(1, 0);
		else if (t == 1)
			glTexCoord2f(1, 1);
		else
			glTexCoord2f(0, 1);
		glVertex3f(xpos + .5 * radius, ypos + x + dx, zpos + z);

		if (t == 0)
			glTexCoord2f(1, 1);
		else if (t == 1)
			glTexCoord2f(1, 0);
		else
			glTexCoord2f(0, 0);
		glVertex3f(xpos + .5 * radius, ypos + x + dx, zpos + z + dx);

		if (t == 0)
			glTexCoord2f(0, 1);
		else if (t == 1)
			glTexCoord2f(0, 0);
		else
			glTexCoord2f(1, 0);
		glVertex3f(xpos + .5 * radius, ypos + x, zpos + z + dx);
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
		glVertex3f(xpos + -.5 * radius, ypos + x, zpos + z);

		if (t == 0)
			glTexCoord2f(1, 0);
		else if (t == 1)
			glTexCoord2f(1, 1);
		else
			glTexCoord2f(0, 1);
		glVertex3f(xpos + -.5 * radius, ypos + x + dx, zpos + z);

		if (t == 0)
			glTexCoord2f(1, 1);
		else if (t == 1)
			glTexCoord2f(1, 0);
		else
			glTexCoord2f(0, 0);
		glVertex3f(xpos + -.5 * radius, ypos + x + dx, zpos + z + dx);

		if (t == 0)
			glTexCoord2f(0, 1);
		else if (t == 1)
			glTexCoord2f(0, 0);
		else
			glTexCoord2f(1, 0);
		glVertex3f(xpos + -.5 * radius, ypos + x, zpos + z + dx);
		glEnd();
		}




}

void cross(float *res, float a1, float a2, float a3, float b1, float b2, float b3)
{
	res[0] = a2 * b3 - a3 * b2;
	res[1] = a3 * b1 - a1 * b3;
	res[2] = a1 * b2 - a2 * b1;
}

float magnitude(float x, float y, float z) //"POP POP!"
{
	return sqrt(x * x + y * y + z * z);
}

void mouse_motion(int x, int y)
{
	float mag;
	float res[3];

	cross(&res[0], upx, upy, upz, atx, aty, atz);
	upx += atx * MOUSE_Y_SENSITIVITY * (y - SCREEN_HEIGHT / 2);
	upy += aty * MOUSE_Y_SENSITIVITY * (y - SCREEN_HEIGHT / 2);
	upz += atz * MOUSE_Y_SENSITIVITY * (y - SCREEN_HEIGHT / 2);
	atx += upx * MOUSE_Y_SENSITIVITY * (y - SCREEN_HEIGHT / 2);
	aty += upy * MOUSE_Y_SENSITIVITY * (y - SCREEN_HEIGHT / 2);
	atz += upz * MOUSE_Y_SENSITIVITY * (y - SCREEN_HEIGHT / 2);
	mag = magnitude(upx, upy, upz);
	upx = upx / mag;
	upy = upy / mag;
	upz = upz / mag;

	atx = res[0];
	aty = res[1];
	atz = res[2];

	cross(&res[0], upx, upy, upz, atx, aty, atz);
	atx = -res[0] + atx * MOUSE_X_SENSITIVITY * (SCREEN_WIDTH / 2 - x);
	aty = -res[1] + aty * MOUSE_X_SENSITIVITY * (SCREEN_WIDTH / 2 - x);
	atz = -res[2] + atz * MOUSE_X_SENSITIVITY * (SCREEN_WIDTH / 2 - x);
	mag = magnitude(atx, aty, atz);
	atx = atx / mag;
	aty = aty / mag;
	atz = atz / mag;

	glutWarpPointer(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

	my_display();
}


void my_display(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(xpos, ypos, zpos,
		atx, aty, atz,
		upx, upy, upz);

	make_skybox(SKYBOX_RADIUS, NUM_TILES);


	glutSwapBuffers();

	return;
}

void my_timer(int val) {

	glutPostRedisplay();
	glutTimerFunc(DELTA_TIME, my_timer, 0);
	return;
}
