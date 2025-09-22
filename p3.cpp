#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

#define WinX 800
#define WinY 600
#define maxRect 30

GLfloat c2GLx(int x) {
	return (2.0f * x) / WinX - 1.0f;
}

GLfloat c2GLy(int y) {
	return 1.0f - (2.0f * y) / WinY;
}

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
void Mouse(int button, int state, int x, int y);
void Keyboard(unsigned char key, int x, int y);
void Motion(int x, int y);

typedef struct _CRECT {
	int left;
	int bottom;
	int sizeX;
	int sizeY;
	GLfloat RGB[3];
	bool val;

} cRect;

bool left_click = false;
bool right_click = false;

int rectCnt = 0;
int rectIdx = 0;
int mx = 0;
int my = 0;

int dragOffsetX = 0;
int dragOffsetY = 0;

int sel = -1;

cRect rects[30];

GLfloat bRGB[3];

void main(int argc, char** argv)
{
	srand(time(NULL));


	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(WinX, WinY);
	glutCreateWindow("practice3");
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
	glutMainLoop();
}

GLvoid drawScene()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	for (int i = 0; i < rectIdx; ++i) {
		if (rects[i].val) {
			glColor3f(rects[i].RGB[0], rects[i].RGB[1], rects[i].RGB[2]);
			glRectf(c2GLx(rects[i].left), c2GLy(rects[i].bottom), c2GLx(rects[i].left + rects[i].sizeX), c2GLy(rects[i].bottom - rects[i].sizeY));
		}
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

	case 'a':
		if (rectCnt < 30) {
			int makeCnt = 10;
			if (rectCnt + 10 > maxRect) makeCnt = maxRect - rectCnt;

			while (makeCnt) {
				bool valCheck = false;
				for (int i = 0; i < rectIdx; ++i) {
					if (rects[i].val == false) {
						rects[i].left = rand() % (WinX - 100);
						rects[i].bottom = rand() % (WinY - 100);
						rects[i].sizeX = rand() % 81 + 20;
						rects[i].sizeY = rand() % 81 + 20;
						for (int j = 0; j < 3; ++j) {
							rects[i].RGB[j] = (float)rand() / (float)RAND_MAX;
						}
						rects[i].val = true;
						rectCnt++;
						valCheck = true;
						break;
					}
				}
				if (!valCheck) {
					rects[rectIdx].left = rand() % (WinX - 100);
					rects[rectIdx].bottom = rand() % (WinY - 100) + 100;
					rects[rectIdx].sizeX = rand() % 81 + 20;
					rects[rectIdx].sizeY = rand() % 81 + 20;
					for (int i = 0; i < 3; ++i) {
						rects[rectIdx].RGB[i] = (float)rand() / (float)RAND_MAX;
					}
					rects[rectIdx].val = true;
					rectCnt++;
					if (rectIdx < 29) rectIdx++;
				}
				makeCnt--;
			}
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
		for (int i = 0; i < rectIdx; ++i) {
			if (mx >= rects[i].left && mx <= rects[i].left + rects[i].sizeX &&
				my <= rects[i].bottom && my >= rects[i].bottom - rects[i].sizeY) {
				left_click = true;
				sel = i;
				dragOffsetX = mx - rects[i].left;
				dragOffsetY = my - rects[i].bottom;
				break;
			}
		}
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		if (rectCnt < 30) {
			for (int i = 0; i < rectIdx; ++i) {
				if (mx >= rects[i].left && mx <= rects[i].left + rects[i].sizeX &&
					my <= rects[i].bottom && my >= rects[i].bottom - rects[i].sizeY) {
					bool valCheck = false;
					for (int j = 0; j < rectIdx; ++j) {
						if (rects[j].val == false) {
							rects[i].sizeX /= 2;
							rects[i].sizeY /= 2;

							rects[j].left = rects[i].left + 20;
							rects[j].bottom = rects[i].bottom + 20;
							rects[j].sizeX = rects[i].sizeX - rand() % 21;
							rects[j].sizeY = rects[i].sizeY - rand() % 21;

							for (int k = 0; k < 3; ++k) {
								rects[j].RGB[k] = (float)rand() / (float)RAND_MAX;
							}
							rects[j].val = true;
							rectCnt++;
							valCheck = true;
							break;
						}
					}
					if (!valCheck) {
						rects[i].sizeX /= 2;
						rects[i].sizeY /= 2;

						rects[rectIdx].left = rects[i].left + 20;
						rects[rectIdx].bottom = rects[i].bottom + 20;
						rects[rectIdx].sizeX = rects[i].sizeX - rand() % 21;
						rects[rectIdx].sizeY = rects[i].sizeY - rand() % 21;
						for (int j = 0; j < 3; ++j) {
							rects[rectIdx].RGB[j] = (float)rand() / (float)RAND_MAX;
						}
						rects[rectIdx].val = true;
						rectCnt++;
						if (rectIdx < 29) rectIdx++;
					}
					break;
				}
			}
		}
	}

	drawScene();
}

void Motion(int x, int y)
{
	mx = x;
	my = y;

	if (left_click && sel != -1) {
		rects[sel].left = x - dragOffsetX;
		rects[sel].bottom = y - dragOffsetY;
		for (int i = 0; i < rectIdx; ++i) {
			if (i != sel && rects[i].val) {
				if (rects[sel].left < rects[i].left + rects[i].sizeX &&
					rects[sel].left + rects[sel].sizeX > rects[i].left &&
					rects[sel].bottom > rects[i].bottom - rects[i].sizeY &&
					rects[sel].bottom - rects[sel].sizeY < rects[i].bottom) {
					if (rects[sel].left > rects[i].left) {
						rects[sel].sizeX += rects[sel].left - rects[i].left;
						rects[sel].left = rects[i].left;
					}
					else {
						if (rects[sel].left + rects[sel].sizeX < rects[i].left + rects[i].sizeX) {
							rects[sel].sizeX += (rects[i].left + rects[i].sizeX) - (rects[sel].left + rects[sel].sizeX);
						}
					}

					if (rects[sel].bottom > rects[i].bottom) {
						if (rects[sel].bottom - rects[sel].sizeY > rects[i].bottom - rects[i].sizeY) {
							rects[sel].sizeY += (rects[sel].bottom - rects[sel].sizeY) - (rects[i].bottom - rects[i].sizeY);
						}
					}
					else {
						if (rects[sel].bottom - rects[sel].sizeY > rects[i].bottom - rects[i].sizeY) {
							rects[sel].sizeY = rects[i].sizeY;
						}
						else {
							rects[sel].sizeY += rects[i].bottom - rects[sel].bottom;
						}
						rects[sel].bottom = rects[i].bottom;
					}

					for (int i = 0; i < 3; ++i) {
						rects[sel].RGB[i] = (float)rand() / (float)RAND_MAX;
					}
					rects[i].val = false;
					rectCnt--;
					sel = -1;
					break;
				}
			}
		}
	}


	drawScene();
}
