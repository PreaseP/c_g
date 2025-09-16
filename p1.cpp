#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
void Keyboard(unsigned char key, int x, int y);
void SpecialKeyboard(int key, int x, int y);
void TimerFunction(int value);

bool timerToggle = false;

void main(int argc, char** argv)
{
	srand(time(NULL));
	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(800, 600);
	glutCreateWindow("practice1");
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
	glutKeyboardFunc(Keyboard);
	glutTimerFunc(1000, TimerFunction, 1);
	glutMainLoop();
}

GLvoid drawScene()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	// 그리기 부분 구현: 그리기 관련 부분이 여기에포함된다.
	glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	
}

void TimerFunction(int value) 
{
	if (timerToggle) {
		glClearColor((float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glutSwapBuffers();
		glutTimerFunc(1000, TimerFunction, 1);
	}
}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'q':
		std::cout << "q" << std::endl;
		exit(0);
	case 'c':
		std::cout << "c" << std::endl;
		glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glutSwapBuffers();
		break;
	case 'm':
		std::cout << "m" << std::endl;
		glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glutSwapBuffers();
		break;
	case 'y':
		std::cout << "y" << std::endl;
		glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glutSwapBuffers();
		break;
	case 'a':
		std::cout << "a" << std::endl;
		glClearColor((float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glutSwapBuffers();
		break;
	case 'w':
		std::cout << "w" << std::endl;
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glutSwapBuffers();
		break;
	case 'k':
		std::cout << "k" << std::endl;
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glutSwapBuffers();
		break;
	case 't':
		std::cout << "t" << std::endl;
		timerToggle = true;
		glutTimerFunc(1000, TimerFunction, 1);
		break;
	case 's':
		std::cout << "s" << std::endl;
		timerToggle = false;
		break;
	}
}

void SpecialKeyboard(int key, int x, int y)
{
	
}

