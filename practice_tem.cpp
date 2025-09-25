#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

#define WinX 800
#define WinY 600
#define maxRect 5

#define STOP 0

#define LU 11
#define LD 12
#define RU 13
#define RD 14

#define LEFT 21
#define RIGHT 22

GLfloat c2GLx(int x) {
	return (2.0f * x) / WinX - 1.0f;
}

GLfloat c2GLy(int y) {
	return 1.0f - (2.0f * y) / WinY;
}

GLfloat c2GLrgb(int c) {
	return (GLfloat)(c / 255.0f);
}

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
void Mouse(int button, int state, int x, int y);
void Keyboard(unsigned char key, int x, int y);
void Motion(int x, int y);
void TimerFunction(int value);

typedef struct _CRECT {
	int left;
	int bottom;
	int sizeX;
	int sizeY;
	int RGB[3];
	int mode;
	bool toggles[4];

} cRect;

cRect rects[5];
cRect orgRect[5];

int rectIdx = 0;
int mx = 0;
int my = 0;

int sel = -1;

void main(int argc, char** argv)
{
	srand(time(NULL));


	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(WinX, WinY);
	glutCreateWindow("practice2");
	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized\n";
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);
	glutKeyboardFunc(Keyboard);
	glutMotionFunc(Motion);
	glutTimerFunc(100, TimerFunction, 1);
	glutMainLoop();
}

GLvoid drawScene()
{
	glClearColor(c2GLrgb(95), c2GLrgb(95), c2GLrgb(95), 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	for (int i = 0; i < rectIdx; ++i) {
		
	}
	glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);

}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'q':
		exit(0);
		break;

	}

	drawScene();
}

void Mouse(int button, int state, int x, int y)
{
	mx = x;
	my = y;

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		
	}

	drawScene();
}

void Motion(int x, int y)
{
	
}

void TimerFunction(int value)
{
	glutPostRedisplay();
	glutTimerFunc(100, TimerFunction, 1);
}
