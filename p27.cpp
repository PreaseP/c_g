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

typedef struct REC {
	GLfloat pos[12];
	GLfloat normal[12];
	GLfloat color[3];
	GLuint VAO, VBO_pos, VBO_normal, EBO;
} mRec;

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

float recNormal[6][12] = {
	{ 0.0f,-1.0f, 0.0f,  0.0f,-1.0f, 0.0f,  0.0f,-1.0f, 0.0f,  0.0f,-1.0f, 0.0f }, // Bottom
	{ 0.0f, 0.0f,-1.0f,  0.0f, 0.0f,-1.0f,  0.0f, 0.0f,-1.0f,  0.0f, 0.0f,-1.0f }, // Front
	{ 1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f }, // Right
	{ 0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f }, // Back
	{-1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f }, // Left
	{ 0.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f }  // Top
};

typedef struct CUBOID {
	mRec recs[6];
	glm::mat4 mat;
} mCuboid;

mCuboid cuboids[8];

enum cuNum { bot = 0, mid, top1, top2, gun1, gun2, flag1, flag2 };

// 탱크 이동 제한을 위한 상수 추가 (바닥 경계와 탱크 반치수)
static const float kFloorMinX = -10.0f;
static const float kFloorMaxX = 10.0f;
static const float kFloorMinZ = -10.0f;
static const float kFloorMaxZ = 10.0f;

// bot 큐브의 스케일 결과(±3, ±1.5), bot의 기본 중심 z는 -5.0f
static const float kTankHalfX = 3.0f;
static const float kTankHalfZ = 1.5f;
static const float kTankBaseCenterZ = -5.0f;

GLfloat gPos[12] = {
	-10.0f,-10.0f,-10.0f,   -10.0f,-10.0f, 10.0f,   10.0f,-10.0f, 10.0f,  10.0f,-10.0f, -10.0f
};

GLfloat gNormal[12] = {
	0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f
};

GLfloat gColor[3] = { 0.4f, 0.4f, 0.4f };
GLuint gVAO, gVBO_pos, gVBO_normal, gEBO;

unsigned int sIndex[] = {
	0, 2, 1,
	0, 3, 2
};

unsigned int gIndex[] = {
	0, 1, 2,
	0, 2, 3
};

bool m_toggle = false;

int colorIdx = 0;
float rotLDir = 0.0f;
float rotLY = 0.0f;
float rotCDir = 0.0f;
float rotCY = 0.0f;

void resetAll();
GLvoid InitBufferAll();
void InitCuboids();

glm::vec3 cameraPos = glm::vec3(0.0f, 5.0f, 3.0f);
glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 initLightPos = glm::vec3(1.0f, -4.0f, 1.0f);
glm::vec3 lightPos = initLightPos;

glm::vec3 colors[4] = {
	glm::vec3(1.0f, 1.0f, 1.0f),
	glm::vec3(0.5f, 1.0f, 0.5f),
	glm::vec3(0.5f, 0.25f, 0.75f),
	glm::vec3(0.25f, 0.4f, 0.8f)
};

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

float tankPos[2] = { 0.0f, 0.0f };

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
	// InitBufferAll(); // 주석 처리 또는 삭제
	InitBufferAll();
	InitCuboids();
	srand(time(NULL));
	std::cout << "m: 주변조명을켜기/끄기" << std::endl;
	std::cout << "c:  조명색을다른색으로바뀌도록한다.  3종류의다른색을적용해본다." << std::endl;
	std::cout << "y: 조명이화면중심의 y축기준으로양/음방향으로회전하기(공전)" << std::endl;
	std::cout << "s:  회전멈추기" << std::endl;
	std::cout << "z/Z: 카메라가z축양/음방향으로 이동" << std::endl;
	std::cout << "x/X: 카메라가x축양/음방향으로 이동" << std::endl;
	std::cout << "r/R: 화면의중심의y축에대하여카메라가양/음방향으로회전(중점에대하여공전)" << std::endl;

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
	else if (key == 'm') m_toggle = !m_toggle;
	else if (key == 'c') colorIdx = (colorIdx + 1) % 4;
	else if (key == 'y') rotLDir = 1.0f;
	else if (key == 'Y') rotLDir = -1.0f;
	else if (key == 'r') rotCDir = 1.0f;
	else if (key == 'R') rotCDir = -1.0f;
	else if (key == 'z') cameraPos += 0.1f * glm::normalize(cameraDirection - cameraPos);
	else if (key == 'Z') cameraPos -= 0.1f * glm::normalize(cameraDirection - cameraPos);
	else if (key == 'x') {
		glm::vec3 right = glm::normalize(glm::cross(cameraDirection - cameraPos, cameraUp));
		cameraPos += 0.1f * right;
		cameraDirection += 0.1f * right;
	}
	else if (key == 'X') {
		glm::vec3 right = glm::normalize(glm::cross(cameraDirection - cameraPos, cameraUp));
		cameraPos -= 0.1f * right;
		cameraDirection -= 0.1f * right;
	}
	else if (key == 's') resetAll();
	glutPostRedisplay();
}

void SpecialKeyboard(int key, int x, int y)
{
	if (key == GLUT_KEY_UP)      tankPos[1] -= 0.1f; // -z
	else if (key == GLUT_KEY_DOWN)  tankPos[1] += 0.1f; // +z
	else if (key == GLUT_KEY_LEFT)  tankPos[0] -= 0.1f; // -x
	else if (key == GLUT_KEY_RIGHT) tankPos[0] += 0.1f; // +x

	// 바닥 내부로 위치 클램프(경계 밖 이동 방지)
	const float minX = kFloorMinX + kTankHalfX;
	const float maxX = kFloorMaxX - kTankHalfX;
	const float minZ = (kFloorMinZ + kTankHalfZ) - kTankBaseCenterZ; // tankPos[1] 한계
	const float maxZ = (kFloorMaxZ - kTankHalfZ) - kTankBaseCenterZ;

	if (tankPos[0] < minX) tankPos[0] = minX;
	else if (tankPos[0] > maxX) tankPos[0] = maxX;

	if (tankPos[1] < minZ) tankPos[1] = minZ;
	else if (tankPos[1] > maxZ) tankPos[1] = maxZ;

	glutPostRedisplay();
}

void Mouse(int button, int state, int x, int y) {

}

void make_vertexShaders()
{
	vertexSource = filetobuf("vertex_light.glsl");
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
	fragmentSource = filetobuf("fragment_light.glsl");    // 프래그세이더 읽어오기
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

	// Uniform 변수 설정
	unsigned int lightPosLocation = glGetUniformLocation(shaderProgramID, "lightPos");
	glUniform3fv(lightPosLocation, 1, &lightPos[0]);
	int lightColorLocation = glGetUniformLocation(shaderProgramID, "lightColor");
	glUniform3f(lightColorLocation, colors[colorIdx].x, colors[colorIdx].y, colors[colorIdx].z);
	int objColorLocation = glGetUniformLocation(shaderProgramID, "vColor");
	unsigned int viewPosLocation = glGetUniformLocation(shaderProgramID, "viewPos");
	glUniform3f(viewPosLocation, cameraPos.x, cameraPos.y, cameraPos.z);
	unsigned int useLightingLocation = glGetUniformLocation(shaderProgramID, "lightingEnabled");

	glClearColor(0.1f, 0.1f, 0.1, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glEnable(GL_DEPTH_TEST);

	unsigned int modelLoc = glGetUniformLocation(shaderProgramID, "model");
	unsigned int viewLoc = glGetUniformLocation(shaderProgramID, "view");
	unsigned int projLoc = glGetUniformLocation(shaderProgramID, "projection");
	unsigned int normalMatrixLoc = glGetUniformLocation(shaderProgramID, "normalMatrix");

	// 뷰, 투영 행렬을 먼저 설정
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WinX / (float)WinY, 0.1f, 100.0f);
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glm::mat4 view = glm::lookAt(cameraPos, cameraDirection, cameraUp)
		* glm::rotate(glm::mat4(1.0f), glm::radians(rotCY), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	if (m_toggle) glUniform1i(useLightingLocation, false);
	else glUniform1i(useLightingLocation, true);

	glm::mat4 grMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

	// 바닥 그리기 (기존)
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(grMat));
	glUniform3f(objColorLocation, gColor[0], gColor[1], gColor[2]);
	glBindVertexArray(gVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glm::mat4 moveMat = glm::translate(glm::mat4(1.0f), glm::vec3(tankPos[0], -9.0f, tankPos[1]));

	// 직육면체(탱크 구성 요소) 그리기
	for (int i = 0; i < 8; ++i) {
		glm::mat4 modelMat = moveMat * cuboids[i].mat;
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMat));

		// Normal Matrix 계산 및 전달
		glm::mat3 normalMat = glm::transpose(glm::inverse(glm::mat3(modelMat)));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMat));

		for (int f = 0; f < 6; ++f) {
			glUniform3f(objColorLocation,
				cuboids[i].recs[f].color[0],
				cuboids[i].recs[f].color[1],
				cuboids[i].recs[f].color[2]);
			glBindVertexArray(cuboids[i].recs[f].VAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
	}

	// --- 조명 위치에 작은 직육면체 그리기 ---
	glUniform1i(useLightingLocation, false); // 조명 비활성화
	glUniform3f(objColorLocation, colors[colorIdx].x, colors[colorIdx].y, colors[colorIdx].z); // 조명 색상 (흰색)

	glm::mat4 lightOrbit = glm::mat4(1.0f);
	lightOrbit = glm::rotate(lightOrbit, glm::radians(rotLY), glm::vec3(0.0f, 1.0f, 0.0f));
	lightPos = glm::vec3(lightOrbit * glm::vec4(initLightPos, 1.0f));

	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);
	lightModel = glm::scale(lightModel, glm::vec3(0.2f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(lightModel));

	for (int f = 0; f < 6; ++f) {
		glBindVertexArray(cuboids[mid].recs[f].VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	glutSwapBuffers();
}


GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

void TimerFunction(int value)
{
	if (rotLDir) {
		rotLY += 0.5f * rotLDir;
		if (rotLY >= 360.0f) rotLY -= 360.0f;
		else if (rotLY <= -360.0f) rotLY += 360.0f;
	}

	if (rotCDir) {
		rotCY += 0.5f * rotCDir;
		if (rotCY >= 360.0f) rotCY -= 360.0f;
		else if (rotCY <= -360.0f) rotCY += 360.0f;
	}
	glutPostRedisplay();
	glutTimerFunc(16, TimerFunction, 1);
}

GLvoid InitBufferAll()
{
	glGenVertexArrays(1, &gVAO);
	glBindVertexArray(gVAO);

	glGenBuffers(1, &gVBO_pos);
	glBindBuffer(GL_ARRAY_BUFFER, gVBO_pos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(gPos), gPos, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &gVBO_normal);
	glBindBuffer(GL_ARRAY_BUFFER, gVBO_normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(gNormal), gNormal, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &gEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(gIndex), gIndex, GL_STATIC_DRAW);
}

void resetAll() {
	rotCDir = 0.0f;
	rotLDir = 0.0f;
}

void InitCuboids()
{
	auto setFace = [&](mRec& r, const float* posSrc, const float* normalSrc, const GLfloat col[3]) {
		for (int k = 0; k < 12; ++k) {
			r.pos[k] = posSrc[k];
			r.normal[k] = normalSrc[k];
		}
		r.color[0] = col[0];
		r.color[1] = col[1];
		r.color[2] = col[2];

		glGenVertexArrays(1, &r.VAO);
		glBindVertexArray(r.VAO);

		// 위치 VBO
		glGenBuffers(1, &r.VBO_pos);
		glBindBuffer(GL_ARRAY_BUFFER, r.VBO_pos);
		glBufferData(GL_ARRAY_BUFFER, sizeof(r.pos), r.pos, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		// 법선 VBO
		glGenBuffers(1, &r.VBO_normal);
		glBindBuffer(GL_ARRAY_BUFFER, r.VBO_normal);
		glBufferData(GL_ARRAY_BUFFER, sizeof(r.normal), r.normal, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);

		// EBO
		glGenBuffers(1, &r.EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, r.EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sIndex), sIndex, GL_STATIC_DRAW);
		};

	// 색상 정의
	const GLfloat colBase[3] = { 0.30f, 0.30f, 0.30f }; // 바닥(탱크 하부)
	const GLfloat colMid[3] = { 0.60f, 0.60f, 0.60f }; // 중앙 연결 몸체
	const GLfloat colBody[3] = { 0.30f, 0.70f, 0.30f }; // 상부 몸체(터렛)
	const GLfloat colGun[3] = { 0.85f, 0.85f, 0.20f }; // 포신
	const GLfloat colFlag[3] = { 0.95f, 0.70f, 0.20f }; // 깃대

	// 각 cuboid의 6면 VAO 생성
	for (int i = 0; i < 8; ++i) {
		for (int f = 0; f < 6; ++f) {
			const GLfloat* chosen =
				(i == bot) ? colBase :
				(i == mid) ? colMid :
				(i == top1 || i == top2) ? colBody :
				(i == gun1 || i == gun2) ? colGun :
				/* flag1 flag2 */          colFlag;
			setFace(cuboids[i].recs[f], recPos[f], recNormal[f], chosen);
		}
		cuboids[i].mat = glm::mat4(1.0f);
	}

	// 변환 설정 (단위 큐브 기반: -0.5~0.5 범위를 스케일)
	// 순서: Translate * Rotate * Scale
	// 1. 하부(넓은 탱크 바닥)
	cuboids[bot].mat =
		glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, -5.0f)) *
		glm::scale(glm::mat4(1.0f), glm::vec3(6.0f, 1.0f, 3.0f));

	// 2. 중앙 몸체(얇은 플랫폼)
	cuboids[mid].mat =
		glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.25f, -5.0f)) *
		glm::scale(glm::mat4(1.0f), glm::vec3(4.0f, 0.5f, 1.2f));

	// 3. 좌측 상부 몸체(터렛)
	cuboids[top1].mat =
		glm::translate(glm::mat4(1.0f), glm::vec3(-1.5f, 1.0f, -5.0f)) *
		glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.0f, 1.2f));

	// 4. 우측 상부 몸체(터렛)
	cuboids[top2].mat =
		glm::translate(glm::mat4(1.0f), glm::vec3(1.5f, 1.0f, -5.0f)) *
		glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.0f, 1.2f));

	// 5. 좌측 포신 (약간 아래 위치, 전방(Z-)으로 돌출)
	cuboids[gun1].mat =
		glm::translate(glm::mat4(1.0f), glm::vec3(-1.5f, 1.0f, -4.3f)) *
		glm::scale(glm::mat4(1.0f), glm::vec3(0.3f, 0.3f, 1.5f));

	// 6. 우측 포신
	cuboids[gun2].mat =
		glm::translate(glm::mat4(1.0f), glm::vec3(1.5f, 1.0f, -4.3f)) *
		glm::scale(glm::mat4(1.0f), glm::vec3(0.3f, 0.3f, 1.5f));

	// 7. 좌측 깃대
	cuboids[flag1].mat =
		glm::translate(glm::mat4(1.0f), glm::vec3(-1.5f, 2.3f, -5.0f)) *
		glm::scale(glm::mat4(1.0f), glm::vec3(0.3f, 1.5f, 0.3f));

	// 8. 우측 깃대
	cuboids[flag2].mat =
		glm::translate(glm::mat4(1.0f), glm::vec3(1.5f, 2.3f, -5.0f)) *
		glm::scale(glm::mat4(1.0f), glm::vec3(0.3f, 1.5f, 0.3f));
}