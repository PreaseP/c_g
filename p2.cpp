#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
void Mouse(int button, int state, int x, int y);

typedef struct _CRECT {
	GLfloat l;
	GLfloat b;
	GLfloat r;
	GLfloat t;
	GLfloat RGB[3];
	int cnt;

} cRect;

cRect rects[4];

GLfloat bRGB[3];

void main(int argc, char** argv)
{
	srand(time(NULL));

	for (int i = 0; i < 4; ++i) {
		rects[i].l = (i % 2) ? -1.0f : 0.0f;
		rects[i].b = (i < 2) ? 0.0f : -1.0f;
		rects[i].r = (i % 2) ? 0.0f : 1.0f;
		rects[i].t = (i < 2) ? 1.0f : 0.0f;
		for (int j = 0; j < 3; ++j) {
			rects[i].RGB[j] = (float)rand() / (float)RAND_MAX;
			bRGB[j] = 1.0f;
		}

		rects[i].cnt = 0;
	}


	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(800, 600);
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
	glutMainLoop();
}

GLvoid drawScene()
{
	glClearColor(bRGB[0], bRGB[1], bRGB[2], 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	for (int i = 0; i < 4; ++i) {
		glColor3f(rects[i].RGB[0], rects[i].RGB[1], rects[i].RGB[2]);
		glRectf(rects[i].l, rects[i].b, rects[i].r, rects[i].t);
	}
	glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);

}

void Mouse(int button, int state, int x, int y) 
{
	float ox = (float)(x - (float)800 / 2.0) * (float)(1.0 / (float)(800 / 2.0));
	float oy = -(float)(y - (float)600 / 2.0) * (float)(1.0 / (float)(600 / 2.0));

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		std::cout << ox << " " << oy << std::endl;

		for (int i = 0; i < 4; ++i) {
			float l = (i % 2) ? -1.0f : 0.0f;
			float b = (i < 2) ? 0.0f : -1.0f;
			float r = (i % 2) ? 0.0f : 1.0f;
			float t = (i < 2) ? 1.0f : 0.0f;

			if (ox >= l && ox <= r && oy >= b && oy <= t) {
				if (ox >= rects[i].l && ox <= rects[i].r && oy >= rects[i].b && oy <= rects[i].t) {
					for (int j = 0; j < 3; ++j)
						rects[i].RGB[j] = (float)rand() / (float)RAND_MAX;
				}
				else {
					for (int j = 0; j < 3; ++j)
						bRGB[j] = (float)rand() / (float)RAND_MAX;
				}
			}
		}
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		std::cout << ox << " " << oy << std::endl;

		for (int i = 0; i < 4; ++i) {
			float l = (i % 2) ? -1.0f : 0.0f;
			float b = (i < 2) ? 0.0f : -1.0f;
			float r = (i % 2) ? 0.0f : 1.0f;
			float t = (i < 2) ? 1.0f : 0.0f;

			if (ox >= l && ox <= r && oy >= b && oy <= t) {
				if (ox >= rects[i].l && ox <= rects[i].r && oy >= rects[i].b && oy <= rects[i].t) {
					if (rects[i].cnt < 5) {
						rects[i].l += 0.05f;
						rects[i].b += 0.05f;
						rects[i].r -= 0.05f;
						rects[i].t -= 0.05f;
						rects[i].cnt += 1;
					}
				}
				else {
					if (rects[i].cnt > 0) {
						rects[i].l -= 0.05f;
						rects[i].b -= 0.05f;
						rects[i].r += 0.05f;
						rects[i].t += 0.05f;
						rects[i].cnt -= 1;
					}
				}
			}
		}
	}

	drawScene();
}
