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
	int mid[2];
	int sizeX;
	int sizeY;
	int RGB[3];
	bool rgbMode;
	int mode;
	int sizeMode;
} cRect;

cRect rects[5];
int oriCord[5][2];
int oriSize[5][2];

int rectIdx = 0;
int mx = 0;
int my = 0;

int sel = -1;
int cur = -1;
int timer = 0;

bool toggles[5] = { false, false, false, false, false };

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
		glColor3f(c2GLrgb(rects[i].RGB[0]), c2GLrgb(rects[i].RGB[1]), c2GLrgb(rects[i].RGB[2]));
		glRectf(c2GLx(rects[i].mid[0] - rects[i].sizeX), c2GLy(rects[i].mid[1] + rects[i].sizeY),
			c2GLx(rects[i].mid[0] + rects[i].sizeX), c2GLy(rects[i].mid[1] - rects[i].sizeY));
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

	case '1':
		if (toggles[0]) {
			toggles[0] = false;

			for (int i = 0; i < rectIdx; ++i) {
				if (rects[i].mode == STOP) {
					toggles[0] = true;
					break;
				}
			}

			if (toggles[0]) {
				for (int i = 0; i < rectIdx; ++i)
					if (rects[i].mode == STOP)
						rects[i].mode = LU + rand() % 4;
			}
			else
				for (int i = 0; i < rectIdx; ++i)
					rects[i].mode = STOP;
		}
		else {
			if (toggles[1]) {
				toggles[1] = false;
				for (int i = 0; i < rectIdx; ++i)
					rects[i].mode = STOP;
			}

			if (toggles[4]) {
				toggles[4] = false;
				sel = -1;
				cur = -1;
			}

			toggles[0] = true;

			for (int i = 0; i < rectIdx; ++i)
				if (rects[i].mode == STOP)
					rects[i].mode = LU + rand() % 4;
		}
		break;

	case '2':
		if (toggles[1]) {
			toggles[1] = false;

			for (int i = 0; i < rectIdx; ++i) {
				if (rects[i].mode == STOP) {
					toggles[1] = true;
					break;
				}
			}

			if (toggles[1]) {
				for (int i = 0; i < rectIdx; ++i)
					if (rects[i].mode == STOP)
						rects[i].mode = LEFT + rand() % 2;
			}
			else
				for (int i = 0; i < rectIdx; ++i)
					rects[i].mode = STOP;
		}
		else {
			if (toggles[0]) {
				toggles[0] = false;
				for (int i = 0; i < rectIdx; ++i)
					rects[i].mode = STOP;
			}

			if (toggles[4]) {
				toggles[4] = false;
				sel = -1;
				cur = -1;
			}

			toggles[1] = true;

			for (int i = 0; i < rectIdx; ++i)
				if (rects[i].mode == STOP)
					rects[i].mode = LEFT + rand() % 2;
		}
		break;

	case '3':
		if (toggles[2]) {
			toggles[2] = false;

			for (int i = 0; i < rectIdx; ++i) {
				if (rects[i].sizeMode == 0) {
					toggles[2] = true;
					break;
				}
			}

			if (toggles[2]) {
				for (int i = 0; i < rectIdx; ++i)
					if (rects[i].sizeMode == 0)
						rects[i].sizeMode = 1;
			}
			else
				for (int i = 0; i < rectIdx; ++i)
					rects[i].sizeMode = 0;
		}
		else {
			if (toggles[4]) {
				toggles[4] = false;
				sel = -1;
				cur = -1;
			}

			toggles[2] = true;

			for (int i = 0; i < rectIdx; ++i)
				if (rects[i].sizeMode == 0)
					rects[i].sizeMode = 1;
		}
		break;

	case '4':
		if (toggles[3]) {
			toggles[3] = false;

			for (int i = 0; i < rectIdx; ++i) {
				if (rects[i].rgbMode == false) {
					toggles[3] = true;
					break;
				}
			}

			if (toggles[3]) {
				for (int i = 0; i < rectIdx; ++i)
					if (rects[i].rgbMode == false)
						rects[i].rgbMode = true;
			}
			else
				for (int i = 0; i < rectIdx; ++i)
					rects[i].rgbMode = false;
		}
		else {
			if (toggles[4]) {
				toggles[4] = false;
				sel = -1;
				cur = -1;
			}

			toggles[3] = true;

			for (int i = 0; i < rectIdx; ++i)
				if (rects[i].rgbMode == false)
					rects[i].rgbMode = true;
		}
		break;

	case '5':
		if (toggles[4]) {
			toggles[4] = false;

			for (int i = 0; i < rectIdx; ++i) {
				rects[i].mode = STOP;
				rects[i].sizeMode = 0;
				rects[i].rgbMode = false;
			}
		}
		else {
			toggles[4] = true;
			sel = rand() % rectIdx;
			cur = sel + 1;
			if (cur >= rectIdx)
				cur = 0;

			std::cout << "Selected : " << sel << std::endl;
		}
		break;

	case 's':
		for (int i = 0; i < 5; ++i)
			toggles[i] = false;

		for (int i = 0; i < rectIdx; ++i) {
			rects[i].mode = STOP;
			rects[i].sizeMode = 0;
			rects[i].rgbMode = false;
		}
		break;

	case 'm':
		for (int i = 0; i < 5; ++i)
			toggles[i] = false;

		for (int i = 0; i < rectIdx; ++i) {
			rects[i].mid[0] = oriCord[i][0];
			rects[i].mid[1] = oriCord[i][1];
			rects[i].sizeX = oriSize[i][0];
			rects[i].sizeY = oriSize[i][1];
			rects[i].mode = STOP;
			rects[i].sizeMode = 0;
			rects[i].rgbMode = false;
		}
		break;

	case 'r':

		for (int i = 0; i < 5; ++i)
			toggles[i] = false;

		for (int i = 0; i < rectIdx; ++i) {
			rects[i].mode = STOP;
			rects[i].sizeMode = 0;
			rects[i].rgbMode = false;
		}

		rectIdx = 0;
		break;
	}

	drawScene();
}

void Mouse(int button, int state, int x, int y)
{
	mx = x;
	my = y;

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (rectIdx < maxRect) {
			rects[rectIdx].mid[0] = x;
			rects[rectIdx].mid[1] = y;
			rects[rectIdx].sizeX = 30;
			rects[rectIdx].sizeY = 30;
			for (int i = 0; i < 3; ++i)
				rects[rectIdx].RGB[i] = rand() % 256;
			rects[rectIdx].rgbMode = false;
			rects[rectIdx].mode = STOP;
			rects[rectIdx].sizeMode = 0;
			oriCord[rectIdx][0] = rects[rectIdx].mid[0];
			oriCord[rectIdx][1] = rects[rectIdx].mid[1];
			oriSize[rectIdx][0] = rects[rectIdx].sizeX;
			oriSize[rectIdx][1] = rects[rectIdx].sizeY;
			++rectIdx;
		}
	}

	drawScene();
}

void Motion(int x, int y)
{

}

void TimerFunction(int value)
{
	timer++;
	if (timer >= 5) {
		timer = 0;
		for (int i = 0; i < rectIdx; ++i) {
			if (rects[i].rgbMode) {
				for (int j = 0; j < 3; ++j)
					rects[i].RGB[j] = rand() % 256;
			}
		}

		if (sel != -1) {
			std::cout << "Selected : " << sel << ", Current : " << cur << std::endl;
			rects[cur].rgbMode = rects[sel].rgbMode;
			rects[cur].mode = rects[sel].mode;
			rects[cur].sizeMode = rects[sel].sizeMode;

			cur++;

			if (cur >= rectIdx)
				cur = 0;

			if (cur == sel) {
				sel = -1;
				cur = -1;

				std::cout << "End" << std::endl;
			}
		}
	}

	for (int i = 0; i < rectIdx; ++i) {
		switch (rects[i].mode) {
			case LU:
				rects[i].mid[0] -= 20;
				rects[i].mid[1] -= 20;
				if (rects[i].mid[0] - rects[i].sizeX <= 0)
					rects[i].mode = RU;
				else if (rects[i].mid[1] - rects[i].sizeY <= 0)
					rects[i].mode = LD;
				break;
			case LD:
				rects[i].mid[0] -= 20;
				rects[i].mid[1] += 20;
				if (rects[i].mid[0] - rects[i].sizeX <= 0)
					rects[i].mode = RD;
				else if (rects[i].mid[1] + rects[i].sizeY >= WinY)
					rects[i].mode = LU;
				break;
			case RU:
				rects[i].mid[0] += 20;
				rects[i].mid[1] -= 20;
				if (rects[i].mid[0] + rects[i].sizeX >= WinX)
					rects[i].mode = LU;
				else if (rects[i].mid[1] - rects[i].sizeY <= 0)
					rects[i].mode = RD;
				break;
			case RD:
				rects[i].mid[0] += 20;
				rects[i].mid[1] += 20;
				if (rects[i].mid[0] + rects[i].sizeX >= WinX)
					rects[i].mode = LD;
				else if (rects[i].mid[1] + rects[i].sizeY >= WinY)
					rects[i].mode = RU;
				break;
			case LEFT:
				rects[i].mid[0] -= 20;
				if (rects[i].mid[0] - rects[i].sizeX <= 0) {
					rects[i].mid[1] += 40;
					if(rects[i].mid[1] + rects[i].sizeY >= WinY)
						rects[i].mid[1] = 0 + rects[i].sizeY;
					rects[i].mode = RIGHT;
				}
				break;
			case RIGHT:
				rects[i].mid[0] += 20;
				if (rects[i].mid[0] + rects[i].sizeX >= WinX) {
					rects[i].mid[1] += 40;
					if (rects[i].mid[1] + rects[i].sizeY >= WinY)
						rects[i].mid[1] = 0 + rects[i].sizeY;
					rects[i].mode = LEFT;
				}
				break;
		}

		if (rects[i].sizeMode == -1) {
			rects[i].sizeX -= 5;
			rects[i].sizeY -= 5;
			if (rects[i].sizeX <= 10)
				rects[i].sizeMode = 1;
		}
		else if (rects[i].sizeMode == 1) {
			rects[i].sizeX += 5;
			rects[i].sizeY += 5;
			if (rects[i].sizeX >= 50)
				rects[i].sizeMode = -1;
		}
	}
	glutPostRedisplay();
	glutTimerFunc(100, TimerFunction, 1);
}