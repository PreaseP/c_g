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
	glm::mat4 mat;
	float angle;
} mTri;

typedef struct REC {
	GLfloat pos[12];
	GLfloat color[3];
	GLuint VAO, VBO, EBO;
	glm::mat4 mat;
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

enum triFace { t_FRONT = 0, t_RIGHT, t_BACK, t_LEFT };

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

enum recFace { r_BOTTOM = 0, r_FRONT, r_RIGHT, r_BACK, r_LEFT, r_TOP };

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

float roty = 0.0f;
float yDir = 0.0f;

bool topSpin = false;
float topAngle = 0.0f;

bool frontMove = false;
float frontAngle = 0.0f;
float frontDir = 0.0f;

bool sideSpin = false;
float sideAngle = 0.0f;

bool backScaleT = false;
float backScale = 0.0f;
float backScaleDir = 0.0f;

bool openAll = false;
float openAllDir = 0.0f;

bool openStep = false;
int openStepIdx = t_FRONT;
float openStepDir = 0.0f;

// -25, -30, 10

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
	std::cout << "h: 은면 제거/해제" << std::endl;
	std::cout << "p : 육면체/사각뿔" << std::endl;
	std::cout << "y : y축 자전/멈춤" << std::endl;
	std::cout<< "c : 리셋하고 육면체 출력" << std::endl;
	std::cout<< "q : 프로그램 종료" << std::endl;
	std::cout<< "---육면체 애니메이션---" << std::endl;
	std::cout<< "t : 윗면의 가운데 축 (z축)을 중심으로 회전/멈추기" << std::endl;
	std::cout<< "f : 앞면이 위축을 중심으로 회전하여 열기/닫기" << std::endl;
	std::cout<< "s : 육면체의 옆면이 중점을 기준으로 제자리에서 회전/멈추기" << std::endl;
	std::cout<< "b : 뒷면이 제 자리에서 작아지며 없어진다/커진다." << std::endl;
	std::cout<< "---사각뿔 애니메이션---" << std::endl;
	std::cout<< "o : 사각뿔의 모든 면들이 함께 연다/닫는다." << std::endl;
	std::cout<< "r : 사각뿔의 각 면이 한 개씩 연다/닫는다." << std::endl;

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
		p_toggle = !p_toggle;
		if (p_toggle) {
			for (int i = 0; i < 4; ++i) tris[i].angle = 0.0f;
			openAll = false;
			openAllDir = 0.0f;
			openStep = false;
			openStepDir = 0.0f;
			openStepIdx = t_FRONT;
		}
	}
	else if (key == 'h') {
		h_toggle = !h_toggle;
	}
	else if (key == 'y') {
		yDir = (yDir != 0.0f) ? 0.0f : 1.0f;
	}
	else if (key == 'c') {
		p_toggle = false;
		h_toggle = false;
		InitBufferAll();
		roty = 0.0f;
		yDir = 0.0f;
		topSpin = false;
		topAngle = 0.0f;
		frontMove = false;
		frontAngle = 0.0f;
		frontDir = 0.0f;
		sideSpin = false;
		sideAngle = 0.0f;
		backScaleT = false;
		backScale = 0.0f;
		backScaleDir = 0.0f;
		openAll = false;
		openAllDir = 0.0f;
		openStep = false;
		openStepIdx = t_FRONT;
		openStepDir = 0.0f;
	}
	else if (key == 't') {
		topSpin = !topSpin;
	}
	else if (key == 'f') {
		if (!frontMove) {
			frontMove = true;
			frontAngle == 0.0f ? frontDir = 1.0f : frontDir = -1.0f;
		}
		else {
			frontDir *= -1.0f;
		}
	}
	else if (key == 's') {
		sideSpin = !sideSpin;
	}
	else if (key == 'b') {
		if (!backScaleT) {
			backScaleT = true;
			if(backScaleDir == 0.0f)
				backScaleDir = 1.0f;
		}
		else {
			backScaleT = false;
		}
	}
	else if (key == 'o') {
		p_toggle = true;
		if (!openAll) {
			openAll = true;
			tris[0].angle == 0.0f ? openAllDir = 1.0f : openAllDir = -1.0f;
		}
		else {
			openAllDir *= -1.0f;
		}
	}
	else if (key == 'r') {
		p_toggle = true;
		if (!openStep) {
			openStep = true;
			openStepDir = 1.0f;
			for (int i = 0; i < 4; ++i) {
				if (tris[i].angle != 0.0f) {
					openStepDir = -1.0f;
					break;
				}
			}
		}
		else {
			openStepDir *= -1.0f;
		}
	}

	glutPostRedisplay();
}

void SpecialKeyboard(int key, int x, int y) 
{
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
	glm::mat4 scale = glm::mat4(1.0f);
	glm::mat4 sRes = glm::mat4(1.0f);	

	scale = glm::scale(scale, glm::vec3(0.5f, 0.5f, 0.5f));
	sRoty = glm::rotate(sRoty, glm::radians(roty), glm::vec3(0.0f, 1.0f, 0.0f));

	sRes = sRoty * scale * res;

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
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(sRes * tris[i].mat));
			glBindVertexArray(tris[i].VAO);
			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
		}
		glUniform3f(vColor, recs[0].color[0], recs[0].color[1], recs[0].color[2]);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(sRes * recs[0].mat));
		glBindVertexArray(recs[0].VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
	else {
		for (int i = 0; i < 6; ++i) {
			glUniform3f(vColor, recs[i].color[0], recs[i].color[1], recs[i].color[2]);
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(sRes * recs[i].mat));
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
	if (yDir) {
		roty += yDir * 0.5f;
		if (roty > 360.0f) roty = 0.0f;
	}
	if (topSpin) {
		topAngle += 2.0f;
		if (topAngle >= 360.0f) topAngle -= 360.0f;

		const glm::mat4 tOrigin = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, 0.0f));
		const glm::mat4 tBack = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, 0.0f));
		const glm::mat4 rz = glm::rotate(glm::mat4(1.0f), glm::radians(topAngle), glm::vec3(0.0f, 0.0f, 1.0f));

		recs[r_TOP].mat = tBack * rz * tOrigin;
	}
	if (frontMove) {
		if (frontDir) {
			frontAngle += frontDir * 2.0f;
			if (frontAngle >= 90.0f) {
				frontAngle = 90.0f;
				frontDir = 0.0f;
				frontMove = false;
			}
			else if (frontAngle <= 0.0f) {
				frontAngle = 0.0f;
				frontDir = 0.0f;
				frontMove = false;
			}
		}
		const glm::mat4 tOrigin = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, +0.5f));
		const glm::mat4 tBack = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, -0.5f));
		const glm::mat4 rx = glm::rotate(glm::mat4(1.0f), glm::radians(frontAngle), glm::vec3(1.0f, 0.0f, 0.0f));

		recs[r_FRONT].mat = tBack * rx * tOrigin;
	}
	if (sideSpin) {
		sideAngle += 2.0f;
		if (sideAngle >= 360.0f) sideAngle -= 360.0f;

		const glm::mat4 rx = glm::rotate(glm::mat4(1.0f), glm::radians(sideAngle), glm::vec3(1.0f, 0.0f, 0.0f));

		recs[r_RIGHT].mat = rx;
		recs[r_LEFT].mat = rx;
	}
	if (backScaleT) {
		backScale += backScaleDir * 0.02f;
		if (backScale >= 1.0f) {
			backScale = 1.0f;
			backScaleDir = -1.0f;
		}
		else if (backScale <= 0.0f) {
			backScale = 0.0f;
			backScaleDir = 1.0f;
		}
		const glm::mat4 sc = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f - backScale, 1.0f - backScale, 1.0f - backScale));
		recs[r_BACK].mat = sc;
	}
	if (openAll) {
		for (int i = t_FRONT; i <= t_LEFT; ++i) {
			if (openAllDir) {
				tris[i].angle -= openAllDir * 1.0f;
				if (i == t_LEFT) {
					if (tris[i].angle <= -233.0f) {
						tris[i].angle = -233.0f;
						openAllDir = 0.0f;
						openAll = false;
					}
					else if (tris[i].angle >= 0.0f) {
						tris[i].angle = 0.0f;
						openAllDir = 0.0f;
						openAll = false;
					}
				}
			}
			glm::mat4 rot = glm::mat4(1.0f);
			glm::mat4 tOrigin = glm::mat4(1.0f);
			glm::mat4 tBack = glm::mat4(1.0f);
			switch (i) {
			case t_FRONT:
				tOrigin = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, 0.5f));
				tBack = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, -0.5f));
				rot = glm::rotate(glm::mat4(1.0f), glm::radians(tris[i].angle), glm::vec3(1.0f, 0.0f, 0.0f));
				break;
			case t_RIGHT:
				tOrigin = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, 0.5f, 0.0f));
				tBack = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, -0.5f, 0.0f));
				rot = glm::rotate(glm::mat4(1.0f), glm::radians(tris[i].angle), glm::vec3(0.0f, 0.0f, 1.0f));
				break;
			case t_BACK:
				tOrigin = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, -0.5f));
				tBack = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, 0.5f));
				rot = glm::rotate(glm::mat4(1.0f), glm::radians(tris[i].angle), glm::vec3(-1.0f, 0.0f, 0.0f));
				break;
			case t_LEFT:
				tOrigin = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.0f));
				tBack = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, -0.5f, 0.0f));
				rot = glm::rotate(glm::mat4(1.0f), glm::radians(tris[i].angle), glm::vec3(0.0f, 0.0f, -1.0f));
				break;
			}

			tris[i].mat = tBack * rot * tOrigin;
		}
	}
	if (openStep) {
		tris[openStepIdx].angle -= openStepDir * 1.0f;

		glm::mat4 rot = glm::mat4(1.0f);
		glm::mat4 tOrigin = glm::mat4(1.0f);
		glm::mat4 tBack = glm::mat4(1.0f);

		switch (openStepIdx) {
		case t_FRONT:
			tOrigin = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, 0.5f));
			tBack = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, -0.5f));
			rot = glm::rotate(glm::mat4(1.0f), glm::radians(tris[openStepIdx].angle), glm::vec3(1.0f, 0.0f, 0.0f));
			break;
		case t_RIGHT:
			tOrigin = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, 0.5f, 0.0f));
			tBack = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, -0.5f, 0.0f));
			rot = glm::rotate(glm::mat4(1.0f), glm::radians(tris[openStepIdx].angle), glm::vec3(0.0f, 0.0f, 1.0f));
			break;
		case t_BACK:
			tOrigin = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, -0.5f));
			tBack = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, 0.5f));
			rot = glm::rotate(glm::mat4(1.0f), glm::radians(tris[openStepIdx].angle), glm::vec3(-1.0f, 0.0f, 0.0f));
			break;
		case t_LEFT:
			tOrigin = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.0f));
			tBack = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, -0.5f, 0.0f));
			rot = glm::rotate(glm::mat4(1.0f), glm::radians(tris[openStepIdx].angle), glm::vec3(0.0f, 0.0f, -1.0f));
			break;
		}

		tris[openStepIdx].mat = tBack * rot * tOrigin;

		if (tris[openStepIdx].angle <= -90.0f) {
			tris[openStepIdx].angle = -90.0f;
			openStepIdx++;
			if (openStepIdx > t_LEFT) {
				openStepDir = 0.0f;
				openStepIdx--;
				openStep = false;
			}
		}
		else if (tris[openStepIdx].angle >= 0.0f) {
			tris[openStepIdx].angle = 0.0f;
			openStepIdx--;
			if (openStepIdx < t_FRONT) {
				openStepDir = 0.0f;
				openStepIdx++;
				openStep = false;
			}
		}
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
		tris[i].mat = glm::mat4(1.0f);
		tris[i].angle = 0.0f;

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
		recs[i].mat = glm::mat4(1.0f);

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