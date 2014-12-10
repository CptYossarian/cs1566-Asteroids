#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define NUM_SLICES	20
#define NUM_SHELLS	50
#define NUM_TILES  6
#define MOUSE_X_SENSITIVITY .001
#define MOUSE_Y_SENSITIVITY .001
#define DELTA_TIME 100
#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 600
#define NUM_TEXTURES  2
#define ASTEROID_TEX  0
#define SKYBOX_TEX 1
#define maxFaces 1000
#define maxVertices 200
#define recursionLevel 2
#define nAsteroids 100

#define SKYBOX_RADIUS 100
#define MAX_VELOCITY .3
#define MAX_ANGULAR_VELOCITY 1
#define MAX_SIZE 1

#define SHOT_LENGTH 100  //distance shot can travel
#define SHOT_SIZE .05    //size of shot
#define SHOT_NUM 10      //max num of shots at a time

/* checkerboard texture */
#define stripeImageWidth 32
#define checkImageWidth  64
#define checkImageHeight 64
#define smallWidth      256
#define mediumWidth     512
#define largeWidth     1024

#define min(a,b) ((a) < (b)? a:b)
#define FALSE 0
#define TRUE  1

void my_init(int argc, char **argv);
void glut_setup();
void gl_setup();
void my_setup();
void lighting_setup();
void texture_setup();

void my_display(void);
void my_reshape(int w, int h);
void my_keyboard(unsigned char key, int x, int y);
void my_idle(int val);
void my_timer(int val);
void mouse_motion(int x, int y);
void make_skybox(float radius, int num_tiles);

void draw_shots();
void update_shots();
void new_shot();
void collision_detect(struct shot temp);
void mouse_click(int button, int state, int x, int y);


void setup_tetrahedron(void);

# define pi 3.14159
# define phi 1.618

int nFaces = 0;
int iFaces = 0;
int nVertices = 0;
#define nnFaces 420
#define iiFaces 100
#define nnVertices 162



# define maxFaces 1000
# define maxVertices 200
# define recursionLevel 2

float xpos;
float ypos;
float zpos;
float atx;
float aty;
float atz;
float upx;
float upy;
float upz;
float theta = 0.0;
int fired; //curr number of shots fired

struct asteroid {
	float size;
	float position[3];
	float velocity[3];
	float angle[3];
	float omega[3];
	GLfloat Texture[maxVertices];
};

struct asteroid asteroids[nAsteroids];

struct Vector3 {
	float x;
	float y;
	float z;
};

//info for shots
struct shot {
	struct Vector3 dir;
	struct Vector3 pos;
	float dist;
};

struct shot shots[SHOT_NUM];   //holds all shots currently firing

GLfloat tetrahedronVertices[12][3] = {
		{ -1, phi, 0 }, { 1, phi, 0 }, { -1, -phi, 0 }, { 1, -phi, 0 },
		{ 0, -1, phi }, { 0, 1, phi }, { 0, -1, -phi }, { 0, 1, -phi },
		{ phi, 0, -1 }, { phi, 0, 1 }, { -phi, 0, -1 }, { -phi, 0, 1 }
};

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

GLfloat tetrasphereVertices[maxVertices][3];
GLint   tetrasphereFaces[maxFaces][3];

int   display_mode = 0;
int player_health;

static GLuint  tex_names[NUM_TEXTURES];
static GLubyte ast_img[mediumWidth*largeWidth * 3];
GLubyte img1[600 * 600 * 3];

float getRandomFloat(float n) {
	float i = (rand()*n) / RAND_MAX;
	return i;
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


int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glut_setup();
	gl_setup();
	my_setup();
	//my_display();

	glutMainLoop();
	return(0);
}

void glut_setup(void) {

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutInitWindowPosition(20, 20);
	glutCreateWindow("Asteroids: Skybox");

	glutSetCursor(GLUT_CURSOR_NONE);
	glutWarpPointer(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

	glutDisplayFunc(my_display);
	glutReshapeFunc(my_reshape);
	glutKeyboardFunc(my_keyboard);
	glutPassiveMotionFunc(mouse_motion);
	glutTimerFunc(DELTA_TIME, my_timer, 0);
	glutMouseFunc(mouse_click);
	//glutIdleFunc(my_idle);
	return;
}

//
//void glut_setup (){
//
//    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
//
//    glutInitWindowSize(SCREEN_WIDTH,SCREEN_HEIGHT);
//    glutInitWindowPosition(20,20);
//    glutCreateWindow("lights");
//
//    /* set up callback functions */
//    glutDisplayFunc(my_display);
//    glutReshapeFunc(my_reshape);
//    glutKeyboardFunc(my_keyboard);
//
//    glutTimerFunc(DELTA_TIME, my_timer, 0);
//
//
//
//
//    return;
//}

void gl_setup(void) {

	// enable depth handling (z-buffer)
	glEnable(GL_DEPTH_TEST);

	// define the background color
	glClearColor(0, 0, 0, 0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// gluPerspective( 20, 1.0, 15, 100.0);
	gluPerspective(30, 1, 1, 200);//)

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();  // init modelview to identity

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
	fired = 0;

	player_health = 100;

	return;
}

void bmp2rgb(GLubyte img[], int size) {
	int i;
	GLubyte temp;
	for (i = 0; i<size; i += 3) {
		temp = img[i + 2];
		img[i + 2] = img[i + 1];
		img[i + 1] = temp;
	}
}

void load_bmp(char *fname, GLubyte img[], int size1, int size2, GLuint *ptname) {
	FILE *fp;
	fp = fopen(fname, "rb");
	if (fp == NULL) {
		fprintf(stderr, "unable to open texture file %s\n", fname);
		exit(1);
	}

	fseek(fp, 8, SEEK_SET);
	fread(img, size1*size2 * 3, 1, fp);
	bmp2rgb(img, size1*size2 * 3);
	fclose(fp);

	if (ptname) {
		// initialize the texture
		glGenTextures(1, ptname);
		glBindTexture(GL_TEXTURE_2D, *ptname);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
			size1, size2,
			0, GL_RGB, GL_UNSIGNED_BYTE, img);
	}
}

void texture_setup() {
	load_bmp("tile1.bmp", img1, 600, 600, &tex_names[SKYBOX_TEX]);
	load_bmp("Asteroid.bmp", ast_img, mediumWidth, largeWidth, &tex_names[ASTEROID_TEX]);
	setup_tetrahedron();

}

void my_reshape(int w, int h) {
	// ensure a square view port
	glViewport(0, 0, min(w, h), min(w, h));
	return;
}

void my_keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case ' ':
		my_idle(1);
		break;
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
		break;
	default: break;
	}
	return;
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

	//    float d = sqrt(tetrasphereVertices[N][0]*tetrasphereVertices[N][0]+
	//                   tetrasphereVertices[N][1]*tetrasphereVertices[N][1]+
	//                   tetrasphereVertices[N][2]*tetrasphereVertices[N][2]);
	float d = magnitude(tetrahedronVertices[N][0], tetrahedronVertices[N][1], tetrahedronVertices[N][2]);
	// (pop, pop!)
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

void setup_asteroids(void) {
	int asteroidNo, j;
	for (asteroidNo = 0; asteroidNo<nAsteroids; asteroidNo++){


		//everything else
		for (j = 0; j<3; j++) {
			asteroids[asteroidNo].position[j] = getRandomFloat(SKYBOX_RADIUS*1.0) - SKYBOX_RADIUS / 2.0;
			asteroids[asteroidNo].velocity[j] = getRandomFloat(MAX_VELOCITY *1.0) - MAX_VELOCITY / 2.0;
			asteroids[asteroidNo].angle[j] = getRandomFloat(360 * 1.0);
			asteroids[asteroidNo].omega[j] = getRandomFloat(MAX_ANGULAR_VELOCITY*1.0) - MAX_ANGULAR_VELOCITY / 2.0;
		}

		j = rand() % 3;
		if (j == 0)
			asteroids[asteroidNo].size = .7;
		else if (j == 1)
			asteroids[asteroidNo].size = 1.5;
		else
			asteroids[asteroidNo].size = 3.0;
		
	}


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

	//for (i=0;i<nAsteroids;i++) {
	//texure things
	//for each "asteroid"
	int asteroidNo;
	for (asteroidNo = 0; asteroidNo<nAsteroids; asteroidNo++){


		//just for setting sizes for testing
		asteroids[asteroidNo].size = (rand()*1.0 / (1.0*RAND_MAX))*3.0 + 3.0;

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
		//printf("\t%d\n",asteroidNo);
		for (i = 0; i<nVertices; i++) {
			asteroids[asteroidNo].Texture[i] = (asteroids[asteroidNo].Texture[i] - min) / max*0.4 + 0.8;
			//            asteroids[asteroidNo].size+
			//            asteroids[asteroidNo].size/5*(((asteroids[asteroidNo].Texture[i]-min)/max)-0.5);
			//printf("%f\n",asteroids[asteroidNo].Texture[i]);
		}



	}

	setup_asteroids();
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
	int faceNo = iiFaces;
	while (faceNo<nnFaces) make_tetrahedron_triangle(asteroidNo, faceNo++);
}

void draw_HUD()
{
	glColor3f(200.0 / 255.0, 200.0 / 255.0, 200.0 / 255.0);
	glBegin(GL_LINES);
	glVertex2f(SCREEN_WIDTH / 2.0 - 2.5, SCREEN_HEIGHT / 2.0 + 2.5);
	glVertex2f(SCREEN_WIDTH / 2.0 - 2.5, SCREEN_HEIGHT / 2.0 + 17.5);
	glEnd();

	glBegin(GL_LINES);
	glVertex2f(SCREEN_WIDTH / 2.0 + 2.5, SCREEN_HEIGHT / 2.0 + 2.5);
	glVertex2f(SCREEN_WIDTH / 2.0 + 2.5, SCREEN_HEIGHT / 2.0 + 17.5);
	glEnd();

	glBegin(GL_LINES);
	glVertex2f(SCREEN_WIDTH / 2.0 + 2.5, SCREEN_HEIGHT / 2.0 + 2.5);
	glVertex2f(SCREEN_WIDTH / 2.0 + 17.5, SCREEN_HEIGHT / 2.0 + 2.5);
	glEnd();

	glBegin(GL_LINES);
	glVertex2f(SCREEN_WIDTH / 2.0 + 2.5, SCREEN_HEIGHT / 2.0 - 2.5);
	glVertex2f(SCREEN_WIDTH / 2.0 + 17.5, SCREEN_HEIGHT / 2.0 - 2.5);
	glEnd();

	glBegin(GL_LINES);
	glVertex2f(SCREEN_WIDTH / 2.0 - 2.5, SCREEN_HEIGHT / 2.0 - 2.5);
	glVertex2f(SCREEN_WIDTH / 2.0 - 2.5, SCREEN_HEIGHT / 2.0 - 17.5);
	glEnd();

	glBegin(GL_LINES);
	glVertex2f(SCREEN_WIDTH / 2.0 + 2.5, SCREEN_HEIGHT / 2.0 - 2.5);
	glVertex2f(SCREEN_WIDTH / 2.0 + 2.5, SCREEN_HEIGHT / 2.0 - 17.5);
	glEnd();

	glBegin(GL_LINES);
	glVertex2f(SCREEN_WIDTH / 2.0 - 2.5, SCREEN_HEIGHT / 2.0 + 2.5);
	glVertex2f(SCREEN_WIDTH / 2.0 - 17.5, SCREEN_HEIGHT / 2.0 + 2.5);
	glEnd();

	glBegin(GL_LINES);
	glVertex2f(SCREEN_WIDTH / 2.0 - 2.5, SCREEN_HEIGHT / 2.0 - 2.5);
	glVertex2f(SCREEN_WIDTH / 2.0 - 17.5, SCREEN_HEIGHT / 2.0 - 2.5);
	glEnd();

	if (player_health <= 35)
		glColor3f(1, 0, 0);
	glBegin(GL_POLYGON);
	glVertex2f(0, 0);
	glVertex2f(0, 50);
	glVertex2f(200 * player_health / 100.0, 50);
	if (player_health == 100)
		glVertex2f(230, 0);
	else
		glVertex2f(200 * player_health / 100.0, 0);
	glEnd();
}

//Draws a cube of length radius centered at the player's position (designated by the triple (xpos, ypos, zpos))
//There are (num_tiles)^2 tiles on each face of the cube
void make_skybox(float radius, int num_tiles) {
	double z;
	double x;

	float dx = radius / num_tiles;
	int r = 17;
	int t;

	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glBindTexture(GL_TEXTURE_2D, tex_names[SKYBOX_TEX]);

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

void drawWholeScreen(void) {
	glColor3f(1, 0, 0);
	glBegin(GL_POLYGON);
	glVertex2d(0, 0);
	glVertex2d(SCREEN_WIDTH, 0);
	glVertex2d(SCREEN_WIDTH, SCREEN_HEIGHT);
	glVertex2d(0, SCREEN_HEIGHT);
	glEnd();
}


void my_display() {

	// enable depth handling (z-buffer)
	glEnable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30, 1, 1, SKYBOX_RADIUS * 2);//)

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();  // init modelview to identity

	GLfloat stripe_plane_s[] = { 1., 1., 0, 1 };
	GLfloat twod_plane_s[] = { 1, 0, 0, 0 };
	GLfloat twod_plane_t[] = { 0, 1, 0, 0 };

	// clear all pixels, reset depth
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();     // init to identity
	//    gluLookAt(0, 0, 20,  // x,y,z coord of the camera
	//              0.0, 0.0, 0.0,  // x,y,z coord of the origin
	//              0.0, 1.0, 0.0); // the direction of up (default is y-axis)

	gluLookAt(xpos, ypos, zpos,
		atx, aty, atz,
		upx, upy, upz);

	//    glDisable(GL_DEPTH_TEST);
	//    glDisable(GL_CULL_FACE);
	//    glDisable(GL_BLEND);
	//    glDisable(GL_TEXTURE_2D);
	//    glDisable(GL_LIGHTING);

	//glRotatef(theta, 0,1,0);

	make_skybox(SKYBOX_RADIUS, NUM_TILES);
	draw_shots();

	// 2d texture
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, tex_names[ASTEROID_TEX]);

	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);

	glTexGenfv(GL_S, GL_OBJECT_PLANE, twod_plane_s);
	glTexGenfv(GL_T, GL_OBJECT_PLANE, twod_plane_t);

	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);

	int astno;
	int dead = 0;
	for (astno = 0; astno < nAsteroids; astno++) {
		glPushMatrix();
		{
			glTranslatef(asteroids[astno].position[0],
				asteroids[astno].position[1],
				asteroids[astno].position[2]);

			glRotatef(asteroids[astno].angle[0], 1, 0, 0);
			glRotatef(asteroids[astno].angle[1], 0, 1, 0);
			glRotatef(asteroids[astno].angle[2], 0, 0, 1);

			// glScalef(0.1, 0.1, 0.1);
			glScalef(asteroids[astno].size, asteroids[astno].size, asteroids[astno].size);

			//glutSolidSphere(1, 20, 20);

			make_tetrahedron(astno);

			//            printf("Asteroid %d:%5.5f %5.5f %5.5f\n", astno,
			//                   asteroids[astno].position[0],
			//                   asteroids[astno].position[1],
			//                   asteroids[astno].position[2]);

		}
		glPopMatrix();
		float d = magnitude(asteroids[astno].position[0],
			asteroids[astno].position[1],
			asteroids[astno].position[2]);
		printf("Asteroid %d:%8.2f\n", astno, d);
		if (d<asteroids[astno].size)
			dead = 1;


	}





	//glutSolidSphere(0.5, 36, 36);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);

	glEnable(GL_COLOR);

	//HUD
	glClear(GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	if (dead) drawWholeScreen();

	draw_HUD();

	// this buffer is ready
	glutSwapBuffers();
}

void draw_shots() {
	int i;
	glColor3f(1, 0, 0);  //set color to red
	glDisable(GL_TEXTURE_2D);

	//draw all active shots
	for (i = 0; i<fired; i++) {
		glPushMatrix();
		glTranslated(shots[i].pos.x, shots[i].pos.y, shots[i].pos.z);
		glutSolidSphere(SHOT_SIZE, 10, 10);
		glPopMatrix();
	}

	glColor3f(.5, .5, .5);
}

//create new shot
void new_shot() {
	struct Vector3 direction;
	struct Vector3 poscoords;

	//get direction from current pos/view
	direction.x = atx - xpos;
	direction.y = aty - ypos;
	direction.z = atz - zpos;

	poscoords.x = xpos;
	poscoords.y = ypos;
	poscoords.z = zpos;

	//create new shot in array
	if (fired < SHOT_NUM) {
		shots[fired].dir = direction;
		shots[fired].pos = poscoords;
		shots[fired].dist = 0;
		fired++;
	}
}

//update shot positions
void update_shots() {
	int i;
	int j;

	for (i = 0; i<fired; i++) {
		if (shots[i].dist <= SHOT_LENGTH) {     //shot only active for SHOT_LENGTH distance
			shots[i].pos.x += shots[i].dir.x;
			shots[i].pos.y += shots[i].dir.y;
			shots[i].pos.z += shots[i].dir.z;
			shots[i].dist += 1;
			collision_detect(shots[i]);   //test for collisions
		}
		else {
			for (j = i; j<fired - 1; j++) {  //clear out inactive shots
				shots[j] = shots[j + 1];
			}
			fired--;
		}
		glutPostRedisplay();
	}
}

//detect collisions
void collision_detect(struct shot temp) {
	int i;
	float distance;
	float xd;
	float zd;
	float yd;

	//calculate distance between center of shot & center of asteroid, test if less than the sum of their diameters
	for (i = 0; i<nAsteroids; i++) {
		xd = (temp.pos.x - asteroids[i].position[0])*(temp.pos.x - asteroids[i].position[0]);
		yd = (temp.pos.y - asteroids[i].position[1])*(temp.pos.y - asteroids[i].position[1]);
		zd = (temp.pos.z - asteroids[i].position[2])*(temp.pos.z - asteroids[i].position[2]);
		distance = sqrt(xd + yd + zd);

		//FOR TESTING - just moves asteroid if collision
		if (distance < SHOT_SIZE + asteroids[i].size) {  //<------ .5 is size of all asteroids right now, needs to
			/*asteroids[i].velocity[0] = 0;    //be changed to actual individual asteroid size once Nick implements that
			asteroids[i].velocity[1] = 0;
			asteroids[i].velocity[2] = 0;*/

			asteroids[i].position[0] = 40;
			asteroids[i].position[1] = 40;
			asteroids[i].position[2] = 40;
		}
	}
}

void mouse_click(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		new_shot();
	}
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

	my_idle(0);
	my_display();
}

void my_timer(int val) {

	my_idle(val);

	//glutPostRedisplay();
	glutTimerFunc(DELTA_TIME, my_timer, 0);
	return;
}

void my_idle(int val) {
	int i, j;

	update_shots();

	for (i = 0; i<nAsteroids; i++) {
		for (j = 0; j<3; j++) {
			asteroids[i].position[j] += asteroids[i].velocity[j]; //+SKYBOX_RADIUS/2.0);
			//asteroids[i].position[j] = fmodf(asteroids[i].position[j], (SKYBOX_RADIUS*1.0));
			if (asteroids[i].position[j] > SKYBOX_RADIUS / 2.0) {
				asteroids[i].position[j] -= SKYBOX_RADIUS;
			}
			else if (asteroids[i].position[j] < SKYBOX_RADIUS / -2.0) {
				asteroids[i].position[j] += SKYBOX_RADIUS;
			}

			asteroids[i].angle[j] += asteroids[i].omega[j];
			if (asteroids[i].angle[j] > 360.0) {
				asteroids[i].angle[j] -= 360.0;
			}
			else if (asteroids[i].angle[j] < -360.0) {
				asteroids[i].angle[j] += 360.0;
			}
		}
	}

	theta += 1.0;
	if (theta>360.0) theta -= 360.0;

	glutPostRedisplay();

	return;
}

