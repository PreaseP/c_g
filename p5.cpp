#include <iostream>
#include <vector>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

#define WinX 800
#define WinY 600
#define defaultSize 10

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
	int mid[2];
	int sizeX;
	int sizeY;
	int RGB[3];
	bool protect;
} cRect;

std::vector<cRect> rects;

int mx = 0;
int my = 0;
int mSizeX = defaultSize * 2;
int mSizeY = defaultSize * 2;
int mRGB[3] = { 0, 0, 0 };

bool left_button = false;
int delRect = 0;

void setRect() {
	int initRect = rand() % 21 + 20;
	delRect = 0;

	for (int i =0; i < initRect; ++i) {
		cRect temp;
		temp.mid[0] = rand() % (WinX - 100) + 50;
		temp.mid[1] = rand() % (WinY - 100) + 50;
		temp.sizeX = defaultSize;
		temp.sizeY = defaultSize;
		for (int j = 0; j < 3; ++j)
			temp.RGB[j] = rand() % 256;
		temp.protect = false;
		rects.push_back(temp);
	}
}


void main(int argc, char** argv)
{
	srand(time(NULL));

	setRect();

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
	for (int i = 0; i < rects.size(); ++i) {
		glColor3f(c2GLrgb(rects[i].RGB[0]), c2GLrgb(rects[i].RGB[1]), c2GLrgb(rects[i].RGB[2]));
		glRectf(c2GLx(rects[i].mid[0] - rects[i].sizeX), c2GLy(rects[i].mid[1] + rects[i].sizeY),
			c2GLx(rects[i].mid[0] + rects[i].sizeX), c2GLy(rects[i].mid[1] - rects[i].sizeY));
	}

	if (left_button) {
		glColor3f(c2GLrgb(mRGB[0]), c2GLrgb(mRGB[1]), c2GLrgb(mRGB[2]));
		glRectf(c2GLx(mx - mSizeX), c2GLy(my + mSizeY), c2GLx(mx + mSizeX), c2GLy(my - mSizeY));
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

	case 'r':
		rects.clear();
		setRect();

		if (left_button) {
			for (int i = 0; i < rects.size(); ++i)
				rects[i].protect = true;
		}
		break;
	}

	drawScene();
}

void Mouse(int button, int state, int x, int y)
{
	mx = x;
	my = y;

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		left_button = true;
		mSizeX = defaultSize * 2;
		mSizeY = defaultSize * 2;
		for (int i = 0; i < 3; ++i)
			mRGB[i] = 0;
	}

	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		if (delRect) {
			cRect temp;
			temp.mid[0] = mx;
			temp.mid[1] = my;
			temp.sizeX = defaultSize;
			temp.sizeY = defaultSize;
			for (int i =0; i < 3; ++i)
				temp.RGB[i] = rand() % 256;
			if (left_button) temp.protect = true;
			else temp.protect = false;
			rects.push_back(temp);
			if (mSizeX > defaultSize * 2) {
				mSizeX -= defaultSize;
				mSizeY -= defaultSize;
			}
			delRect--;
		}
	}

	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		left_button = false;

		for (int i= rects.size() - 1; i>=0; --i) {
			if (rects[i].protect)
				rects[i].protect = false;
			else break;
		}
	}

	drawScene();
}

void Motion(int x, int y)
{
	if (left_button) {
			mx = x;
			my = y;
		for (int i =0; i < rects.size(); ++i) {
			if (mx - mSizeX <= rects[i].mid[0] + rects[i].sizeX && mx + mSizeX >= rects[i].mid[0] - rects[i].sizeX &&
				my - mSizeY <= rects[i].mid[1] + rects[i].sizeY && my + mSizeY >= rects[i].mid[1] - rects[i].sizeY &&
				rects[i].protect == false) {
				for (int j = 0; j < 3; ++j)
					mRGB[j] = rects[i].RGB[j];
				rects.erase(rects.begin() + i);
				mSizeX += defaultSize;
				mSizeY += defaultSize;
				delRect++;
			}
		}
	}


	drawScene();
}

void TimerFunction(int value)
{
	glutPostRedisplay();
	glutTimerFunc(100, TimerFunction, 1);
}
