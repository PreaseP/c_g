#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

#define WinX 1024
#define WinY 768
#define maxRect 10

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
	int right;
	int top;
	int RGB[3];
	bool val;

} cRect;

class bRect {
	public:
		int left;
		int bottom;
		int right;
		int top;
		int RGB[3];

		bRect () {
			left = 0;
			bottom = 0;
			right = 0;
			top = 0;
			RGB[0] = 0;
			RGB[1] = 0;
			RGB[2] = 0;
		}

		void setRect(int l, int b, int r, int t, int R, int G, int B) {
			this->left = l;
			this->bottom = b;
			this->right = r;
			this->top = t;
			this->RGB[0] = R;
			this->RGB[1] = G;
			this->RGB[2] = B;
		}

		void drawRect() {
			glColor3f(c2GLrgb(RGB[0]), c2GLrgb(RGB[1]), c2GLrgb(RGB[2]));
			glRectf(c2GLx(left), c2GLy(bottom), c2GLx(right), c2GLy(top));
		}
};

bRect check[10];
cRect mainRects[10];

void resetRect(bRect check[10]) {
	for (int i = 0; i < 10; ++i) {
		mainRects[i].left = rand() % 250 + 512;
		mainRects[i].bottom = rand() % 450 + 200;
		mainRects[i].right = mainRects[i].left + (check[i].right - check[i].left);
		mainRects[i].top = mainRects[i].bottom - (check[i].bottom - check[i].top);
		for (int j = 0; j < 3; ++j) {
			mainRects[i].RGB[j] = check[i].RGB[j];
		}
		mainRects[i].val = false;
	}
}
bool left_click = false;

int mx = 0;
int my = 0;

int dragOffsetX = 0;
int dragOffsetY = 0;

int sel = -1;
int moveSel = -1;

void main(int argc, char** argv)
{
	srand(time(NULL));

	check[0].setRect(10, 60, 80, 10, 137, 111, 172);
	check[1].setRect(80, 100, 110, 40, 59, 172, 194);
	check[2].setRect(110, 90, 210, 50, 137, 161, 30);
	check[3].setRect(40, 130, 80, 70, 64, 190, 57);
	check[4].setRect(140, 130, 190, 90, 73, 160, 238);
	check[5].setRect(40, 170, 210, 130, 235, 194, 208);
	check[6].setRect(80, 240, 110, 170, 141, 92, 251);
	check[7].setRect(40, 310, 110, 240, 127, 47, 208);
	check[8].setRect(110, 310, 230, 200, 105, 212, 134);
	check[9].setRect(40, 560, 230, 310, 127, 195, 28);

	resetRect(check);

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
	glutTimerFunc(16, TimerFunction, 1);
	glutMainLoop();
}

GLvoid drawScene()
{
	glClearColor(c2GLrgb(95), c2GLrgb(95), c2GLrgb(95), 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	for (int i = 0; i < maxRect; ++i) {
		check[i].drawRect();
	}

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	for (int i = maxRect - 1; i >= 0; --i) {
		glColor3f(c2GLrgb(mainRects[i].RGB[0]), c2GLrgb(mainRects[i].RGB[1]), c2GLrgb(mainRects[i].RGB[2]));
		glRectf(c2GLx(mainRects[i].left), c2GLy(mainRects[i].bottom), c2GLx(mainRects[i].right), c2GLy(mainRects[i].top));
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
		resetRect(check);
		sel = -1;
		left_click = false;
		break;
	case 'h':
		if (moveSel == -1) {
			for (int i = 0; i < maxRect; ++i) {
				if (mainRects[i].val == false) {
					moveSel = i;
					break;
				}
			}
		}
		break;
	}

	drawScene();
}

void Mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		for (int i = 0; i < maxRect; ++i) {
			if (x >= mainRects[i].left && x <= mainRects[i].right &&
				y <= mainRects[i].bottom && y >= mainRects[i].top && mainRects[i].val == false) {
				left_click = true;
				sel = i;
				dragOffsetX = x - mainRects[i].left;
				dragOffsetY = y - mainRects[i].bottom;
				break;
			}
		}
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		if(left_click && sel != -1) {
			if (mainRects[sel].left >= check[sel].left - 20 && mainRects[sel].right <= check[sel].right + 20
				&& mainRects[sel].bottom <= check[sel].bottom + 20 && mainRects[sel].top >= check[sel].top - 20) {
				mainRects[sel].left = check[sel].left;
				mainRects[sel].bottom = check[sel].bottom;
				mainRects[sel].right = check[sel].right;
				mainRects[sel].top = check[sel].top;
				mainRects[sel].val = true;
			}
			left_click = false;
			sel = -1;
		}
	}

	drawScene();
}

void Motion(int x, int y)
{

	if (left_click && sel != -1) {
		mainRects[sel].left = x - dragOffsetX;
		mainRects[sel].bottom = y - dragOffsetY;
		mainRects[sel].right = mainRects[sel].left + (check[sel].right - check[sel].left);
		mainRects[sel].top = mainRects[sel].bottom - (check[sel].bottom - check[sel].top);
	}

	drawScene();
}

void TimerFunction(int value)
{	
	if (moveSel >= 0) {
		float alpha = 0.15f;

		float newLeft = mainRects[moveSel].left + (check[moveSel].left - mainRects[moveSel].left) * alpha;
		float newBottom = mainRects[moveSel].bottom + (check[moveSel].bottom - mainRects[moveSel].bottom) * alpha;
		float newRight = mainRects[moveSel].right + (check[moveSel].right - mainRects[moveSel].right) * alpha;
		float newTop = mainRects[moveSel].top + (check[moveSel].top - mainRects[moveSel].top) * alpha;

		mainRects[moveSel].left = (int)(newLeft);
		mainRects[moveSel].bottom = (int)(newBottom);
		mainRects[moveSel].right = (int)(newRight);
		mainRects[moveSel].top = (int)(newTop);
		
		if (mainRects[moveSel].left >= check[moveSel].left - 5 && mainRects[moveSel].right <= check[moveSel].right + 5
			&& mainRects[moveSel].bottom <= check[moveSel].bottom + 5 && mainRects[moveSel].top >= check[moveSel].top - 5) {
			mainRects[moveSel].left = check[moveSel].left;
			mainRects[moveSel].bottom = check[moveSel].bottom;
			mainRects[moveSel].right = check[moveSel].right;
			mainRects[moveSel].top = check[moveSel].top;
			mainRects[moveSel].val = true;
			moveSel = -1;
		}
	}
	glutPostRedisplay();
	glutTimerFunc(16, TimerFunction, 1);
}
