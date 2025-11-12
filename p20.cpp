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
	GLfloat color[3];
	GLuint VAO, VBO, EBO;
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

GLfloat gColor[3] = { 0.4f, 0.4f, 0.4f };
GLuint gVAO, gVBO, gEBO;

unsigned int sIndex[] = {
	0, 1, 2,
	0, 2, 3
};

void resetAll();

GLvoid InitBufferAll();
void InitCuboids();

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 7.0f);
glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float tankPos[2] = { 0.0f, 0.0f };

bool toggles[6] = { false, false, false, false, false, true };

enum toggleType { T_TOGGLE = 0, L_TOGGLE, G_TOGGLE, P_TOGGLE, A_TOGGLE, O_TOGGLE };

float rotMid = 0.0f;
float rotGun = 0.0f;

float rotFlag = 0.0f;
float rotFlagDir = 1.0f;

float rotC = 0.0f;
float rotCR = 0.0f;

int moveCnt = 0;

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
	InitBufferAll();
	InitCuboids();
	srand(time(NULL));
	std::cout << "←/↑/→/↓: 탱크가 xz평면에서 x축과 z축방향으로 이동한다." << std::endl;
	std::cout << "t: 중앙몸체가 y축에 대하여 회전한다." << std::endl;
	std::cout << "l: 상부몸체가 이동하여 서로 위치를 바꾼다." << std::endl;
	std::cout << "g: 상부 몸체 앞의 포신이 y축에 대하여 회전한다.  양쪽의 포신은 서로 반대방향으로 회전한다." << std::endl;
	std::cout << "p: 상부 몸체 위의 깃대가 x축에 대하여 회전한다.  양쪽의 깃대는 서로 반대방향으로 회전한다." << std::endl;
	std::cout << "z/Z: 카메라가 z축 양/음 방향으로 이동" << std::endl;
	std::cout << "x/X: 카메라가 x축 양/음 방향으로 이동" << std::endl;
	std::cout << "y/Y: 카메라 기준 y축에 대하여 회전(카메라가 제자리에서 자전)" << std::endl;
	std::cout << "r/R: 화면의 중심의 y축에 대하여 카메라가 회전(중점에 대하여 공전)" << std::endl;
	std::cout << "a: 카메라 공전 애니메이션" << std::endl;
	std::cout << "o: 모든 움직임 멈추기" << std::endl;
	std::cout << "c: 모든 움직임이 초기화된다." << std::endl;

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
	else if (key == 'z') {
		cameraPos += 0.1f * glm::normalize(cameraDirection - cameraPos);
	}
	else if (key == 'Z') {
		cameraPos -= 0.1f * glm::normalize(cameraDirection - cameraPos);
	}
	else if (key == 'X') {
		glm::vec3 right = glm::normalize(glm::cross(cameraDirection - cameraPos, cameraUp));
		cameraPos -= 0.1f * right;
		cameraDirection -= 0.1f * right;
	}
	else if (key == 'x') {
		glm::vec3 right = glm::normalize(glm::cross(cameraDirection - cameraPos, cameraUp));
		cameraPos += 0.1f * right;
		cameraDirection += 0.1f * right;
	}
	else if (key == 't') {
		toggles[T_TOGGLE] = !toggles[T_TOGGLE];
	}
	else if (key == 'l') {
		toggles[L_TOGGLE] = !toggles[L_TOGGLE];
		if (toggles[L_TOGGLE]) moveCnt = 10;
		else moveCnt = -10;
	}
	else if (key == 'g') {
		toggles[G_TOGGLE] = !toggles[G_TOGGLE];
	}
	else if (key == 'p') {
		toggles[P_TOGGLE] = !toggles[P_TOGGLE];
	}
	else if (key == 'a') {
		toggles[A_TOGGLE] = !toggles[A_TOGGLE];
	}
	else if (key == 'o') {
		toggles[O_TOGGLE] = !toggles[O_TOGGLE];
	}
	else if (key == 'c') {
		resetAll();
	}
	else if (key == 'y') {
		rotC += 0.5f;
	}
	else if (key == 'Y') {
		rotC -= 0.5f;
	}
	else if (key == 'r') {
		rotCR += 0.5f;
	}
	else if (key == 'R') {
		rotCR -= 0.5f;
	}

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

	glm::mat4 vT = glm::rotate(glm::mat4(1.0f), glm::radians(rotC), glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::lookAt(cameraPos, cameraDirection, cameraUp) *
		glm::rotate(glm::mat4(1.0f), glm::radians(rotCR), glm::vec3(0.0f, 1.0f, 0.0f));

	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(vT));

	glm::mat4 pT = glm::mat4(1.0f);
	pT = glm::perspective(glm::radians(45.0f), (float)WinX / (float)WinY, 0.1f, 100.0f);
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pT));

	glClearColor(1.0, 1.0, 1.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	int vColor = glGetUniformLocation(shaderProgramID, "vColor");

	glm::mat4 grMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 9.0f, 0.0f));

	// 바닥 그리기 (기존)
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(grMat));
	glUniform3f(vColor, gColor[0], gColor[1], gColor[2]);
	glBindVertexArray(gVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glm::mat4 moveMat = glm::translate(glm::mat4(1.0f), glm::vec3(tankPos[0], 0.001f, tankPos[1]));

	glm::mat4 rotMidM = glm::rotate(glm::mat4(1.0f), glm::radians(rotMid), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 rotGunM1 = glm::rotate(glm::mat4(1.0f), glm::radians(rotGun), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 rotGunM2 = glm::rotate(glm::mat4(1.0f), glm::radians(-rotGun), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 rotFlagM1 = glm::rotate(glm::mat4(1.0f), glm::radians(rotFlag), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 rotFlagM2 = glm::rotate(glm::mat4(1.0f), glm::radians(-rotFlag), glm::vec3(1.0f, 0.0f, 0.0f));

	// 직육면체(탱크 구성 요소) 그리기
	for (int i = 0; i < 8; ++i) {
		glm::mat4 modelMat = glm::mat4(1.0f);
		if (i == bot) {
			modelMat = moveMat * cuboids[i].mat;
		}
		else if (i == mid) {
			modelMat = moveMat * cuboids[i].mat * rotMidM;
		}
		else if (i == top1 || i == top2) {
			modelMat = moveMat * cuboids[i].mat;
		}
		else if (i == gun1) {
			// 좌측 상부 몸체(top1)를 기준으로 공전
			glm::mat4 top1Transform = moveMat * cuboids[top1].mat;
			glm::mat4 gun1LocalTransform = glm::inverse(cuboids[top1].mat) * cuboids[gun1].mat;
			modelMat = top1Transform * rotGunM1 * gun1LocalTransform;
		}
		else if (i == gun2) {
			// 우측 상부 몸체(top2)를 기준으로 공전
			glm::mat4 top2Transform = moveMat * cuboids[top2].mat;
			glm::mat4 gun2LocalTransform = glm::inverse(cuboids[top2].mat) * cuboids[gun2].mat;
			modelMat = top2Transform * rotGunM2 * gun2LocalTransform;
		}
		else if (i == flag1) {
			// 좌측 상부 몸체(top1)에 부착
			glm::mat4 top1Transform = moveMat * cuboids[top1].mat;
			glm::mat4 flag1LocalTransform = glm::inverse(cuboids[top1].mat) * cuboids[flag1].mat;
			modelMat = top1Transform * rotFlagM1 * flag1LocalTransform;
		}
		else if (i == flag2) {
			// 우측 상부 몸체(top2)에 부착
			glm::mat4 top2Transform = moveMat * cuboids[top2].mat;
			glm::mat4 flag2LocalTransform = glm::inverse(cuboids[top2].mat) * cuboids[flag2].mat;
			modelMat = top2Transform * rotFlagM2 * flag2LocalTransform;
		}
		else {
			modelMat = moveMat * cuboids[i].mat;
		}

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMat));

		for (int f = 0; f < 6; ++f) {
			glUniform3f(vColor,
				cuboids[i].recs[f].color[0],
				cuboids[i].recs[f].color[1],
				cuboids[i].recs[f].color[2]);
			glBindVertexArray(cuboids[i].recs[f].VAO);
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
	if (toggles[O_TOGGLE]) {
		if (toggles[T_TOGGLE]) {
			rotMid += 0.5f;
			if (rotMid >= 360.0f) rotMid -= 360.0f;
		}

		if (moveCnt) {
			if (moveCnt > 0) {
				cuboids[top1].mat = glm::translate(cuboids[top1].mat, glm::vec3(+0.2f, 0.0f, 0.0f));
				cuboids[top2].mat = glm::translate(cuboids[top2].mat, glm::vec3(-0.2f, 0.0f, 0.0f));
				moveCnt--;
			}
			else if (moveCnt < 0) {
				cuboids[top1].mat = glm::translate(cuboids[top1].mat, glm::vec3(-0.2f, 0.0f, 0.0f));
				cuboids[top2].mat = glm::translate(cuboids[top2].mat, glm::vec3(+0.2f, 0.0f, 0.0f));
				moveCnt++;
			}
		}

		if (toggles[G_TOGGLE]) {
			rotGun += 1.0f;
			if (rotGun >= 360.0f) rotGun -= 360.0f;
		}
		if (toggles[P_TOGGLE]) {
			rotFlag += 1.0f * rotFlagDir;
			if (rotFlag >= 60.0f || rotFlag <= -60.0f) rotFlagDir *= -1.0f;
		}
		if (toggles[A_TOGGLE]) {
			rotCR += 0.5f;
			if (rotFlag >= 360.0f) rotCR -= 360.0f;
		}
	}
	glutPostRedisplay();
	glutTimerFunc(16, TimerFunction, 1);
}

GLvoid InitBufferAll()
{
	glGenVertexArrays(1, &gVAO);
	glGenBuffers(1, &gVBO);

	glBindVertexArray(gVAO);

	glBindBuffer(GL_ARRAY_BUFFER, gVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(gPos), gPos, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &gEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sIndex), sIndex, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
}

void resetAll() {
	tankPos[0] = tankPos[1] = 0.0f;

	for (int i = 0; i < 6; i++) toggles[i] = false;
	toggles[O_TOGGLE] = true;

	rotMid = rotGun = rotFlag = rotC = rotCR = 0.0f;

	moveCnt = 0;
}

// 새 함수 구현: 8개 직육면체(탱크 형태) 생성
void InitCuboids()
{
	auto setFace = [&](mRec& r, const float* src, const GLfloat col[3]) {
		for (int k = 0; k < 12; ++k) r.pos[k] = src[k];
		r.color[0] = col[0];
		r.color[1] = col[1];
		r.color[2] = col[2];

		glGenVertexArrays(1, &r.VAO);
		glGenBuffers(1, &r.VBO);
		glBindVertexArray(r.VAO);

		glBindBuffer(GL_ARRAY_BUFFER, r.VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(r.pos), r.pos, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glGenBuffers(1, &r.EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, r.EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sIndex), sIndex, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
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
			setFace(cuboids[i].recs[f], recPos[f], chosen);
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