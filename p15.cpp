#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
#include <vector>
#include <cmath>

#define WinX 1024
#define WinY 768

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

typedef struct LINE {
	GLfloat pos[6];
	GLfloat color[3];
	GLuint VAO, VBO, EBO;
} mLine;

typedef struct TRI {
	GLfloat pos[9];
	GLfloat color[3];
	GLuint VAO, VBO, EBO;
	bool val;
} mTri;

typedef struct REC {
	GLfloat pos[12];
	GLfloat color[3];
	GLuint VAO, VBO, EBO;
	bool val;
} mRec;

float vPositionList[] = {
	0.0f, 0.5f, 0.0f,
	-0.5f, -0.5f, 0.5f,
	0.5f, -0.5f, 0.5f,
	0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f
};

float linePos[3][6] = {
	{1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f},
	{0.0f, 1.0f, 0.0f,
	0.0f,  -1.0f, 0.0f},
	{0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, -1.0f}
};

float triPos[4][9] = {
	// Front
	{ 0.0f,  0.5f,  0.0f,   -0.5f, -0.5f,  -0.5f,    0.5f, -0.5f,  -0.5f },
	// Right
	{ 0.0f,  0.5f,  0.0f,    0.5f, -0.5f, -0.5f,    0.5f, -0.5f,  0.5f },
	// Back
	{ 0.0f,  0.5f,  0.0f,   0.5f, -0.5f, 0.5f,    -0.5f, -0.5f, 0.5f },
	// Left
	{ 0.0f,  0.5f,  0.0f,   -0.5f, -0.5f,  0.5f,   -0.5f, -0.5f, -0.5f }
};

float recPos[6][12] = {
	// Bottom
	{ -0.5f,-0.5f,-0.5f,   -0.5f,-0.5f, 0.5f,   0.5f,-0.5f, 0.5f,  0.5f,-0.5f, -0.5f },
	// Front
	{ -0.5f, 0.5f, -0.5f,  -0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f,  0.5f, 0.5f, -0.5f },
	// Right
	{  0.5f, 0.5f, -0.5f,   0.5f, -0.5f, -0.5f,   0.5f, -0.5f, 0.5f,   0.5f, 0.5f, 0.5f },
	// Back
	{ 0.5f, 0.5f,0.5f,   0.5f, -0.5f, 0.5f,   -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f, 0.5f },
	// Left
	{ -0.5f, 0.5f, 0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, -0.5f,  -0.5f, 0.5f, -0.5f },
	// Top
	{ -0.5f, 0.5f, 0.5f,   -0.5f, 0.5f, -0.5f,   0.5f, 0.5f,-0.5f,  0.5f, 0.5f, 0.5f }
};

GLuint VAO;

unsigned int index[] = {
	0, 1, 2,
	0, 2, 3,
	0, 3, 4,
	0, 4, 1,
	4, 2, 1,
	2, 4, 3
};

unsigned int sIndex[] = {
	0, 1, 2,
	0, 2, 3
};

mLine lines[3];
mTri tris[4];
mRec recs[6];

GLvoid InitBuffer();
GLvoid InitBufferAll();

bool p_toggle = false;
bool h_toggle = false;

float rotx = -25.0f;
float roty = -30.0f;
float rotz = 10.0f;

// -25, -30, 10

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

GLchar* vertexSource, * fragmentSource;

//--- ���� �Լ�
void main(int argc, char** argv)
{
	//--- ����������ϱ�
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(WinX, WinY);
	glutCreateWindow("Example1");

	//--- GLEW �ʱ�ȭ�ϱ�
	glewExperimental = GL_TRUE;
	glewInit();

	//--- ���̴��о�ͼ����̴����α׷������: ����������Լ�ȣ��
	make_shaderProgram();
	InitBuffer();
	InitBufferAll();
	srand(time(NULL));

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
	if (key == 'q') exit(0);
	else if (key == 'p') { 
		p_toggle = !p_toggle;
		recs[0].val = false;
	}
	else if (key == 'r') {
		for (int i = 0; i < 4; ++i)
			tris[i].val = true;

		for (int i = 0; i < 6; ++i)
			recs[i].val = true;
	}
	else if (key >= '1' && key <= '6') {
		for (int i = 0; i < 6; ++i)
			recs[i].val = false;
		recs[key - '1'].val = true;
	}
	else if (key == '7' || key == '8' || key == '9' || key == '0') {
		for (int i = 0; i < 4; ++i)
			tris[i].val = false;
		int idx = (key == '0') ? 3 : (key - '7'); // '7'->0, '8'->1, '9'->2, '0'->3
		tris[idx].val = true;
		recs[0].val = false;
	}
	else if (key == 'c') {
		for (int i = 0; i < 6; ++i)
			recs[i].val = false;
		int randIdx1 = rand() % 6;
		recs[randIdx1].val = true;
		int randIdx2;
		do {
			randIdx2 = rand() % 6;
		} while (randIdx1 == randIdx2);
		recs[randIdx2].val = true;
	}
	else if (key == 't') {
		for (int i = 0; i < 4; ++i)
			tris[i].val = false;
		recs[0].val = true;
		tris[rand() % 4].val = true;
	}
	else if (key == 'h') {
		h_toggle = !h_toggle;
	}

	std::cout << key << std::endl;
	std::cout << "rotx: " << rotx << ", roty: " << roty << ", rotz: " << rotz << std::endl;
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
	//
	glUseProgram(shaderProgramID);

	if (h_toggle) {
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
	}
	else {
		glDisable(GL_CULL_FACE);
	}

	glEnable(GL_DEPTH_TEST);

	glClearColor(1.0, 1.0, 1.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 Tra = glm::mat4(1.0f);
	glm::mat4 Rotx = glm::mat4(1.0f);
	glm::mat4 Roty = glm::mat4(1.0f);
	glm::mat4 Rotz = glm::mat4(1.0f);
	glm::mat4 Sca = glm::mat4(1.0f);
	glm::mat4 res = glm::mat4(1.0f);

	Sca = glm::scale(Sca, glm::vec3(0.5f, 0.5f, 0.5f));
	Tra = glm::translate(Tra, glm::vec3(0.0f, 0.0f, 0.0f));
	Rotx = glm::rotate(Rotx, glm::radians(rotx), glm::vec3(1.0f, 0.0f, 0.0f));
	Roty = glm::rotate(Roty, glm::radians(roty), glm::vec3(0.0f, 1.0f, 0.0f));
	Rotz = glm::rotate(Rotz, glm::radians(rotz), glm::vec3(0.0f, 0.0f, 1.0f));

	res = Tra * Rotz * Roty * Rotx;
	// res = Rot * Tra * Sca;

	unsigned int modelLoc = glGetUniformLocation(shaderProgramID, "matrix");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(res));

	int vColor = glGetUniformLocation(shaderProgramID, "vColor");

	for (int i =0; i < 3; ++i) {
		glUniform3f(vColor, lines[i].color[0], lines[i].color[1], lines[i].color[2]);
		glBindVertexArray(lines[i].VAO);
		glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);
	}
	
	if (p_toggle) {
		for (int i =0; i <4; ++i) {
			if (tris[i].val) {
				glUniform3f(vColor, tris[i].color[0], tris[i].color[1], tris[i].color[2]);
				glBindVertexArray(tris[i].VAO);
				glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
			}
		}
		if (recs[0].val == true) {
			glUniform3f(vColor, recs[0].color[0], recs[0].color[1], recs[0].color[2]);
			glBindVertexArray(recs[0].VAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
	}
	else {
		for (int i = 0; i < 6; ++i) {
			if (recs[i].val) {
				glUniform3f(vColor, recs[i].color[0], recs[i].color[1], recs[i].color[2]);
				glBindVertexArray(recs[i].VAO);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			}
		}
	}

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

void TimerFunction(int value)
{
	glutPostRedisplay();
	glutTimerFunc(16, TimerFunction, 1);
}

GLvoid InitBuffer()
{
	GLuint VBO_pos, EBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO_pos);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_pos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vPositionList), vPositionList, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
}

GLvoid InitBufferAll()
{
	for (int i =0; i < 3; ++i) {
		for (int j = 0; j < 6; ++j)
			lines[i].pos[j] = linePos[i][j];
		i == 0 ? lines[i].color[0] = 1.0f : lines[i].color[0] = 0.0f;
		i == 1 ? lines[i].color[1] = 1.0f : lines[i].color[1] = 0.0f;
		i == 2 ? lines[i].color[2] = 1.0f : lines[i].color[2] = 0.0f;

		glGenVertexArrays(1, &lines[i].VAO);
		glGenBuffers(1, &lines[i].VBO);

		glBindVertexArray(lines[i].VAO);

		glBindBuffer(GL_ARRAY_BUFFER, lines[i].VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(lines[i].pos), lines[i].pos, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glGenBuffers(1, &lines[i].EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lines[i].EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sIndex), sIndex, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
	}
	
	for (int i =0; i <4; ++i) {
		for (int j = 0; j < 9; ++j)
			tris[i].pos[j] = triPos[i][j];
		tris[i].color[0] = (float)(rand() % 256) / 255.0f;
		tris[i].color[1] = (float)(rand() % 256) / 255.0f;
		tris[i].color[2] = (float)(rand() % 256) / 255.0f;
		tris[i].val = true;

		glGenVertexArrays(1, &tris[i].VAO);
		glGenBuffers(1, &tris[i].VBO);

		glBindVertexArray(tris[i].VAO);

		glBindBuffer(GL_ARRAY_BUFFER, tris[i].VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(tris[i].pos), tris[i].pos, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glGenBuffers(1, &tris[i].EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tris[i].EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sIndex), sIndex, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
	}

	for (int i =0; i <6; ++i) {
		for (int j = 0; j < 12; ++j)
			recs[i].pos[j] = recPos[i][j];
		recs[i].color[0] = (float)(rand() % 256) / 255.0f;
		recs[i].color[1] = (float)(rand() % 256) / 255.0f;
		recs[i].color[2] = (float)(rand() % 256) / 255.0f;
		recs[i].val = true;

		glGenVertexArrays(1, &recs[i].VAO);
		glGenBuffers(1, &recs[i].VBO);

		glBindVertexArray(recs[i].VAO);

		glBindBuffer(GL_ARRAY_BUFFER, recs[i].VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(recs[i].pos), recs[i].pos, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glGenBuffers(1, &recs[i].EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, recs[i].EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sIndex), sIndex, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
	}
}