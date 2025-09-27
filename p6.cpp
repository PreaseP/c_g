#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

#define WinX 800
#define WinY 600

#define wasd 1
#define cross 2
#define same 3
#define all 4

#define STOP 0
#define UP 1
#define LEFT 2
#define RIGHT 3
#define DOWN 4

#define LU 11
#define RU 12
#define LD 13
#define RD 14

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
void setRect();


typedef struct _MRECT {
	int mid[2];
	int sizeX;
	int sizeY;
	int RGB[3];
	int mode;
} mRect;

typedef struct _CRECT {
	int mid[2];
	mRect rects4[4];
	mRect rects8[9];
	int sizeX;
	int sizeY;
	int RGB[3];
	int state;
	bool val;
} cRect;

cRect rects[10];

int maxRect = 0;
int mx = 0;
int my = 0;

void moveRect(mRect& rect);

void main(int argc, char** argv)
{
	srand(time(NULL));

	maxRect = rand() % 6 + 5;
	
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
	for (int i = 0; i < maxRect; ++i) {
		switch (rects[i].state) {
			case STOP:
				glColor3f(c2GLrgb(rects[i].RGB[0]), c2GLrgb(rects[i].RGB[1]), c2GLrgb(rects[i].RGB[2]));
				glRectf(c2GLx(rects[i].mid[0] - rects[i].sizeX), c2GLy(rects[i].mid[1] + rects[i].sizeY),
					c2GLx(rects[i].mid[0] + rects[i].sizeX), c2GLy(rects[i].mid[1] - rects[i].sizeY));
				break;
		
			case all:
				for (int j = 0; j < 9; ++j) {
					glColor3f(c2GLrgb(rects[i].RGB[0]), c2GLrgb(rects[i].RGB[1]), c2GLrgb(rects[i].RGB[2]));
					if (j != 4) {
						glRectf(c2GLx(rects[i].rects8[j].mid[0] - rects[i].rects8[j].sizeX), c2GLy(rects[i].rects8[j].mid[1] + rects[i].rects8[j].sizeY),
							c2GLx(rects[i].rects8[j].mid[0] + rects[i].rects8[j].sizeX), c2GLy(rects[i].rects8[j].mid[1] - rects[i].rects8[j].sizeY));
					}
				}
				break;

			default:
				for (int j = 0; j < 4; ++j) {
					glColor3f(c2GLrgb(rects[i].RGB[0]), c2GLrgb(rects[i].RGB[1]), c2GLrgb(rects[i].RGB[2]));
					glRectf(c2GLx(rects[i].rects4[j].mid[0] - rects[i].rects4[j].sizeX), c2GLy(rects[i].rects4[j].mid[1] + rects[i].rects4[j].sizeY),
						c2GLx(rects[i].rects4[j].mid[0] + rects[i].rects4[j].sizeX), c2GLy(rects[i].rects4[j].mid[1] - rects[i].rects4[j].sizeY));
				}
				break;

		}
	}
	glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);

}

void setRect()
{
	for (int i =0; i < maxRect; ++i) {
		rects[i].mid[0] = rand() % (WinX - 200) + 100;
		rects[i].mid[1] = rand() % (WinY - 200) + 100;
		rects[i].sizeX = rand() % 10 + 50;
		rects[i].sizeY = rand() % 10 + 50;
		rects[i].val = true;

		for (int j = 0; j < 3; ++j)
			rects[i].RGB[j] = rand() % 256;
		rects[i].state = STOP;
		for (int j = 0; j < 4; ++j) {
			rects[i].rects4[j].sizeX = rects[i].sizeX / 2;
			rects[i].rects4[j].sizeY = rects[i].sizeY / 2;
			switch (j % 2) {
			case 0:
				rects[i].rects4[j].mid[0] = rects[i].mid[0] - rects[i].rects4[j].sizeX;
				break;
			case 1:
				rects[i].rects4[j].mid[0] = rects[i].mid[0] + rects[i].rects4[j].sizeX;
				break;
			}

			switch (j / 2) {
			case 0:
				rects[i].rects4[j].mid[1] = rects[i].mid[1] - rects[i].rects4[j].sizeY;
				break;
			case 1:
				rects[i].rects4[j].mid[1] = rects[i].mid[1] + rects[i].rects4[j].sizeY;
				break;
			}

			for (int k = 0; k < 3; ++k)
				rects[i].rects4[j].RGB[k] = rects[i].RGB[k];
			rects[i].rects4[j].mode = STOP;
		}
		for (int j = 0; j < 9; ++j) {
			rects[i].rects8[j].sizeX = rects[i].sizeX / 3;
			rects[i].rects8[j].sizeY = rects[i].sizeY / 3;
			switch (j % 3) {
				case 0:
					rects[i].rects8[j].mid[0] = rects[i].mid[0] - (rects[i].rects8[j].sizeX * 2);
					break;
				case 1:
					rects[i].rects8[j].mid[0] = rects[i].mid[0];
					break;
				case 2:
					rects[i].rects8[j].mid[0] = rects[i].mid[0] + (rects[i].rects8[j].sizeX * 2);
					break;
			}

			switch (j / 3) {
			case 0:
				rects[i].rects8[j].mid[1] = rects[i].mid[1] - (rects[i].rects8[j].sizeY * 2);
				break;
			case 1:
				rects[i].rects8[j].mid[1] = rects[i].mid[1];
				break;
			case 2:
				rects[i].rects8[j].mid[1] = rects[i].mid[1] + (rects[i].rects8[j].sizeY * 2);
				break;
			}

			for (int k = 0; k < 3; ++k)
				rects[i].rects8[j].RGB[k] = rects[i].RGB[k];
			rects[i].rects8[j].mode = STOP;
		}
	}
}

void moveRect(mRect& rect) {
	switch (rect.mode) {
		case LU:
			rect.mid[0] -= 30;
			rect.mid[1] -= 30;
			break;
		case LD:
			rect.mid[0] -= 30;
			rect.mid[1] += 30;
			break;
		case RU:
			rect.mid[0] += 30;
			rect.mid[1] -= 30;
			break;
		case RD:
			rect.mid[0] += 30;
			rect.mid[1] += 30;
			break;
		case LEFT:
			rect.mid[0] -= 30;
			break;
		case RIGHT:
			rect.mid[0] += 30;
			break;
		case UP:
			rect.mid[1] -= 30;
			break;
		case DOWN:
			rect.mid[1] += 30;
			break;
	}
	
	if (rect.sizeX) {
		rect.sizeX -= 5;
		if (rect.sizeX < 0)
			rect.sizeX = 0;
	}
	if (rect.sizeY) {
		rect.sizeY -= 5;
		if (rect.sizeY < 0)
			rect.sizeY = 0;
	}

	if (rect.sizeX == 0 && rect.sizeY == 0) {
		rect.mode = STOP;
	}

}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'q':
		exit(0);
		break;
	case 'r':
		maxRect = rand() % 6 + 5;
		setRect();
		break;
	}

	drawScene();
}

void Mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		for (int i= 0; i < maxRect; ++i) {
			if (x >= rects[i].mid[0] - rects[i].sizeX && x <= rects[i].mid[0] + rects[i].sizeX &&
				y >= rects[i].mid[1] - rects[i].sizeY && y <= rects[i].mid[1] + rects[i].sizeY && rects[i].val) {
				rects[i].state = rand() % 4 + wasd;
				rects[i].val = false;

				switch(rects[i].state) {
					case wasd:
						rects[i].rects4[0].mode = LEFT;
						rects[i].rects4[1].mode = UP;
						rects[i].rects4[2].mode = DOWN;
						rects[i].rects4[3].mode = RIGHT;
						break;
					case cross:
						rects[i].rects4[0].mode = LU;
						rects[i].rects4[1].mode = RU;
						rects[i].rects4[2].mode = LD;
						rects[i].rects4[3].mode = RD;
						break;
					case same:
						rects[i].rects4[0].mode = UP + rand() % 4;
						for (int j = 1; j < 4; ++j)
							rects[i].rects4[j].mode = rects[i].rects4[0].mode;
						break;
					case all:
						rects[i].rects8[0].mode = LU;
						rects[i].rects8[1].mode = UP;
						rects[i].rects8[2].mode = RU;
						rects[i].rects8[3].mode = LEFT;
						rects[i].rects8[4].mode = STOP;
						rects[i].rects8[5].mode = RIGHT;
						rects[i].rects8[6].mode = LD;
						rects[i].rects8[7].mode = DOWN;
						rects[i].rects8[8].mode = RD;
						break;
				}

				break;
			}
		}
	}

	drawScene();
}

void Motion(int x, int y)
{

}

void TimerFunction(int value)
{
	for (int i = 0; i < maxRect; ++i) {
		if (rects[i].state == STOP)
			continue;
		switch (rects[i].state) {
			case all:
				for (int j = 0; j < 9; ++j)
					moveRect(rects[i].rects8[j]);
				break;

			default:
				for (int j = 0; j < 4; ++j)
					moveRect(rects[i].rects4[j]);
				break;
		}
	}
	glutPostRedisplay();
	glutTimerFunc(100, TimerFunction, 1);
}
