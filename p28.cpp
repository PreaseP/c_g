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
} mTri;

typedef struct REC {
	GLfloat pos[12];
	GLfloat color[3];
	GLuint VAO, VBO, EBO;
} mRec;

typedef struct SNOW {
	GLfloat pos[3];
	float speed;
} mSnow;

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
	{ 0.0f,  0.5f,  0.0f,   -0.5f, -0.5f,  0.5f,    0.5f, -0.5f,  0.5f },
	// Right
	{ 0.0f,  0.5f,  0.0f,    0.5f, -0.5f, 0.5f,    0.5f, -0.5f,  -0.5f },
	// Back
	{ 0.0f,  0.5f,  0.0f,   0.5f, -0.5f, -0.5f,    -0.5f, -0.5f, -0.5f },
	// Left
	{ 0.0f,  0.5f,  0.0f,   -0.5f, -0.5f,  -0.5f,   -0.5f, -0.5f, 0.5f }
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

// 각 면에 대한 법선 벡터 추가
float recNormal[6][3] = {
	{ 0.0f, -1.0f,  0.0f}, // Bottom
	{ 0.0f,  0.0f, -1.0f}, // Front
	{ 1.0f,  0.0f,  0.0f}, // Right
	{ 0.0f,  0.0f,  1.0f}, // Back
	{-1.0f,  0.0f,  0.0f}, // Left
	{ 0.0f,  1.0f,  0.0f}  // Top
};

// 피라미드 각 면에 대한 법선 벡터 추가 (계산된 값)
float triNormal[4][3] = {
	{ 0.0f, 0.447214f, 0.894427f }, // Front
	{ 0.894427f, 0.447214f, 0.0f }, // Right
	{ 0.0f, 0.447214f, -0.894427f }, // Back
	{ -0.894427f, 0.447214f, 0.0f }  // Left
};

unsigned int sIndex[] = {
   0, 1, 2
};

unsigned int rIndex[] = {
   0, 2, 1,
   0, 3, 2
};

// 구 렌더링용 전역
GLuint sphereVAO = 0, sphereVBO = 0, sphereEBO = 0;
GLsizei sphereIndexCount = 0;

// 구 버텍스/인덱스 생성 함수 선언
void BuildSphere(float radius, int stacks, int slices, std::vector<float>& vertices, std::vector<unsigned int>& indices);

mLine lines[3];
mTri tris[4];
mRec recs[6];
mSnow snows[300];

GLuint orbitVAO[2] = { 0, 0 };
GLuint orbitVBO[2] = { 0, 0 };
GLsizei orbitVertexCount[2] = { 0, 0 };

float rotLY = 0.0f;
float spRot = 0.0f;

float rotDir = 0.0f;

float spSize[3] = { 0.1f, 0.2f, 0.4f };



float spColors[3][3] = {
	{1.0f, 0.0f, 0.0f},
	{0.0f, 1.0f, 0.0f},
	{0.0f, 0.0f, 1.0f}
};

bool dropping = false;
bool dropToggle = false;

void resetAll();
void BuildCircle(float radius, int segments, std::vector<float>& verts);
GLvoid InitBufferAll();
void InitBuffer();

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 7.0f);
glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 initLightPos = glm::vec3(0.0f, 1.0f, 1.0f);
glm::vec3 lightPos = initLightPos;
float lightIntensity = 1.0f;

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
	// InitBufferAll(); // 주석 처리 또는 삭제
	InitBufferAll();
	srand(time(NULL));

	// snows 초기화
	// 위치와 속도 설정
	// 속도는 0.01 ~ 0.05 사이의 랜덤 값으로 설정
	// 위치는 x, z는 -3.0 ~ 3.0 사이 랜덤 값으로, y는 5.0으로 설정

	for (int i =0; i < 300; i++) {
		snows[i].pos[0] = static_cast<float>(rand() % 1000) / 1000.0f * 6.0f - 3.0f; // x: -3.0 ~ 3.0
		snows[i].pos[1] = 5.0f;
		snows[i].pos[2] = static_cast<float>(rand() % 1000) / 1000.0f * 6.0f - 3.0f; // z: -3.0 ~ 3.0
		snows[i].speed = static_cast<float>(rand() % 50) / 1000.0f + 0.01f; // 속도: 0.01 ~ 0.05
	}

	std::cout << "s: 하늘에서 눈이 내린다./ 멈춘다." << std::endl;
	std::cout << "r: 조명이 화면 중앙의 y축에 대하여 공전한다/멈춘다. " << std::endl;
	std::cout << "n/f: 조명이 가까워지기/멀어지기" << std::endl;
	std::cout << "+/-: 조명의 세기 높아지기/낮춰지기" << std::endl;

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
	else if (key == 'r' || key == 'R') {
		if (!rotDir)
			rotDir = 1.0f;
		else
			rotDir = 0.0f;
	}
	else if (key == 'n') {
		initLightPos.z -= 0.5f;
		initLightPos.y -= 0.5f;
	}
	else if (key == 'f') {
		initLightPos.z += 0.5f;
		initLightPos.y += 0.5f;
	}
	else if (key == '+') {
		lightIntensity += 0.1f;
		if (lightIntensity > 2.0f)
			lightIntensity = 2.0f;
	}
	else if (key == '-') {
		lightIntensity -= 0.1f;
		if (lightIntensity < 0.0f)
			lightIntensity = 0.0f;
	}
	else if (key == 's' || key == 'S') {
		if (!dropToggle) dropToggle = true;
		
		dropping = !dropping;
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
	glUniform3f(lightColorLocation, 1.0f, 1.0f, 1.0f);
	int objColorLocation = glGetUniformLocation(shaderProgramID, "vColor");
	unsigned int viewPosLocation = glGetUniformLocation(shaderProgramID, "viewPos");
	glUniform3f(viewPosLocation, cameraPos.x, cameraPos.y, cameraPos.z);
	unsigned int useLightingLocation = glGetUniformLocation(shaderProgramID, "lightingEnabled");
	unsigned int lightIntensityLocation = glGetUniformLocation(shaderProgramID, "lightIntensity");
	glUniform1f(lightIntensityLocation, lightIntensity);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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

	glm::mat4 view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 cord = glm::mat4(1.0f);
	cord = glm::rotate(cord, glm::radians(25.0f), glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)));

	glUniform1i(useLightingLocation, true);

	glm::mat4 ground = glm::mat4(1.0f);
	ground = glm::scale(ground, glm::vec3(7.0f, 0.0f, 7.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(cord * ground));
	glUniform3f(objColorLocation, 0.3f, 0.3f, 0.3f);
	glBindVertexArray(recs[r_TOP].VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(2.0f, 1.0f, 2.0f));

	glm::mat4 finalModel = cord * model;
	glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(finalModel)));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(finalModel));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	// 사각뿔 그리기
	for (int i = 0; i < 4; ++i) {
		glUniform3f(objColorLocation, tris[i].color[0], tris[i].color[1], tris[i].color[2]);
		glBindVertexArray(tris[i].VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}
	// 사각뿔 바닥 그리기 (recs[r_BOTTOM] 재사용)
	glUniform3f(objColorLocation, recs[r_BOTTOM].color[0], recs[r_BOTTOM].color[1], recs[r_BOTTOM].color[2]);
	glBindVertexArray(recs[r_BOTTOM].VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glm::mat4 spCord[3];

	spCord[0] = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 1.0f));
	spCord[1] = glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 1.0f));
	spCord[2] = glm::rotate(glm::mat4(1.0f), glm::radians(15.0f), glm::vec3(1.0f, 1.0f, 0.0f));

	// 구 그리기
	for (int i = 0; i < 3; ++i) {

		model = glm::mat4(1.0f);
		glUniform3f(objColorLocation, spColors[i][0], spColors[i][1], spColors[i][2]); // 구 색상
		model = glm::rotate(model, glm::radians(spRot), glm::vec3(1.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(1.0f, 1.0f, -1.0f));
		model = glm::scale(model, glm::vec3(spSize[i])); // 구 크기 조절
		model = cord * spCord[i] * model;

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(sphereVAO);
		glDrawElements(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, 0);
	}

	if (dropToggle) {
		for (int i =0; i < 300; i++) {
			model = glm::mat4(1.0f);
			glUniform3f(objColorLocation, 1.0f, 1.0f, 1.0f);
			model = glm::translate(model, glm::vec3(snows[i].pos[0], snows[i].pos[1], snows[i].pos[2]));
			model = glm::scale(model, glm::vec3(0.02f));
			model = cord * model;
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

			glBindVertexArray(sphereVAO);
			glDrawElements(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, 0);
		}
	}

	glm::mat4 lightOrbit = glm::mat4(1.0f);
	lightOrbit = glm::rotate(lightOrbit, glm::radians(rotLY), glm::vec3(0.0f, 1.0f, 0.0f));
	lightPos = glm::vec3(lightOrbit * glm::vec4(initLightPos, 1.0f));

	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, glm::vec3(lightPos.x, lightPos.y, lightPos.z));
	lightModel = glm::scale(lightModel, glm::vec3(0.2f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(cord * lightModel));

	glUniform3f(objColorLocation, 1.0f, 1.0f, 1.0f);
	for (int i = 0; i < 6; ++i) {
		glBindVertexArray(recs[i].VAO);
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
	if (rotDir) {
		rotLY += rotDir * 1.0f;
		if (rotLY > 360.0f) rotLY -= 360.0f;
		else if (rotLY < -360.0f) rotLY += 360.0f;
	}

	spRot += 1.0f;
	if (spRot > 360.0f) spRot -= 360.0f;

	if (dropping) {
		for (int i = 0; i < 300; i++) {
			snows[i].pos[1] -= snows[i].speed;
			if (snows[i].pos[1] < 0.0f) snows[i].pos[1] =  5.0f;
		}
	}

	glutPostRedisplay();
	glutTimerFunc(16, TimerFunction, 1);
}

GLvoid InitBufferAll()
{
	// 축(lines) 초기화
	for (int i = 0; i < 3; ++i) {
		// 각 축에 색상 할당 (X:빨강, Y:초록, Z:파랑)
		lines[i].color[0] = (i == 0) ? 1.0f : 0.0f;
		lines[i].color[1] = (i == 1) ? 1.0f : 0.0f;
		lines[i].color[2] = (i == 2) ? 1.0f : 0.0f;

		glGenVertexArrays(1, &lines[i].VAO);
		glGenBuffers(1, &lines[i].VBO);

		glBindVertexArray(lines[i].VAO);
		glBindBuffer(GL_ARRAY_BUFFER, lines[i].VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(linePos[i]), linePos[i], GL_STATIC_DRAW);

		// 위치 속성만 설정 (법선 없음)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	}

	// 정육면체(recs) 초기화
	for (int i = 0; i < 6; ++i) {
		recs[i].color[0] = 0.0f; // 색상 고정 (예: 주황색)
		recs[i].color[1] = 1.0f;
		recs[i].color[2] = 0.0f;

		glGenVertexArrays(1, &recs[i].VAO);
		glGenBuffers(1, &recs[i].VBO);
		glGenBuffers(1, &recs[i].EBO);

		// 위치와 법선 데이터를 합친 새로운 버텍스 배열 생성
		GLfloat recVertices[4 * 6]; // 4개의 정점, 각 정점은 위치(3) + 법선(3) = 6개의 float
		for (int j = 0; j < 4; ++j) {
			recVertices[j * 6 + 0] = recPos[i][j * 3 + 0];
			recVertices[j * 6 + 1] = recPos[i][j * 3 + 1];
			recVertices[j * 6 + 2] = recPos[i][j * 3 + 2];
			recVertices[j * 6 + 3] = recNormal[i][0];
			recVertices[j * 6 + 4] = recNormal[i][1];
			recVertices[j * 6 + 5] = recNormal[i][2];
		}

		glBindVertexArray(recs[i].VAO);
		glBindBuffer(GL_ARRAY_BUFFER, recs[i].VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(recVertices), recVertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, recs[i].EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(rIndex), rIndex, GL_STATIC_DRAW);

		// 위치 속성 (location = 0)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// 법선 속성 (location = 1)
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
	}

	// 사각뿔(tris) 초기화
	for (int i = 0; i < 4; ++i) {
		tris[i].color[0] = 0.0f; // 색상 고정 (예: 청록색)
		tris[i].color[1] = 1.0f;
		tris[i].color[2] = 0.0f;

		glGenVertexArrays(1, &tris[i].VAO);
		glGenBuffers(1, &tris[i].VBO);
		glGenBuffers(1, &tris[i].EBO);

		// 위치와 법선 데이터를 합친 새로운 버텍스 배열 생성
		GLfloat triVertices[3 * 6]; // 3개의 정점, 각 정점은 위치(3) + 법선(3) = 6개의 float
		for (int j = 0; j < 3; ++j) {
			triVertices[j * 6 + 0] = triPos[i][j * 3 + 0];
			triVertices[j * 6 + 1] = triPos[i][j * 3 + 1];
			triVertices[j * 6 + 2] = triPos[i][j * 3 + 2];
			triVertices[j * 6 + 3] = triNormal[i][0];
			triVertices[j * 6 + 4] = triNormal[i][1];
			triVertices[j * 6 + 5] = triNormal[i][2];
		}

		glBindVertexArray(tris[i].VAO);
		glBindBuffer(GL_ARRAY_BUFFER, tris[i].VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(triVertices), triVertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tris[i].EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sIndex), sIndex, GL_STATIC_DRAW);

		// 위치 속성 (location = 0)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// 법선 속성 (location = 1)
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
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

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	// 버텍스 속성 설정 (위치 + 법선)
	GLsizei stride = 6 * sizeof(float);
	// 위치 속성 (location = 0)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);
	// 법선 속성 (location = 1)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	// 궤도(원) 생성: segments는 필요에 따라 조절(예: 128)
	for (int i = 0; i < 2; ++i) {
		std::vector<float> cverts;
		BuildCircle(1.0f, 128, cverts);
		orbitVertexCount[i] = (GLsizei)(cverts.size() / 3);

		glGenVertexArrays(1, &orbitVAO[i]);
		glGenBuffers(1, &orbitVBO[i]);

		glBindVertexArray(orbitVAO[i]);
		glBindBuffer(GL_ARRAY_BUFFER, orbitVBO[i]);
		glBufferData(GL_ARRAY_BUFFER, cverts.size() * sizeof(float), cverts.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(0);
		glBindVertexArray(0);
	}
}

void BuildCircle(float radius, int segments, std::vector<float>& verts)
{
	const float PI = 3.14159265358979323846f;
	verts.clear();
	verts.reserve((size_t)segments * 3);
	for (int i = 0; i < segments; ++i) {
		float t = (float)i / (float)segments;
		float ang = t * 2.0f * PI;
		float x = radius * std::cos(ang);
		float z = radius * std::sin(ang);
		verts.push_back(x);
		verts.push_back(0.0f); // XZ 평면(y=0)에서 시작
		verts.push_back(z);
	}
}

void BuildSphere(float radius, int stacks, int slices, std::vector<float>& vertices, std::vector<unsigned int>& indices)
{
	const float PI = 3.14159265358979323846f;
	vertices.clear();
	indices.clear();
	// 정점 데이터는 위치(3) + 법선(3) = 6개의 float 값을 가집니다.
	vertices.reserve((size_t)(stacks + 1) * (slices + 1) * 6);

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

			glm::vec3 pos = glm::vec3(x, y, z);
			glm::vec3 normal = glm::normalize(pos);

			// 위치와 법선 데이터를 순서대로 저장
			vertices.push_back(pos.x);
			vertices.push_back(pos.y);
			vertices.push_back(pos.z);
			vertices.push_back(normal.x);
			vertices.push_back(normal.y);
			vertices.push_back(normal.z);
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

void InitBuffer() {

}

void resetAll() {

}