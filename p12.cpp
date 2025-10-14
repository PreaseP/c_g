#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <vector>
#include <cmath>

#define WinX 800
#define WinY 600
#define Ratio 0.5f

enum ShapeType
{
	S_LINE = 2,
	S_TRI,
	S_REC,
	S_PEN
};

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
	GLfloat vPos[15];
	GLuint VAO, VBO, EBO;
	int shapeType;
	int moveCnt;
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

//--- 필요한변수선언
GLint width, height;
GLuint shaderProgramID; //--- 세이더 프로그램 이름
GLuint vertexShader;

//--- 버텍스세이더객체
GLuint fragmentShader; //--- 프래그먼트 세이더객체

GLuint VAO;

Shape shapes[5];

GLvoid InitBuffer();
GLvoid InitBuffer(Shape& s);
GLvoid UpdateShape(Shape& s);
GLvoid SetShapes(Shape s[]);

GLvoid SetLINE(Shape& s);
GLvoid SetTRI(Shape& s);
GLvoid SetREC(Shape& s);
GLvoid SetPEN(Shape& s);

GLvoid MoveLINE(Shape& s);
GLvoid MoveTRI(Shape& s);
GLvoid MoveREC(Shape& s);
GLvoid MovePEN(Shape& s);

bool a_toggle = true;

float vPositionList[] = {
	-1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, -1.0f, 0.0f
};

float linePos[] = {
	0.25f, 0.25f, 0.0f,
	-0.25f, -0.25f, 0.0f
};

float triPos[] = {
	0.0f, 0.25f, 0.0f,
	-0.25f, -0.25f, 0.0f,
	0.25f, -0.25f, 0.0f
};

float recPos[] = {
	-0.25f, 0.25f, 0.0f,
	-0.25f, -0.25f, 0.0f,
	0.25f, -0.25f, 0.0f,
	0.25f, 0.25f, 0.0f
};

float penPos[] = {
	0.0f, 0.25f, 0.0f,
	-0.25f, 0.05f, 0.0f,
	-0.15f, -0.25f, 0.0f,
	0.15f, -0.25f, 0.0f,
	0.25f, 0.05f, 0.0f
};

unsigned int index[] = {
	0, 1, 2, 3
};

GLuint vIndex[] = {
	0, 1, 2,
	0, 2, 3,
	0, 3, 4
};

GLchar* vertexSource, * fragmentSource;

//--- 메인 함수
void main(int argc, char** argv)
{
	//--- 윈도우생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Example1");

	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	glewInit();

	//--- 세이더읽어와서세이더프로그램만들기: 사용자정의함수호출
	make_shaderProgram();
	InitBuffer();
	SetShapes(shapes);

	//--- 세이더프로그램만들기
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(Mouse);
	glutTimerFunc(64, TimerFunction, 1);
	glutMainLoop();
}

void Keyboard(unsigned char key, int x, int y)
{
	if(key == 'q') exit(0);
	else if (key == 'a') {
		if (!a_toggle) {
			a_toggle = true;
		}
		else a_toggle = false; 
	}
	else if (key == 'l') {
		if (!a_toggle && shapes[4].shapeType == S_LINE && shapes[4].moveCnt == 0) {
			for (int i =0; i< 6; ++i)
				shapes[4].vPos[i] = linePos[i];

			SetTRI(shapes[4]);
		}
	}
	else if (key == 't') {
		if (!a_toggle && shapes[4].shapeType == S_TRI && shapes[4].moveCnt == 0) {
			for (int i = 0; i < 9; ++i)
				shapes[4].vPos[i] = triPos[i];

			SetREC(shapes[4]);
		}
	}
	else if (key == 'r') {
		if (!a_toggle && shapes[4].shapeType == S_REC && shapes[4].moveCnt == 0) {
			for (int i = 0; i < 12; ++i)
				shapes[4].vPos[i] = recPos[i];

			SetPEN(shapes[4]);
		}
	}
	else if (key == 'p') {
		if (!a_toggle && shapes[4].shapeType == S_PEN && shapes[4].moveCnt == 0) {
			for (int i = 0; i < 15; ++i)
				shapes[4].vPos[i] = penPos[i];

			SetLINE(shapes[4]);
		}
	}
	drawScene();
}

void Mouse(int button, int state, int x, int y) {
	
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
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}

void make_fragmentShaders()
{
	fragmentSource = filetobuf("fragment.glsl");    // 프래그세이더 읽어오기
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);
	glCompileShader(fragmentShader);

	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: frag_shader 컴파일 실패\n" << errorLog << std::endl;
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
	//--- 만들어진세이더프로그램사용하기
   //--- 여러 개의세이더프로그램만들수있고, 그중한개의프로그램을사용하려면
   //--- glUseProgram 함수를 호출하여사용할특정프로그램을지정한다.
	//--- 사용하기직전에호출할수있다.
}

GLvoid drawScene()
{
	//--- 콜백 함수: 그리기콜백함수

	glClearColor(1.0, 1.0, 1.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	int vColor = glGetUniformLocation(shaderProgramID, "vColor");

	glUseProgram(shaderProgramID);

	glUniform3f(vColor, 0.0f, 1.0f, 1.0f);

	glLineWidth(3.0f);


	if (a_toggle) {
		for (int i = 0; i < 4; ++i) {
			glBindVertexArray(shapes[i].VAO);
			if (shapes[i].shapeType == S_LINE)
				glDrawElements(GL_LINES, shapes[i].shapeType, GL_UNSIGNED_INT, 0);
			else
				glDrawElements(GL_TRIANGLES, (3 * shapes[i].shapeType) - 6, GL_UNSIGNED_INT, 0);
		}
	}
	else {
		glBindVertexArray(shapes[4].VAO);
		if (shapes[4].shapeType == S_LINE)
			glDrawElements(GL_LINES, shapes[4].shapeType, GL_UNSIGNED_INT, 0);
		else
			glDrawElements(GL_TRIANGLES, (3 * shapes[4].shapeType) - 6, GL_UNSIGNED_INT, 0);
	}

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

GLvoid InitBuffer()
{
	GLuint VBO_pos, EBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO_pos);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_pos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vPositionList), vPositionList, GL_STATIC_DRAW);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);
}

GLvoid InitBuffer(Shape& s)
{
	glGenVertexArrays(1, &s.VAO);
	glGenBuffers(1, &s.VBO);
	glGenBuffers(1, &s.EBO);

	glBindVertexArray(s.VAO);

	glBindBuffer(GL_ARRAY_BUFFER, s.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(s.vPos), s.vPos, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s.EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vIndex), vIndex, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

}

GLvoid UpdateShape(Shape& s) {
	
	glBindBuffer(GL_ARRAY_BUFFER, s.VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(s.vPos), s.vPos);
}

void TimerFunction(int value)
{
	if (a_toggle) {
		for (int i = 0; i < 4; ++i) {
			switch (shapes[i].shapeType) {
			case S_LINE:
				if (!shapes[i].moveCnt)
					SetTRI(shapes[i]);
				break;

			case S_TRI:
				if (shapes[i].moveCnt)
					MoveTRI(shapes[i]);
				else
					SetREC(shapes[i]);
				break;

			case S_REC:
				if (shapes[i].moveCnt)
					MoveREC(shapes[i]);
				else
					SetPEN(shapes[i]);
				break;

			case S_PEN:
				if (shapes[i].moveCnt > 10)
					MoveLINE(shapes[i]);
				else if (shapes[i].moveCnt)
					MovePEN(shapes[i]);
				else
					SetLINE(shapes[i]);
				break;
			}
		}
	}
	else {
		switch (shapes[4].shapeType) {
		case S_TRI:
			if (shapes[4].moveCnt)
				MoveTRI(shapes[4]);
			break;
		case S_REC:
			if (shapes[4].moveCnt)
				MoveREC(shapes[4]);
			break;
		case S_PEN:
			if (shapes[4].moveCnt > 10)
				MoveLINE(shapes[4]);
			else if (shapes[4].moveCnt)
				MovePEN(shapes[4]);
			break;
		}
	}

	glutPostRedisplay();
	glutTimerFunc(64, TimerFunction, 1);
}

GLvoid SetShapes(Shape s[]) 
{
	s[0].shapeType = S_LINE;
	for (int i = 0; i < 6; ++i) {
		s[0].vPos[i] = linePos[i];
		if (i % 3 == 0)
			s[0].vPos[i] -= 0.5f;
		else if (i % 3 == 1)
			s[0].vPos[i] += 0.5f;
	}

	s[1].shapeType = S_TRI;
	for (int i = 0; i < 9; ++i) {
		s[1].vPos[i] = triPos[i];
		if (i % 3 == 0)
			s[1].vPos[i] += 0.5f;
		else if (i % 3 == 1)
			s[1].vPos[i] += 0.5f;
	}

	s[2].shapeType = S_REC;
	for (int i = 0; i < 12; ++i) {
		s[2].vPos[i] = recPos[i];
		if (i % 3 == 0)
			s[2].vPos[i] -= 0.5f;
		else if (i % 3 == 1)
			s[2].vPos[i] -= 0.5f;
	}

	s[3].shapeType = S_PEN;
	for (int i = 0; i < 15; ++i) {
		s[3].vPos[i] = penPos[i];
		if (i % 3 == 0)
			s[3].vPos[i] += 0.5f;
		else if (i % 3 == 1)
			s[3].vPos[i] -= 0.5f;
	}

	s[4].shapeType = S_LINE;
	for (int i = 0; i < 6; ++i)
		s[4].vPos[i] = linePos[i];

	for (int i = 0; i < 6; ++i) {
		s[i].moveCnt = 0;
		InitBuffer(s[i]);
	}
}

GLvoid SetLINE(Shape& s) 
{
	s.moveCnt = 20;
}
GLvoid SetTRI(Shape& s)
{
	s.shapeType = S_TRI;
	s.moveCnt = 10;

	for (int i =0; i< 3; ++i)
		s.vPos[i + 6] = s.vPos[i];

	UpdateShape(s);
}
GLvoid SetREC(Shape& s)
{
	s.shapeType = S_REC;
	s.moveCnt = 10;

	for (int i = 0; i < 3; ++i)
		s.vPos[i + 9] = s.vPos[i];

	UpdateShape(s);
}
GLvoid SetPEN(Shape& s)
{
	s.shapeType = S_PEN;
	s.moveCnt = 10;

	for (int i = 0; i < 3; ++i)
		s.vPos[i + 12] = s.vPos[i + 9];

	UpdateShape(s);
}

GLvoid MoveLINE(Shape& s)
{
	s.vPos[0] += 0.025f;
	s.vPos[4] -= 0.03f;
	s.vPos[6] -= 0.01f;
	s.vPos[9] -= 0.04f;
	s.vPos[13] += 0.02f;
	--(s.moveCnt);
	if (s.moveCnt == 10) {
		s.shapeType = S_LINE;
		s.moveCnt = 0;
	}

	UpdateShape(s);
}
GLvoid MoveTRI(Shape& s)
{
	s.vPos[0] -= 0.025f;
	s.vPos[7] -= 0.05f;
	--(s.moveCnt);

	UpdateShape(s);
}
GLvoid MoveREC(Shape& s)
{
	s.vPos[0] -= 0.025f;
	s.vPos[9] += 0.025f;
	--(s.moveCnt);

	UpdateShape(s);
}
GLvoid MovePEN(Shape& s)
{
	s.vPos[0] += 0.025f;
	s.vPos[4] += 0.03f;
	s.vPos[6] -= 0.04f;
	s.vPos[9] -= 0.01f;
	s.vPos[10] -= 0.05f;
	s.vPos[13] -= 0.02f;
	--(s.moveCnt);

	UpdateShape(s);
}