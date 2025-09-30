#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <vector>
#include <cmath>

#define WinX 800
#define WinY 600

#define CCW 0
#define CW 1

char* filetobuf(const char* file)
{
	FILE* fptr;
	long length;
	char* buf;

	fptr = fopen(file, "rb");
	if (!fptr)
		return NULL;
	fseek(fptr, 0, SEEK_END);
	length = ftell(fptr);
	buf = (char*)malloc(length + 1);
	fseek(fptr, 0, SEEK_SET);
	fread(buf, length, 1, fptr);
	fclose(fptr);
	buf[length] = 0;
	return buf;
}

GLfloat c2GLx(int x) {
	return (2.0f * x) / WinX - 1.0f;
}

GLfloat c2GLy(int y) {
	return 1.0f - (2.0f * y) / WinY;
}

GLfloat c2GLrgb(int c) {
	return (GLfloat)(c / 255.0f);
}

typedef struct _SHAPE {
	int mid[2];
	std::vector<GLfloat> vPosition;
	std::vector<GLuint> vIndex;
	GLuint VAO, VBO, EBO;
	int spCnt;
	int maxSp;
	float radian;
	float r;
	bool drawing;
} Shape;

void Mouse(int button, int state, int x, int y);
void Keyboard(unsigned char key, int x, int y);
// void Motion(int x, int y);
void TimerFunction(int value);

void make_vertexShaders();
void make_fragmentShaders();
void make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);

//--- �ʿ��Ѻ�������
GLint width, height;
GLuint shaderProgramID; //--- ���̴� ���α׷� �̸�
GLuint vertexShader;

//--- ���ؽ����̴���ü
GLuint fragmentShader; //--- �����׸�Ʈ ���̴���ü

GLuint VAO;

Shape shapes[5];
int bRGB[3] = { 0,0,0 };
int shapeIdx = 0;
int mode = GL_POINTS;
int spMode = CCW;

GLvoid InitBuffer();
GLvoid InitBuffer(Shape& s);
GLvoid InitBufferA(Shape& s);
GLvoid SetShape(Shape& s, int x, int y);
GLvoid SetAll(int x, int y);

GLchar* vertexSource, * fragmentSource;

//--- ���� �Լ�
void main(int argc, char** argv)
{
	//--- ����������ϱ�
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Example1");

	//--- GLEW �ʱ�ȭ�ϱ�
	glewExperimental = GL_TRUE;
	glewInit();

	//--- ���̴��о�ͼ����̴����α׷������: ����������Լ�ȣ��
	make_shaderProgram();

	//--- ���̴����α׷������
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(Mouse);
	glutTimerFunc(16, TimerFunction, 1);
	glutMainLoop();
}

void Keyboard(unsigned char key, int x, int y)
{	
	GLfloat mx = c2GLx(x);
	GLfloat my = c2GLy(y);

	if (key == 'q') exit(0);
	else if (key == 'p') {
		
		mode = GL_POINTS;

		SetAll(x, y);
	}
	else if (key == 'l') {
		
		mode = GL_LINE_STRIP;
		SetAll(x, y);
	}
	else if (key >= '1' && key <= '5') {
		
		shapeIdx = key - '1';
		
		SetAll(x, y);
	}
	else if (key == 'c') {

		mode = GL_POINTS;
		spMode = CCW;
		shapeIdx = 0;

		SetAll(x, y);
	}

	glutPostRedisplay();
}

void Mouse(int button, int state, int x, int y) {

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		SetAll(x, y);
	}

	glutPostRedisplay();
}

void make_vertexShaders()
{
	vertexSource = filetobuf("vertex.glsl");
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);
	glCompileShader(vertexShader);
	
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if(!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader ������ ����\n" << errorLog << std::endl;
		return;
	}
}

void make_fragmentShaders()
{
	fragmentSource = filetobuf("fragment.glsl");    // �����׼��̴� �о����
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);
	glCompileShader(fragmentShader);
	
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: frag_shader ������ ����\n" << errorLog << std::endl;
		return;
	}
}

void make_shaderProgram()
{
	make_vertexShaders();
	make_fragmentShaders();

	shaderProgramID = glCreateProgram();

	glAttachShader(shaderProgramID, vertexShader);
	glAttachShader(shaderProgramID, fragmentShader);
	glLinkProgram(shaderProgramID);
	
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	
	glUseProgram(shaderProgramID);
	//--- ����������̴����α׷�����ϱ�
   //--- ���� ���Ǽ��̴����α׷�������ְ�, �����Ѱ������α׷�������Ϸ���
   //--- glUseProgram �Լ��� ȣ���Ͽ������Ư�����α׷��������Ѵ�.
	//--- ����ϱ�������ȣ���Ҽ��ִ�.
}

GLvoid drawScene()
{
	//--- �ݹ� �Լ�: �׸����ݹ��Լ�

	glClearColor(c2GLrgb(bRGB[0]), c2GLrgb(bRGB[1]), c2GLrgb(bRGB[2]), 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// int vColor = glGetUniformLocation(shaderProgramID, "vColor");

	glUseProgram(shaderProgramID);

	for (int i = 0; i < shapeIdx +1; ++i) {
		glBindVertexArray(shapes[i].VAO);
		glDrawArrays(mode, 0, shapes[i].vPosition.size() / 3);
	}

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

GLvoid InitBuffer(Shape& s) 
{
	glGenVertexArrays(1, &s.VAO);
	glGenBuffers(1, &s.VBO);
	glGenBuffers(1, &s.EBO);

	glBindVertexArray(s.VAO);

	glBindBuffer(GL_ARRAY_BUFFER, s.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * s.vPosition.size(), s.vPosition.data(), GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s.EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * s.vIndex.size(), s.vIndex.data(), GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

}

GLvoid InitBufferA(Shape& s) {
	glGenVertexArrays(1, &s.VAO);
	glGenBuffers(1, &s.VBO);

	glBindVertexArray(s.VAO);

	glBindBuffer(GL_ARRAY_BUFFER, s.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * s.vPosition.size(), s.vPosition.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);
}

void TimerFunction(int value)
{	
	for (int i =0; i < shapeIdx + 1; ++i) {
		switch (spMode) {
		case CW:
			if (shapes[i].drawing) {
				if (shapes[i].spCnt < shapes[i].maxSp) {
					shapes[i].radian += 0.1f;
					shapes[i].r += 0.2f;
					if (shapes[i].radian > 2 * 3.141592f) {
						shapes[i].radian -= 2 * 3.141592f;
						shapes[i].spCnt++;
					}
					shapes[i].vPosition.push_back(c2GLx(shapes[i].mid[0] + shapes[i].r * cosf(shapes[i].radian)));
					shapes[i].vPosition.push_back(c2GLy(shapes[i].mid[1] + shapes[i].r * sinf(shapes[i].radian)));
					shapes[i].vPosition.push_back(0.0f);
					InitBufferA(shapes[i]);

					if (shapes[i].spCnt == shapes[i].maxSp) {
						shapes[i].mid[0] += (int)((shapes[i].r * cosf(shapes[i].radian)) * 2);
						shapes[i].radian = 3.141592f;
					}
				}
				else {
					shapes[i].radian -= 0.1f;
					shapes[i].r -= 0.2f;
					if (shapes[i].radian < -1 * 3.141592f) {
						shapes[i].radian += 2 * 3.141592f;
						shapes[i].spCnt++;
					}
					shapes[i].vPosition.push_back(c2GLx(shapes[i].mid[0] + shapes[i].r * cosf(shapes[i].radian)));
					shapes[i].vPosition.push_back(c2GLy(shapes[i].mid[1] + shapes[i].r * sinf(shapes[i].radian)));
					shapes[i].vPosition.push_back(0.0f);
					InitBufferA(shapes[i]);

					if (shapes[i].spCnt == shapes[i].maxSp * 2) {
						shapes[i].drawing = false;
					}
				}
			}
			break;
		case CCW:
			if (shapes[i].drawing) {
				if (shapes[i].spCnt < shapes[i].maxSp) {
					shapes[i].radian -= 0.1f;
					shapes[i].r += 0.2f;
					if (shapes[i].radian < -2 * 3.141592f) {
						shapes[i].radian += 2 * 3.141592f;
						shapes[i].spCnt++;

					}
					shapes[i].vPosition.push_back(c2GLx(shapes[i].mid[0] + shapes[i].r * cosf(shapes[i].radian)));
					shapes[i].vPosition.push_back(c2GLy(shapes[i].mid[1] + shapes[i].r * sinf(shapes[i].radian)));
					shapes[i].vPosition.push_back(0.0f);
					InitBufferA(shapes[i]);

					if (shapes[i].spCnt == shapes[i].maxSp) {
						shapes[i].mid[0] += (int)((shapes[i].r * cosf(shapes[i].radian)) * 2);
						shapes[i].radian = 3.141592f;
					}
				}
				else {
					shapes[i].radian += 0.1f;
					shapes[i].r -= 0.2f;
					if (shapes[i].radian > 3 * 3.141592f) {
						shapes[i].radian -= 2 * 3.141592f;
						shapes[i].spCnt++;
					}
					shapes[i].vPosition.push_back(c2GLx(shapes[i].mid[0] + shapes[i].r * cosf(shapes[i].radian)));
					shapes[i].vPosition.push_back(c2GLy(shapes[i].mid[1] + shapes[i].r * sinf(shapes[i].radian)));
					shapes[i].vPosition.push_back(0.0f);
					InitBufferA(shapes[i]);

					if (shapes[i].spCnt == shapes[i].maxSp * 2) {
						shapes[i].drawing = false;
					}
				}
			}
			break;
		}
	}
	glutPostRedisplay();
	glutTimerFunc(16, TimerFunction, 1);
}

GLvoid SetShape(Shape& s, int x, int y) {
	s.vPosition.clear();
	s.mid[0] = x;
	s.mid[1] = y;
	s.radian = 0.0f;
	s.r = 0.2f;
	s.spCnt = 0;
	if (rand() % 2) s.maxSp = 2;
	else s.maxSp = 3;
	s.drawing = true;
	s.vPosition.push_back(c2GLx(s.mid[0] + s.r * cosf(s.radian)));
	s.vPosition.push_back(c2GLy(s.mid[1] + s.r * sinf(s.radian)));
	s.vPosition.push_back(0.0f);
	InitBufferA(s);
}

GLvoid SetAll(int x, int y) {
	spMode = rand() % 2;

	for (int i = 0; i < 3; ++i)
		bRGB[i] = rand() % 256;

	SetShape(shapes[0], x, y);

	if (shapeIdx > 0) {
		for (int i = 1; i < shapeIdx + 1; ++i) {
			SetShape(shapes[i], rand() % 700, rand() % 500);
		}
	}
}