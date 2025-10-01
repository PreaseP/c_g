#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <vector>

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
	std::vector<GLfloat> vPosition;
	std::vector<GLuint> vIndex;
	GLfloat color[3];
	GLuint VAO, VBO, EBO;
} Shape;

void Mouse(int button, int state, int x, int y);
void Keyboard(unsigned char key, int x, int y);
// void Motion(int x, int y);

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

Shape shapes[4][5];

int rightIdx[4] = { 1, 1, 1, 1 };
int maxShape[4] = { 1, 1, 1, 1 };
int mode = GL_TRIANGLES;

GLvoid InitBuffer();
GLvoid InitBuffer(Shape& s);
GLvoid SetShape(int pos, int idx, int x, int y);

bool init = true;

float vPositionList[] = {
	-1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, -1.0f, 0.0f
};

unsigned int index[] = {
	0, 1, 2, 3
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

	SetShape(0, 0, WinX / 4, WinY / 4);
	SetShape(1, 0, WinX * 3 / 4, WinY / 4);
	SetShape(2, 0, WinX / 4, WinY * 3 / 4);
	SetShape(3, 0, WinX * 3 / 4, WinY * 3 / 4);
	init = false;

	//--- 세이더프로그램만들기
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(Mouse);
	glutMainLoop();
}

void Keyboard(unsigned char key, int x, int y)
{
	GLfloat mx = c2GLx(x);
	GLfloat my = c2GLy(y);

	if (key == 'q') exit(0);
	else if (key == 'a') mode = GL_TRIANGLES;
	else if (key == 'b') mode = GL_LINE_LOOP;
	else if (key == 'c') {
		init = true;
		SetShape(0, 0, WinX / 4, WinY / 4);
		SetShape(1, 0, WinX * 3 / 4, WinY / 4);
		SetShape(2, 0, WinX / 4, WinY * 3 / 4);
		SetShape(3, 0, WinX * 3 / 4, WinY * 3 / 4);
		init = false;
		rightIdx[0] = rightIdx[1] = rightIdx[2] = rightIdx[3] = 1;
		maxShape[0] = maxShape[1] = maxShape[2] = maxShape[3] = 1;
	}

	drawScene();
}

void Mouse(int button, int state, int x, int y) {
	int pos = 0;
	if (x > WinX / 2) pos += 1;
	if (y > WinY / 2) pos += 2;

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		// std::cout << "pos: " << pos << ", x: " << x << ", y: " << y << std::endl;
		SetShape(pos, 0, x, y);
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		SetShape(pos, rightIdx[pos], x, y);
		rightIdx[pos]++;
		if (rightIdx[pos] > 4) rightIdx[pos] = 1;
		if (maxShape[pos] < 5) maxShape[pos]++;
	}
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

	glBindVertexArray(VAO);
	glDrawElements(GL_LINES, 4, GL_UNSIGNED_INT, 0);

	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < maxShape[i]; ++j) {
			glUniform3f(vColor, shapes[i][j].color[0], shapes[i][j].color[1], shapes[i][j].color[2]);
			glBindVertexArray(shapes[i][j].VAO);
			glDrawElements(mode, shapes[i][j].vIndex.size(), GL_UNSIGNED_INT, 0);
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * s.vPosition.size(), s.vPosition.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s.EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * s.vIndex.size(), s.vIndex.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

}

GLvoid SetShape(int pos, int idx, int x, int y)
{
	shapes[pos][idx].vPosition.clear();
	shapes[pos][idx].vIndex.clear();

	for (int i = 0; i < 3; ++i)
		shapes[pos][idx].color[i] = c2GLrgb(rand() % 256);

	if (idx == 0) {
		int dx, dy;
		if (init) {
			dx = 30;
			dy = 30;
		}
		else {
			dx = rand() % 40 + 5;
			dy = rand() % 40 + 5;
		}
		shapes[pos][idx].vPosition.push_back(c2GLx(x - dx));
		shapes[pos][idx].vPosition.push_back(c2GLy(y + dy));
		shapes[pos][idx].vPosition.push_back(0.0f);
		shapes[pos][idx].vPosition.push_back(c2GLx(x));
		shapes[pos][idx].vPosition.push_back(c2GLy(y - dy));
		shapes[pos][idx].vPosition.push_back(0.0f);
		shapes[pos][idx].vPosition.push_back(c2GLx(x + dx));
		shapes[pos][idx].vPosition.push_back(c2GLy(y + dy));
		shapes[pos][idx].vPosition.push_back(0.0f);
		shapes[pos][idx].vIndex.push_back(0);
		shapes[pos][idx].vIndex.push_back(2);
		shapes[pos][idx].vIndex.push_back(1);
	}
	else {
		shapes[pos][idx].vPosition.push_back(c2GLx(x - 30));
		shapes[pos][idx].vPosition.push_back(c2GLy(y + 30));
		shapes[pos][idx].vPosition.push_back(0.0f);
		shapes[pos][idx].vPosition.push_back(c2GLx(x));
		shapes[pos][idx].vPosition.push_back(c2GLy(y - 20));
		shapes[pos][idx].vPosition.push_back(0.0f);
		shapes[pos][idx].vPosition.push_back(c2GLx(x + 30));
		shapes[pos][idx].vPosition.push_back(c2GLy(y + 30));
		shapes[pos][idx].vPosition.push_back(0.0f);
		shapes[pos][idx].vIndex.push_back(0);
		shapes[pos][idx].vIndex.push_back(1);
		shapes[pos][idx].vIndex.push_back(2);
	}

	InitBuffer(shapes[pos][idx]);
}