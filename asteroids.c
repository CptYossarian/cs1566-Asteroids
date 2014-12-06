#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define min(a,b) ((a) < (b)? a:b)

#define SKYBOX_RADIUS 200
#define NUM_SLICES	20
#define NUM_SHELLS	50
#define NUM_TILES  6
#define MOUSE_X_SENSITIVITY .001
#define MOUSE_Y_SENSITIVITY .001
#define DELTA_TIME 50
#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900
#define ASTEROID_TEX  0
#define SKYBOX_TEX 1
# define maxFaces 1000
# define maxVertices 200
# define recursionLevel 2
#define nAsteroids 10


#define PI 3.14159265
# define phi 1.618


struct asteroid {
	float size;
	float position[3];
	float angle[3];
	float dir[3];
	GLfloat Texture[maxVertices];
};

void glut_setup(void);
void gl_setup(void);
void my_setup(void);
void texture_setup(void);
void mouse_motion(int x, int y);
void my_display(void);
void my_reshape(int w, int h);
void my_keyboard(unsigned char key, int x, int y);
void my_timer(int val);
void setup_tetrahedron(void);

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
static GLubyte ast_img[512*1024* 3];
GLuint tex_name[2];
int nFaces;
int iFaces;
int nVertices;
struct asteroid asteroids[10];



GLfloat tetrahedronFaces[20][3] = {
		{ 0, 11, 5 },
		{ 0, 5, 1 },
		{ 0, 1, 7 },
		{ 0, 7, 10 },
		{ 0, 10, 11 },
		{ 1, 5, 9 },
		{ 5, 11, 4 },
		{ 11, 10, 2 },
		{ 10, 7, 6 },
		{ 7, 1, 8 },
		{ 3, 9, 4 },
		{ 3, 4, 2 },
		{ 3, 2, 6 },
		{ 3, 6, 8 },
		{ 3, 8, 9 },
		{ 4, 9, 5 },
		{ 2, 4, 11 },
		{ 6, 2, 10 },
		{ 8, 6, 7 },
		{ 9, 8, 1 }
};

GLfloat tetrahedronVertices[12][3] = {
		{ -1, phi, 0 }, { 1, phi, 0 }, { -1, -phi, 0 }, { 1, -phi, 0 },
		{ 0, -1, phi }, { 0, 1, phi }, { 0, -1, -phi }, { 0, 1, -phi },
		{ phi, 0, -1 }, { phi, 0, 1 }, { -phi, 0, -1 }, { -phi, 0, 1 }
};

GLfloat tetrasphereVertices[maxVertices][3];
GLint   tetrasphereFaces[maxFaces][3];

int theta;
int display_mode;


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
	nFaces = 0;
	iFaces = 0;
	nVertices = 0;
	display_mode = 0;
	asteroids[0].position[0] = 50;
	asteroids[0].position[1] = 0;
	asteroids[0].position[2] = 0;
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
	glGenTextures(1, &tex_name[SKYBOX_TEX]);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	load_bmp(fopen("tile1.bmp", "rb"), img1, 600, 600, &tex_name[SKYBOX_TEX]);
	load_bmp(fopen("Asteroid.bmp", "rb"), ast_img, 512, 1024, &tex_name[ASTEROID_TEX]);
	setup_tetrahedron();


}

void my_keyboard(unsigned char key, int x, int y) {

	switch (key) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		display_mode = key - '0';
		glutPostRedisplay();
		break;
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
	glBindTexture(GL_TEXTURE_2D, tex_name[SKYBOX_TEX]);




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

int add_face(int i) {
	int N = nFaces++;
	tetrasphereFaces[N][0] = tetrahedronFaces[i][0];
	tetrasphereFaces[N][1] = tetrahedronFaces[i][1];
	tetrasphereFaces[N][2] = tetrahedronFaces[i][2];
	return N;
}

int add_vertex(int i) {
	int N = nVertices++;
	tetrasphereVertices[N][0] = tetrahedronVertices[i][0];
	tetrasphereVertices[N][1] = tetrahedronVertices[i][1];
	tetrasphereVertices[N][2] = tetrahedronVertices[i][2];

	float d = sqrt(tetrasphereVertices[N][0] * tetrasphereVertices[N][0] +
		tetrasphereVertices[N][1] * tetrasphereVertices[N][1] +
		tetrasphereVertices[N][2] * tetrasphereVertices[N][2]);
	tetrasphereVertices[N][0] /= d;
	tetrasphereVertices[N][1] /= d;
	tetrasphereVertices[N][2] /= d;
	return N;

}

int add_vertex_midpoint(a, b) {
	int N = nVertices++;

	//calculate midpoint
	tetrasphereVertices[N][0] = (tetrasphereVertices[a][0] + tetrasphereVertices[b][0]) / 2.0;
	tetrasphereVertices[N][1] = (tetrasphereVertices[a][1] + tetrasphereVertices[b][1]) / 2.0;
	tetrasphereVertices[N][2] = (tetrasphereVertices[a][2] + tetrasphereVertices[b][2]) / 2.0;

	//normalize to distance of 1
	float d = sqrt(tetrasphereVertices[N][0] * tetrasphereVertices[N][0] +
		tetrasphereVertices[N][1] * tetrasphereVertices[N][1] +
		tetrasphereVertices[N][2] * tetrasphereVertices[N][2]);
	tetrasphereVertices[N][0] /= d;
	tetrasphereVertices[N][1] /= d;
	tetrasphereVertices[N][2] /= d;

	//check to see if the vertex already exists
	int i;
	float r[3];
	for (i = 0; i<nVertices - 1; i++) {
		r[0] = tetrasphereVertices[N][0] - tetrasphereVertices[i][0]; r[0] *= r[0];
		r[1] = tetrasphereVertices[N][1] - tetrasphereVertices[i][1]; r[1] *= r[1];
		r[2] = tetrasphereVertices[N][2] - tetrasphereVertices[i][2]; r[2] *= r[2];
		float rr = r[0] + r[1] + r[2];
		if (rr<0.001) {
			nVertices--;
			return i;
		}
	}
	return N;
}

int add_traingle(a, b, c) {
	int N = nFaces++;
	tetrasphereFaces[N][0] = a;
	tetrasphereFaces[N][1] = b;
	tetrasphereFaces[N][2] = c;
	return N;
}

void setup_tetrahedron() {
	srand(time(NULL));

	int i, j;//,factor;
	while (nVertices < 12) add_vertex(nVertices);
	while (nFaces < 20) add_face(nFaces);

	//add vertices to smooth out tertrahedron
	for (i = 0; i<recursionLevel; i++){
		int N = nFaces;
		while (iFaces<N) {

			int a = add_vertex_midpoint(tetrasphereFaces[iFaces][0], tetrasphereFaces[iFaces][1]);
			int b = add_vertex_midpoint(tetrasphereFaces[iFaces][1], tetrasphereFaces[iFaces][2]);
			int c = add_vertex_midpoint(tetrasphereFaces[iFaces][2], tetrasphereFaces[iFaces][0]);

			add_traingle(tetrasphereFaces[iFaces][0], c, a);
			add_traingle(tetrasphereFaces[iFaces][1], b, a);
			add_traingle(tetrasphereFaces[iFaces][2], c, b);
			add_traingle(a, b, c);

			iFaces++;
		}
	}

	//for each "asteroid"
	int asteroidNo = 0;
	for (asteroidNo = 0; asteroidNo<10; asteroidNo++){

		//just for setting sizes for testing
		asteroids[asteroidNo].size = (rand()*1.0 / (1.0*RAND_MAX)) * 4 + 1;

		//randomize vertices
		float k = (rand()*1.0 / (1.0*RAND_MAX)) * 360;
		float r = 0;
		float factor;
		for (factor = 1; factor <= 8; factor *= 2){
			for (i = 0; i<nVertices; i++){
				r = (rand()*1.0 / (1.0*RAND_MAX)) * 2 - 1; //random number -1 to 1
				k += r * 4;
				asteroids[asteroidNo].Texture[i] += sin((3.14 / 180) * k / factor) / factor;

				//printf("%f\t%f\t%f\t%f\n",r,k,sin( (3.14 / 180) *  k / factor ) * factor,rockTexture[i]);
			}
		}

		//find max and min
		float max = 0;
		float min = 1000000;
		for (i = 0; i<nVertices; i++) {
			asteroids[asteroidNo].Texture[i] = fabsf(asteroids[asteroidNo].Texture[i]);
			max = (asteroids[asteroidNo].Texture[i] >(max)) ? asteroids[asteroidNo].Texture[i] : max;
			min = (asteroids[asteroidNo].Texture[i] < (min)) ? asteroids[asteroidNo].Texture[i] : min;
		}

		//to normalize variation down to variation from min-max
		//printf("\t%d\n", asteroidNo);
		for (i = 0; i<nVertices; i++) {
			asteroids[asteroidNo].Texture[i] = asteroids[asteroidNo].size + asteroids[asteroidNo].size / 5 * (((asteroids[asteroidNo].Texture[i] - min) / max) - 0.5);
			//printf("%f\n", asteroids[asteroidNo].Texture[i]);
		}
	}
}

void make_tetrahedron_triangle(int asteroidNo, int faceNo) {
	int xyz; float a[3], b[3], c[3];
	for (xyz = 0; xyz<3; xyz++) {
		a[xyz] =//tetrasphereVertices[tetrasphereFaces[faceNo][0]][xyz]+
			(tetrasphereVertices[tetrasphereFaces[faceNo][0]][xyz] * asteroids[asteroidNo].Texture[tetrasphereFaces[faceNo][0]]);
		b[xyz] =//tetrasphereVertices[tetrasphereFaces[faceNo][1]][xyz]+
			(tetrasphereVertices[tetrasphereFaces[faceNo][1]][xyz] * asteroids[asteroidNo].Texture[tetrasphereFaces[faceNo][1]]);
		c[xyz] =//tetrasphereVertices[tetrasphereFaces[faceNo][2]][xyz]+
			(tetrasphereVertices[tetrasphereFaces[faceNo][2]][xyz] * asteroids[asteroidNo].Texture[tetrasphereFaces[faceNo][2]]);
	}

	glColor3f(1.0, 0, 0);
	glBegin(GL_POLYGON);
	{
		float nx = (a[0] + b[0] + c[0]) / 3.0;
		float ny = (a[1] + b[1] + c[1]) / 3.0;
		float nz = (a[2] + b[2] + c[2]) / 3.0;
		glNormal3f(nx, ny, nz);
		glVertex3fv(a);
		glVertex3fv(b);
		glVertex3fv(c);
	}
	glEnd();
}

void make_tetrahedron(int asteroidNo) {
	int faceNo = iFaces;
	while (faceNo<nFaces) make_tetrahedron_triangle(asteroidNo, faceNo++);
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

	GLfloat stripe_plane_s[] = { 1., 1., 0, 1 };
	GLfloat twod_plane_s[] = { 1, 0, 0, 0 };
	GLfloat twod_plane_t[] = { 0, 1, 0, 0 };
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(xpos, ypos, zpos,
		atx, aty, atz,
		upx, upy, upz);


	make_skybox(SKYBOX_RADIUS, NUM_TILES);

	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, tex_name[ASTEROID_TEX]);

	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);

	glTexGenfv(GL_S, GL_OBJECT_PLANE, twod_plane_s);
	glTexGenfv(GL_T, GL_OBJECT_PLANE, twod_plane_t);

	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);

	make_tetrahedron(display_mode);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);

	glutSwapBuffers();

	return;
}

void my_timer(int val) {

	glutPostRedisplay();
	glutTimerFunc(DELTA_TIME, my_timer, 0);
	return;
}
