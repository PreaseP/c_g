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
} mTri;

typedef struct REC {
	GLfloat pos[12];
	GLfloat color[3];
	GLuint VAO, VBO, EBO;
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

float rotx = 0.0f;
float roty = 0.0f;
float rotz = 0.0f;
float xDir = 0.0f;
float yDir = 0.0f;

// -25, -30, 10

float dx = 0.0f;
float dy = 0.0f;

void Mouse(int button, int state, int x, int y);
void Keyboard(unsigned char key, int x, int y);
void SpecialKeyboard(int key, int x, int y);
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

GLchar* vertexSource, * fragmentSource;

//--- 메인 함수
void main(int argc, char** argv)
{
	//--- 윈도우생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(WinX, WinY);
	glutCreateWindow("Example1");

	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	glewInit();

	//--- 세이더읽어와서세이더프로그램만들기: 사용자정의함수호출
	make_shaderProgram();
	InitBuffer();
	InitBufferAll();
	srand(time(NULL));
	std::cout << "c: 육면체" << std::endl;
	std::cout << "p : 사각뿔(피라미드모양으로바닥은사각형, 옆면은삼각형)" << std::endl;
	std::cout << "h : 은면제거적용 / 해제" << std::endl;
	std::cout << "w / W : 와이어객체 / 솔리드객체" << std::endl;
	std::cout << "x / X : x축을기준으로양 / 음방향으로회전애니메이션(자전)" << std::endl;
	std::cout << "y / Y : y축을기준으로양 / 음방향으로회전애니메이션(자전)" << std::endl;
	std::cout << "← / → / ↑ / ↓ : 좌 / 우 / 상 / 하로객체를이동한다. (x축, y축으로이동)" << std::endl;
	std::cout << "s : 초기위치로리셋(모든애니메이션멈추기)" << std::endl;

	//--- 세이더프로그램만들기
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(SpecialKeyboard);
	glutMouseFunc(Mouse);
	glutTimerFunc(16, TimerFunction, 1);
	glutMainLoop();
}

void Keyboard(unsigned char key, int x, int y)
{
	if (key == 'q') exit(0);
	else if (key == 'p') {
		p_toggle = true;
	}
	else if (key == 'c') {
		p_toggle = false;
	}
	else if (key == 'w') {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else if (key == 'W') {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	else if (key == 'h') {
		h_toggle = !h_toggle;
	}
	else if (key == 'x') {
		xDir = 1.0f;
	}
	else if (key == 'y') {
		yDir = 1.0f;
	}
	else if (key == 'X') {
		xDir = -1.0f;
	}
	else if (key == 'Y') {
		yDir = -1.0f;
	}
	else if (key == 's') {
		rotx = 0.0f;
		roty = 0.0f;
		rotz = 0.0f;
		dx = 0.0f;
		dy = 0.0f;
		xDir = 0.0f;
		yDir = 0.0f;
	}

	std::cout << key << std::endl;
	glutPostRedisplay();
}

void SpecialKeyboard(int key, int x, int y) 
{
	if (key == GLUT_KEY_UP) {
		dy += 0.05f;
	}
	else if (key == GLUT_KEY_DOWN) {
		dy -= 0.05f;
	}
	else if (key == GLUT_KEY_LEFT) {
		dx -= 0.05f;
	}
	else if (key == GLUT_KEY_RIGHT) {
		dx += 0.05f;
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

	glm::mat4 Rotx = glm::mat4(1.0f);
	glm::mat4 Roty = glm::mat4(1.0f);
	glm::mat4 Rotz = glm::mat4(1.0f);
	glm::mat4 res = glm::mat4(1.0f);

	Rotx = glm::rotate(Rotx, glm::radians(-25.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	Roty = glm::rotate(Roty, glm::radians(-30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	Rotz = glm::rotate(Rotz, glm::radians(10.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	res = Rotz * Roty * Rotx;

	glm::mat4 sRotx = glm::mat4(1.0f);
	glm::mat4 sRoty = glm::mat4(1.0f);
	glm::mat4 sRotz = glm::mat4(1.0f);
	glm::mat4 trans = glm::mat4(1.0f);
	glm::mat4 sRes = glm::mat4(1.0f);	

	sRotx = glm::rotate(sRotx, glm::radians(rotx), glm::vec3(1.0f, 0.0f, 0.0f));
	sRoty = glm::rotate(sRoty, glm::radians(roty), glm::vec3(0.0f, 1.0f, 0.0f));
	sRotz = glm::rotate(sRotz, glm::radians(rotz), glm::vec3(0.0f, 0.0f, 1.0f));
	trans = glm::translate(trans, glm::vec3(dx, dy, 0.0f));

	sRes = trans * sRotz * sRoty * sRotx * res;

	unsigned int modelLoc = glGetUniformLocation(shaderProgramID, "matrix");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(res));

	int vColor = glGetUniformLocation(shaderProgramID, "vColor");

	for (int i =0; i < 3; ++i) {
		glUniform3f(vColor, lines[i].color[0], lines[i].color[1], lines[i].color[2]);
		glBindVertexArray(lines[i].VAO);
		glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);
	}
	
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(sRes));

	if (p_toggle) {
		for (int i =0; i <4; ++i) {
			glUniform3f(vColor, tris[i].color[0], tris[i].color[1], tris[i].color[2]);
			glBindVertexArray(tris[i].VAO);
			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
		}
		glUniform3f(vColor, recs[0].color[0], recs[0].color[1], recs[0].color[2]);
		glBindVertexArray(recs[0].VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
	else {
		for (int i = 0; i < 6; ++i) {
			glUniform3f(vColor, recs[i].color[0], recs[i].color[1], recs[i].color[2]);
			glBindVertexArray(recs[i].VAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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
	if (xDir) {
		rotx += xDir * 1.0f;
		if (rotx > 360.0f || rotx < -360.0f) rotx = 0.0f;
	}
	if (yDir) {
		roty += yDir * 1.0f;
		if (roty > 360.0f || roty < -360.0f) roty = 0.0f;
	}

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