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

#define WinX 1280
#define WinY 720

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

float linePos[3][6] = {
	{2.0f, 0.0f, 0.0f,
	-2.0f, 0.0f, 0.0f},
	{0.0f, 2.0f, 0.0f,
	0.0f,  -2.0f, 0.0f},
	{0.0f, 0.0f, 2.0f,
	0.0f, 0.0f, -2.0f}
};

enum triFace { t_FRONT = 0, t_RIGHT, t_BACK, t_LEFT };

float triPos[4][9] = {
	// Front
	{ 0.0f,  0.5f,  0.0f,   -0.5f, -0.5f,  0.5f,    0.5f, -0.5f,  0.5f },
	// Right
	{ 0.0f,  0.5f,  0.0f,    0.5f, -0.5f, 0.5f,    0.5f, -0.5f,  -0.5f },
	// Back
	{ 0.0f,  0.5f,  0.0f,   0.5f, -0.5f, -0.5f,    -0.5f, -0.5f, -0.5f },
	// Left
	{ 0.0f,  0.5f,  0.0f,   -0.5f, -0.5f, -0.5f,   -0.5f, -0.5f, 0.5f }
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

mLine lines[3];
mTri tris[4];
mRec recs[6];

unsigned int sIndex[] = {
   0, 1, 2
};

unsigned int rIndex[] = {
   0, 2, 1,
   0, 3, 2
};

GLint sel = 0;

GLfloat rotX = 0.0f, rotY = 0.0f, rotZ = 0.0f, rotRX = 0.0f, rotRY = 0.0f, rotRZ = 0.0f, rotYY = 0.0f, rotRYY = 0.0f;
GLfloat sca = 1.0f, scaR = 1.0f, scaO = 1.0f, scaRO = 1.0f;
GLfloat posX = 1.0f, posY = 0.0f, posRX = -1.0f, posRY = 0.0f;

int rotX_dir = 0, rotRX_dir = 0;
int rotY_dir = 0, rotRY_dir = 0;
int rotYY_dir = 0, rotRYY_dir = 0;
int sca_dir = 0, scaR_dir = 0;
int scaO_dir = 0, scaRO_dir = 0;

int moveT = 0;
int phaseU = 0;
bool T_toggle = false;
bool V_toggle = false;
bool C_toggle = false;

GLfloat opDis[2];
GLfloat opVel[2][2];

GLuint sphereVAO = 0, sphereVBO = 0, sphereEBO = 0;
GLsizei sphereIndexCount = 0;

GLuint coneVAO = 0, coneVBO = 0, coneEBO = 0;
GLsizei coneIndexCount = 0;

void BuildSphere(float radius, int stacks, int slices, std::vector<float>& vertices, std::vector<unsigned int>& indices);
void BuildCone(float radius, float height, int slices, std::vector<float>& vertices, std::vector<unsigned int>& indices);

void resetAll();

GLvoid InitBufferAll();

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

void Mouse(int button, int state, int x, int y);
void Keyboard(unsigned char key, int x, int y);
void SpecialKeyboard(int key, int x, int y);
void Motion(int x, int y);
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
	InitBufferAll();
	srand(time(NULL));
	std::cout << "x/X: 객체의 x축에 대하여 각각 양/음 방향으로 회전하기(자전)" << std::endl;
	std::cout << "y/Y: 객체의 y축에 대하여 각각 양/음 방향으로 회전하기(자전)" << std::endl;
	std::cout << "r/R: 중앙의 y축에 대하여 양/음 방향으로 회전하기(공전)" << std::endl;
	std::cout << "a/A: 도형이 제자리에서 확대/축소" << std::endl;
	std::cout << "b/B: 도형이 원점에 대해서 확대/축소" << std::endl;
	std::cout << "d/D: 도형이 x축에서 좌/우로 이동" << std::endl;
	std::cout << "e/E: 도형이 y축에서 위/아래로 이동" << std::endl;
	std::cout << "-----------------------------------------------------------------" << std::endl;
	std::cout << "t: 두 도형이 원점을 통과하며 상대방의 자리로 이동하는 애니메이션" << std::endl;
	std::cout << "u: 두 도형이 한개는 위로, 다른 도형은 아래로 이동하면서 상대방의 자리로 이동하는 애니메이션" << std::endl;
	std::cout << "v: 두 도형이 한개는 확대, 다른 한개는 축소되며 자전과 공전하기" << std::endl;
	std::cout << "c: 두 도형을 다른 도형으로 바꾼다." << std::endl;
	std::cout << "s: 초기화하기" << std::endl;


	//--- 세이더프로그램만들기
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(SpecialKeyboard);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutTimerFunc(16, TimerFunction, 1);
	glutMainLoop();
}

void Keyboard(unsigned char key, int x, int y)
{
	if (key == 'q') exit(0);
	else if (key == '1') sel = 1;
	else if (key == '2') sel = 2;
	else if (key == '3') sel = 3;
	else if (key == 'x') {
		switch (sel) {
		case 1:
			rotRX_dir = 1;
			rotRY_dir = 0;
			rotRYY_dir = 0;
			break;
		case 2:
			rotX_dir = 1;
			rotY_dir = 0;
			rotYY_dir = 0;
			break;
		case 3:
			rotRX_dir = 1;
			rotX_dir = 1;
			rotRY_dir = 0;
			rotY_dir = 0;
			rotRYY_dir = 0;
			rotYY_dir = 0;
			break;
		}
	}
	else if (key == 'X') {
		switch (sel) {
		case 1:
			rotRX_dir = -1;
			rotRY_dir = 0;
			rotRYY_dir = 0;
			break;
		case 2:
			rotX_dir = -1;
			rotY_dir = 0;
			rotYY_dir = 0;
			break;
		case 3:
			rotRX_dir = -1;
			rotX_dir = -1;
			rotRY_dir = 0;
			rotY_dir = 0;
			rotRYY_dir = 0;
			rotYY_dir = 0;
			break;
		}
	}
	else if (key == 'y') {
		switch (sel) {
		case 1:
			rotRX_dir = 0;
			rotRY_dir = 1;
			rotRYY_dir = 0;
			break;
		case 2:
			rotX_dir = 0;
			rotY_dir = 1;
			rotYY_dir = 0;
			break;
		case 3:
			rotRX_dir = 0;
			rotX_dir = 0;
			rotRY_dir = 1;
			rotY_dir = 1;
			rotRYY_dir = 0;
			rotYY_dir = 0;
			break;
		}
	}
	else if (key == 'Y') {
		switch (sel) {
		case 1:
			rotRX_dir = 0;
			rotRY_dir = -1;
			rotRYY_dir = 0;
			break;
		case 2:
			rotX_dir = 0;
			rotY_dir = -1;
			rotYY_dir = 0;
			break;
		case 3:
			rotRX_dir = 0;
			rotX_dir = 0;
			rotRY_dir = -1;
			rotY_dir = -1;
			rotRYY_dir = 0;
			rotYY_dir = 0;
			break;
		}
	}
	else if (key == 'r') {
		switch (sel) {
		case 1:
			rotRX_dir = 0;
			rotRY_dir = 0;
			rotRYY_dir = 1;
			break;
		case 2:
			rotX_dir = 0;
			rotY_dir = 0;
			rotYY_dir = 1;
			break;
		case 3:
			rotRX_dir = 0;
			rotX_dir = 0;
			rotRY_dir = 0;
			rotY_dir = 0;
			rotRYY_dir = 1;
			rotYY_dir = 1;
			break;
		}
	}
	else if (key == 'R') {
		switch (sel) {
		case 1:
			rotRX_dir = 0;
			rotRY_dir = 0;
			rotRYY_dir = -1;
			break;
		case 2:
			rotX_dir = 0;
			rotY_dir = 0;
			rotYY_dir = -1;
			break;
		case 3:
			rotRX_dir = 0;
			rotX_dir = 0;
			rotRY_dir = 0;
			rotY_dir = 0;
			rotRYY_dir = -1;
			rotYY_dir = -1;
			break;
		}
	}
	else if (key == 'a') {
		switch (sel) {
		case 1:
			scaR_dir = 1;
			scaRO_dir = 0;
			scaRO = 1.0f;
			break;
		case 2:
			sca_dir = 1;
			scaO_dir = 0;
			scaO = 1.0f;
			break;
		case 3:
			scaR_dir = 1;
			sca_dir = 1;
			scaRO_dir = 0;
			scaO_dir = 0;
			scaRO = 1.0f;
			scaO = 1.0f;
			break;
		}
	}
	else if (key == 'A') {
		switch (sel) {
		case 1:
			scaR_dir = -1;
			scaRO_dir = 0;
			scaRO = 1.0f;
			break;
		case 2:
			sca_dir = -1;
			scaO_dir = 0;
			scaO = 1.0f;
			break;
		case 3:
			scaR_dir = -1;
			sca_dir = -1;
			scaRO_dir = 0;
			scaO_dir = 0;
			scaRO = 1.0f;
			scaO = 1.0f;
			break;
		}
	}
	else if (key == 'b') {
		switch (sel) {
		case 1:
			scaR_dir = 0;
			scaRO_dir = 1;
			scaR = 1.0f;
			break;
		case 2:
			sca_dir = 0;
			scaO_dir = 1;
			sca = 1.0f;
			break;
		case 3:
			scaR_dir = 0;
			sca_dir = 0;
			scaRO_dir = 1;
			scaO_dir = 1;
			scaR = 1.0f;
			sca = 1.0f;
			break;
		}
	}
	else if (key == 'B') {
		switch (sel) {
		case 1:
			scaR_dir = 0;
			scaRO_dir = -1;
			scaR = 1.0f;
			break;
		case 2:
			sca_dir = 0;
			scaO_dir = -1;
			sca = 1.0f;
			break;
		case 3:
			scaR_dir = 0;
			sca_dir = 0;
			scaRO_dir = -1;
			scaO_dir = -1;
			scaR = 1.0f;
			sca = 1.0f;
			break;
		}
	}
	else if (key == 'd') {
		switch (sel) {
		case 1:
			posRX -= 0.1f;
			break;
		case 2:
			posX -= 0.1f;
			break;
		case 3:
			posRX -= 0.1f;
			posX -= 0.1f;
			break;
		}
	}
	else if (key == 'D') {
		switch (sel) {
		case 1:
			posRX += 0.1f;
			break;
		case 2:
			posX += 0.1f;
			break;
		case 3:
			posRX += 0.1f;
			posX += 0.1f;
			break;
		}
	}
	else if (key == 'e') {
		switch (sel) {
		case 1:
			posRY += 0.1f;
			break;
		case 2:
			posY += 0.1f;
			break;
		case 3:
			posRY += 0.1f;
			posY += 0.1f;
			break;
		}
	}
	else if (key == 'E') {
		switch (sel) {
		case 1:
			posRY -= 0.1f;
			break;
		case 2:
			posY -= 0.1f;
			break;
		case 3:
			posRY -= 0.1f;
			posY -= 0.1f;
			break;
		}
	}
	else if (key == 't') {
		moveT = 10;
		opDis[0] = abs(posX - posRX);
		opDis[1] = abs(posY - posRY);
		opVel[0][0] = opDis[0] / moveT;
		opVel[0][1] = opDis[1] / moveT;
		opVel[1][0] = -opVel[0][0];
		opVel[1][1] = -opVel[0][1];
		T_toggle = !T_toggle;
	}
	else if (key == 'u') {
		moveT = 10;
		phaseU = 1;
		opDis[0] = abs(posX - posRX);
		opDis[1] = abs(posY - posRY) + 1.0f;
		opVel[0][0] = opDis[0] / moveT;
		opVel[0][1] = 1.0f / moveT;
		opVel[1][0] = -opVel[0][0];
		opVel[1][1] = -opVel[0][1];
		T_toggle = !T_toggle;
	}
	else if (key == 'v') {
		V_toggle = !V_toggle;
	}
	else if (key == 'c') {
		C_toggle = !C_toggle;
	}
	else if (key == 's') {
		resetAll();
	}
	glutPostRedisplay();
}

void SpecialKeyboard(int key, int x, int y)
{
	glutPostRedisplay();
}

void Mouse(int button, int state, int x, int y) {
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
	glUseProgram(shaderProgramID);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);

	unsigned int modelLoc = glGetUniformLocation(shaderProgramID, "model");
	unsigned int viewLoc = glGetUniformLocation(shaderProgramID, "view");
	unsigned int projLoc = glGetUniformLocation(shaderProgramID, "projection");
	unsigned int vColor = glGetUniformLocation(shaderProgramID, "vColor");

	// 기본 축
	glm::mat4 cord = glm::mat4(1.0f);
	cord = glm::rotate(cord, glm::radians(25.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // X 회전
	cord = glm::rotate(cord, glm::radians(-30.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Y 회전
	cord = glm::rotate(cord, glm::radians(10.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Z 회전

	// 좌측 도형에 대한 변환 행렬
	glm::mat4 mT = glm::mat4(1.0f);
	mT = glm::scale(mT, glm::vec3(scaO, scaO, scaO)); // 원점에 대해 확대/축소
	mT = glm::rotate(mT, glm::radians(rotYY), glm::vec3(0.0f, 1.0f, 0.0f)); // Y 공전
	mT = glm::translate(mT, glm::vec3(posX, posY, 0.0f)); // 이동
	mT = glm::scale(mT, glm::vec3(sca, sca, sca)); // 제자리에서 확대/축소
	mT = glm::rotate(mT, glm::radians(rotX), glm::vec3(1.0f, 0.0f, 0.0f)); // X 회전
	mT = glm::rotate(mT, glm::radians(rotY), glm::vec3(0.0f, 1.0f, 0.0f)); // Y 회전

	// 우측 도형에 대한 변환 행렬
	glm::mat4 rmT = glm::mat4(1.0f);
	rmT = glm::scale(rmT, glm::vec3(scaRO, scaRO, scaRO)); // 원점에 대해 확대/축소
	rmT = glm::rotate(rmT, glm::radians(rotRYY), glm::vec3(0.0f, 1.0f, 0.0f)); // Y 공전
	rmT = glm::translate(rmT, glm::vec3(posRX, posRY, 0.0f)); // 이동
	rmT = glm::scale(rmT, glm::vec3(scaR, scaR, scaR)); // 제자리에서 확대/축소
	rmT = glm::rotate(rmT, glm::radians(rotRX), glm::vec3(1.0f, 0.0f, 0.0f)); // X 회전
	rmT = glm::rotate(rmT, glm::radians(rotRY), glm::vec3(0.0f, 1.0f, 0.0f)); // Y 회전

	// 자전용 코드
	// glm::mat4 camRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(rotCY), glm::vec3(0.0f, 1.0f, 0.0f));
	// glm::vec3 rotatedDirection = glm::vec3(camRotationMatrix * glm::vec4(0.0f, 0.0f, -1.0f, 1.0f)); // 초기 방향을 (0,0,-1)로 가정
	// glm::mat4 view = glm::lookAt(cameraPos, cameraPos + rotatedDirection, cameraUp);
	// 카메라 y축 공전
	//glm::mat4 view = glm::lookAt(cameraPos, cameraDirection, cameraUp)
		//* glm::rotate(glm::mat4(1.0f), glm::radians(rotCY), glm::vec3(0.0f, 1.0f, 0.0f));

	// 뷰 행렬
	glm::mat4 vT = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &vT[0][0]);

	// 투영 행렬
	glm::mat4 pT = glm::perspective(glm::radians(45.0f), (float)WinX / (float)WinY, 0.1f, 100.0f);
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, &pT[0][0]);

	glClearColor(1.0, 1.0, 1.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(cord));

	for (int i = 0; i < 3; ++i) {
		// 좌표축 그리기
		glUniform3f(vColor, lines[i].color[0], lines[i].color[1], lines[i].color[2]);
		glBindVertexArray(lines[i].VAO);
		glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);
	}

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(cord * mT));

	if (!C_toggle) {
		// 구 그리기
		glUniform3f(vColor, 0.2f, 0.5f, 0.9f);
		glBindVertexArray(sphereVAO);
		glDrawElements(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
	else {
		// 원뿔 그리기
		glUniform3f(vColor, 0.95f, 0.6f, 0.2f);
		glBindVertexArray(coneVAO);
		glDrawElements(GL_TRIANGLES, coneIndexCount, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(cord * rmT));

	if (!C_toggle) {
		// 정육면체 그리기
		glUniform3f(vColor, recs[0].color[0], recs[0].color[1], recs[0].color[2]);
		for (int i = 0; i < 6; ++i) {
			glBindVertexArray(recs[i].VAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
	}
	else {
		// 사각뿔 그리기
		glUniform3f(vColor, tris[0].color[0], tris[0].color[1], tris[0].color[2]);
		for (int i = 0; i < 4; ++i) {
			glBindVertexArray(tris[i].VAO);
			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
		}
		glUniform3f(vColor, recs[0].color[0], recs[0].color[1], recs[0].color[2]);
		glBindVertexArray(recs[0].VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

void Motion(int x, int y) {
	glutPostRedisplay();
}

void TimerFunction(int value)
{
	rotX = rotX + rotX_dir * 0.4f;
	if (rotX > 360.0f) rotX = rotX - 360.0f;
	else if (rotX < 0.0f) rotX = rotX + 360.0f;

	rotRX = rotRX + rotRX_dir * 0.4f;
	if (rotRX > 360.0f) rotRX = rotRX - 360.0f;
	else if (rotRX < 0.0f) rotRX = rotRX + 360.0f;

	rotY = rotY + rotY_dir * 0.4f;
	if (rotY > 360.0f) rotY = rotY - 360.0f;
	else if (rotY < 0.0f) rotY = rotY + 360.0f;

	rotRY = rotRY + rotRY_dir * 0.4f;
	if (rotRY > 360.0f) rotRY = rotRY - 360.0f;
	else if (rotRY < 0.0f) rotRY = rotRY + 360.0f;

	rotYY = rotYY + rotYY_dir * 0.4f;
	if (rotYY > 360.0f) rotYY = rotYY - 360.0f;
	else if (rotYY < 0.0f) rotYY = rotYY + 360.0f;

	rotRYY = rotRYY + rotRYY_dir * 0.4f;
	if (rotRYY > 360.0f) rotRYY = rotRYY - 360.0f;
	else if (rotRYY < 0.0f) rotRYY = rotRYY + 360.0f;

	sca = sca + sca_dir * 0.01f;
	if (sca > 2.0f) sca = 2.0f;
	else if (sca < 0.5f) sca = 0.5f;

	scaR = scaR + scaR_dir * 0.01f;
	if (scaR > 2.0f) scaR = 2.0f;
	else if (scaR < 0.5f) scaR = 0.5f;

	scaO = scaO + scaO_dir * 0.01f;
	if (scaO > 2.0f) scaO = 2.0f;
	else if (scaO < 0.5f) scaO = 0.5f;

	scaRO = scaRO + scaRO_dir * 0.01f;
	if (scaRO > 2.0f) scaRO = 2.0f;
	else if (scaRO < 0.5f) scaRO = 0.5f;

	if (moveT && phaseU == 0) {
		if (T_toggle) {
			posX -= opVel[0][0];
			posY -= opVel[0][1];
			posRX -= opVel[1][0];
			posRY -= opVel[1][1];
		}
		else {
			posX += opVel[0][0];
			posY += opVel[0][1];
			posRX += opVel[1][0];
			posRY += opVel[1][1];
		}
		moveT--;
	}

	if (phaseU == 1 && moveT) {
		posY += opVel[0][1];
		posRY += opVel[1][1];

		moveT--;
		if (moveT == 0) {
			phaseU = 2;
			moveT = 10;
		}
	}

	if (phaseU == 2 && moveT) {
		if (T_toggle) {
			posX -= opVel[0][0];
			posRX -= opVel[1][0];
		}
		else {
			posX += opVel[0][0];
			posRX += opVel[1][0];
		}
		moveT--;
		if (moveT == 0) {
			phaseU = 3;
			moveT = 10;
			opVel[0][1] = opDis[1] / moveT;
			opVel[1][1] = -opVel[0][1];
		}
	}

	if (phaseU == 3 && moveT) {
		posY -= opVel[0][1];
		posRY -= opVel[1][1];
		moveT--;
		if (moveT == 0) {
			phaseU = 0;
		}
	}

	if (V_toggle) {
		if (sca < 2.0f) sca += 0.001f;
		if (scaR > 0.5f) scaR -= 0.001f;
		rotX += 0.4f;
		rotRX += 0.4f;
		rotY += 0.4f;
		rotRY += 0.4f;
		rotYY += 0.4f;
		rotRYY += 0.4f;
	}

	glutPostRedisplay();
	glutTimerFunc(16, TimerFunction, 1);
}

GLvoid InitBufferAll()
{
	// 좌표축 (선 생성)
	for (int i = 0; i < 3; ++i) {
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

	// 구 생성 및 버퍼 업로드
	std::vector<float> vertices;
	std::vector<unsigned int> indices;
	BuildSphere(1.0f, 32, 64, vertices, indices);
	sphereIndexCount = (GLsizei)indices.size();

	glGenVertexArrays(1, &sphereVAO);
	glGenBuffers(1, &sphereVBO);
	glGenBuffers(1, &sphereEBO);

	glBindVertexArray(sphereVAO);

	glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);

	// 원뿔 생성 및 버퍼 업로드 (반지름 0.5, 높이 1.0, 슬라이스 64)
	std::vector<float> cverts;
	std::vector<unsigned int> cinds;
	BuildCone(0.5f, 1.0f, 64, cverts, cinds);
	coneIndexCount = (GLsizei)cinds.size();

	glGenVertexArrays(1, &coneVAO);
	glGenBuffers(1, &coneVBO);
	glGenBuffers(1, &coneEBO);

	glBindVertexArray(coneVAO);

	glBindBuffer(GL_ARRAY_BUFFER, coneVBO);
	glBufferData(GL_ARRAY_BUFFER, cverts.size() * sizeof(float), cverts.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, coneEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, cinds.size() * sizeof(unsigned int), cinds.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);

	// 정육면체
	for (int i = 0; i < 6; ++i) {
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
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(rIndex), rIndex, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
	}

	// 사각뿔
	for (int i = 0; i < 4; ++i) {
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
}

// 구 버텍스/인덱스 생성
void BuildSphere(float radius, int stacks, int slices, std::vector<float>& vertices, std::vector<unsigned int>& indices)
{
	const float PI = 3.14159265358979323846f;
	vertices.clear();
	indices.clear();
	vertices.reserve((size_t)(stacks + 1) * (slices + 1) * 3);

	for (int i = 0; i <= stacks; ++i) {
		float v = (float)i / (float)stacks;
		float phi = -PI * 0.5f + v * PI; // [-pi/2, pi/2]
		float y = std::sin(phi) * radius;
		float r = std::cos(phi) * radius;

		for (int j = 0; j <= slices; ++j) {
			float u = (float)j / (float)slices;
			float theta = u * 2.0f * PI;

			float x = r * std::cos(theta);
			float z = r * std::sin(theta);

			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(z);
		}
	}

	for (int i = 0; i < stacks; ++i) {
		for (int j = 0; j < slices; ++j) {
			unsigned int first = (unsigned int)(i * (slices + 1) + j);
			unsigned int second = first + (unsigned int)(slices + 1);

			// CCW 외향
			indices.push_back(first);
			indices.push_back(second);
			indices.push_back(first + 1);

			indices.push_back(second);
			indices.push_back(second + 1);
			indices.push_back(first + 1);
		}
	}
}

// 원뿔 버텍스/인덱스 생성
void BuildCone(float radius, float height, int slices, std::vector<float>& vertices, std::vector<unsigned int>& indices)
{
	const float PI = 3.14159265358979323846f;
	const float halfH = height * 0.5f;
	vertices.clear();
	indices.clear();

	// 정점: 0 = 꼭짓점(apex), 1 = 바닥 중심, 2.. = 바닥 링(닫힘 위해 slices+1개)
	// apex
	vertices.push_back(0.0f); vertices.push_back(+halfH); vertices.push_back(0.0f);
	// base center
	vertices.push_back(0.0f); vertices.push_back(-halfH); vertices.push_back(0.0f);

	// base ring
	for (int i = 0; i <= slices; ++i) {
		float u = (float)i / (float)slices;
		float theta = u * 2.0f * PI;
		float x = radius * std::cos(theta);
		float z = radius * std::sin(theta);
		vertices.push_back(x);
		vertices.push_back(-halfH);
		vertices.push_back(z);
	}

	// 옆면 삼각형들 (apex, ring[i], ring[i+1])
	for (int i = 0; i < slices; ++i) {
		unsigned int apex = 0;
		unsigned int r0 = 2 + i;
		unsigned int r1 = 2 + i + 1;
		indices.push_back(apex);
		indices.push_back(r0);
		indices.push_back(r1);
	}

	// 바닥 삼각형들 (ring[i+1], ring[i], baseCenter) - 바깥(-Y)을 전면(CCW)로
	for (int i = 0; i < slices; ++i) {
		unsigned int baseC = 1;
		unsigned int r0 = 2 + i;
		unsigned int r1 = 2 + i + 1;
		indices.push_back(r1);
		indices.push_back(r0);
		indices.push_back(baseC);
	}
}

void resetAll() {
	sel = 0;

	rotX = 0.0f, rotY = 0.0f, rotZ = 0.0f, rotRX = 0.0f, rotRY = 0.0f, rotRZ = 0.0f, rotYY = 0.0f, rotRYY = 0.0f;
	sca = 1.0f, scaR = 1.0f, scaO = 1.0f, scaRO = 1.0f;
	posX = 1.0f, posY = 0.0f, posRX = -1.0f, posRY = 0.0f;

	rotX_dir = 0, rotRX_dir = 0;
	rotY_dir = 0, rotRY_dir = 0;
	rotYY_dir = 0, rotRYY_dir = 0;
	sca_dir = 0, scaR_dir = 0;
	scaO_dir = 0, scaRO_dir = 0;

	moveT = 0;
	int phaseU = 0;
	T_toggle = false;
	V_toggle = false;
	C_toggle = false;
}