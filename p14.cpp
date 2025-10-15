#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <vector>
#include <cmath>

#define WinX 800
#define WinY 600

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
	GLfloat vPos[24];
	float rad[8];
	float angle[8];
	float mid[3];
	GLuint VAO, VBO, EBO;
	int dir;
	int curMid;
	float scale;
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

GLvoid InitBuffer();
GLvoid InitBuffer(Shape& s);
GLvoid UpdateShape(Shape& s);
GLvoid SetShapes(Shape s[]);

bool s_toggle = false;
bool d_toggle = false;
float scaleCnt = 0.0f;

float vPositionList[] = {
	-1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, -1.0f, 0.0f
};

float shape1Pos[] = {
	0.0f, 0.2f, 0.0f,
	-0.1f, 0.1f, 0.0f,
	0.1f, 0.1f, 0.0f,
	0.2f, 0.0f, 0.0f,
	0.1f, -0.1f, 0.0f,
	0.0f, -0.2f, 0.0f,
	-0.1f, -0.1f, 0.0f,
	-0.2f, 0.0f, 0.0f
};

float shape2Pos[] = {
	0.0f, 0.01f, 0.0f,
	0.1f, 0.1f, 0.0f,
	-0.1f, 0.1f, 0.0f,
	-0.01f, 0.0f, 0.0f,
	-0.1f, -0.1f, 0.0f,
	0.0f, -0.01f, 0.0f,
	0.1f, -0.1f, 0.0f,
	0.01f, 0.0f, 0.0f
};

Shape shapes[2];

unsigned int index[] = {
	0, 1, 2, 3
};

GLuint vIndex[] = {
	0, 1, 2,
	2, 4, 3,
	4, 6, 5,
	6, 1, 7
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
	for (int i = 0; i < 2; ++i) shapes[i].dir = 0;

	srand(time(NULL));

	//--- 세이더프로그램만들기
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(Mouse);
	glutTimerFunc(16, TimerFunction, 1);
	glutMainLoop();
}

void Keyboard(unsigned char key, int x, int y)
{
	if (key == 'q') exit(0);
	else if (key == 's') {
		s_toggle = !s_toggle;
	}
	else if (key == 'c') {
		for (int i = 0; i < 2; ++i) shapes[i].dir = -1;
	}
	else if (key == 't') {
		for (int i = 0; i < 2; ++i) shapes[i].dir = 1;
	}
	else if (key == 'd') {
		if (shapes[0].dir) d_toggle = !d_toggle;
		// if (d_toggle) scaleCnt = 0.0f;
	}
	else if (key == 'm') {
		float tmpScale[2];

		for (int i = 0; i < 2; ++i) tmpScale[i] = shapes[i].scale;
		SetShapes(shapes);

		for (int i = 0; i < 2; ++i) {
			shapes[i].scale = tmpScale[i];
			shapes[i].curMid = rand() % 8;
			shapes[i].mid[0] = shapes[i].vPos[shapes[i].curMid * 3];
			shapes[i].mid[1] = shapes[i].vPos[shapes[i].curMid * 3 + 1];

			for (int j = 0; j < 8; ++j) {
				shapes[i].angle[j] = atan2(shapes[i].vPos[j * 3 + 1] - shapes[i].mid[1], shapes[i].vPos[j * 3] - shapes[i].mid[0]);
				shapes[i].rad[j] = sqrt((shapes[i].vPos[j * 3] - shapes[i].mid[0]) * (shapes[i].vPos[j * 3] - shapes[i].mid[0]) +
					(shapes[i].vPos[j * 3 + 1] - shapes[i].mid[1]) * (shapes[i].vPos[j * 3 + 1] - shapes[i].mid[1]));
			}

			UpdateShape(shapes[i]);
		}
	}
	else if (key == 'r') {
		SetShapes(shapes);
		for (int i = 0; i < 2; ++i) shapes[i].dir = 0;
		s_toggle = d_toggle = false;
		scaleCnt = 0.0f;
	}

	glutPostRedisplay();
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

	glUniform3f(vColor, c2GLrgb(255), c2GLrgb(192), c2GLrgb(0));

	for (int i = 0; i < 2; ++i) {
		glBindVertexArray(shapes[i].VAO);
		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
	}

	glUniform3f(vColor, 0.0f, 1.0f, 1.0f);

	for (int i = 0; i < 2; ++i) {
		if (shapes[i].curMid >= 0) {
			glPointSize(5.0f);
			glBindVertexArray(shapes[i].VAO);
			glDrawArrays(GL_POINTS, shapes[i].curMid, 1);
		}
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
	if (!s_toggle) {
		if (d_toggle) scaleCnt += 0.05f;

		for (int i = 0; i < 2; ++i) {
			float curScale = shapes[i].scale;

			if (d_toggle) {
				curScale = 1.0f + 0.5f * sin(scaleCnt);
				shapes[i].scale = curScale;
			}

			for (int j = 0; j < 8; ++j) {
				if (j == shapes[i].curMid) continue;
				shapes[i].angle[j] += 0.09f * shapes[i].dir;
				shapes[i].vPos[j * 3] = curScale * shapes[i].rad[j] * cos(shapes[i].angle[j]) + shapes[i].mid[0];
				shapes[i].vPos[j * 3 + 1] = curScale * shapes[i].rad[j] * sin(shapes[i].angle[j]) + shapes[i].mid[1];
			}

			UpdateShape(shapes[i]);
		}
	}

	glutPostRedisplay();
	glutTimerFunc(16, TimerFunction, 1);
}

GLvoid SetShapes(Shape s[])
{
	for (int i = 0; i < 2; ++i) {
		s[i].mid[0] = -0.5f + (1.0f * i);
		s[i].mid[1] = 0.0f;
		s[i].mid[2] = 0.0f;
		if (s_toggle) s[i].dir = 0;
		s[i].curMid = -1;
		s[i].scale = 1.0f;

		for (int j = 0; j < 8; ++j) {
			if (i == 0) {
				s[i].vPos[j * 3] = shape1Pos[j * 3] - 0.5f;
				s[i].vPos[j * 3 + 1] = shape1Pos[j * 3 + 1];
				s[i].vPos[j * 3 + 2] = shape1Pos[j * 3 + 2];
			}
			else {
				s[i].vPos[j * 3] = shape2Pos[j * 3] + 0.5f;
				s[i].vPos[j * 3 + 1] = shape2Pos[j * 3 + 1];
				s[i].vPos[j * 3 + 2] = shape2Pos[j * 3 + 2];
			}
			s[i].angle[j] = atan2f(s[i].vPos[j * 3 + 1] - s[i].mid[1], s[i].vPos[j * 3] - s[i].mid[0]);
			s[i].rad[j] = sqrt((s[i].vPos[j * 3] - s[i].mid[0]) * (s[i].vPos[j * 3] - s[i].mid[0]) + (s[i].vPos[j * 3 + 1] - s[i].mid[1]) * (s[i].vPos[j * 3 + 1] - s[i].mid[1]));
		}

		InitBuffer(s[i]);
	}
}