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
	{ 0.0f,  0.4472f, -0.8944f }, // Front
	{ 0.8944f, 0.4472f,  0.0f }, // Right
	{ 0.0f,  0.4472f,  0.8944f }, // Back
	{-0.8944f, 0.4472f,  0.0f }  // Left
};

unsigned int sIndex[] = {
   0, 1, 2
};

unsigned int rIndex[] = {
   0, 2, 1,
   0, 3, 2
};

mLine lines[3];
mTri tris[4];
mRec recs[6];

GLuint orbitVAO[2] = { 0, 0 };
GLuint orbitVBO[2] = { 0, 0 };
GLsizei orbitVertexCount[2] = { 0, 0 };

bool n_toggle = false;
bool y_toggle = false;
bool m_toggle = false;

float rotY = 0.0f;
float rotLY = 0.0f;

float rotDir = 0.0f;

void resetAll();
void BuildCircle(float radius, int segments, std::vector<float>& verts);
GLvoid InitBufferAll();
void InitBuffer();

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 7.0f);
glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 initLightPos = glm::vec3(0.0f, 0.0f, 2.0f);
glm::vec3 lightPos = initLightPos;

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
	std::cout << "n: 육면체/사각뿔그리기" << std::endl;
	std::cout << "m: 조명켜기/끄기" << std::endl;
	std::cout << "y: 객체를y축에대하여회전(제자리에서자전)" << std::endl;
	std::cout << "r: 조명을객체의중심y축에대하여양/음방향으로공전시키기" << std::endl;
	std::cout << "z/Z: 조명을객체에가깝게/멀게이동하기" << std::endl;

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
	else if (key == 'n' || key == 'N') {
		n_toggle = !n_toggle;
	}
	else if (key == 'y' || key == 'Y') {
		y_toggle = !y_toggle;
	}
	else if (key == 'r' || key == 'R') {
		if (!rotDir)
			rotDir = 1.0f;
		else
			rotDir *= -1.0f;
	}
	else if (key == 'z') {
		initLightPos.z -= 0.5f;
	}
	else if (key == 'Z') {
		initLightPos.z += 0.5f;
	}
	else if (key == 'm') {
		m_toggle = !m_toggle;
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
	cord = glm::rotate(cord, glm::radians(35.0f), glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)));
	cord = glm::rotate(cord, glm::radians(-5.0f), glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)));
	glUniform1i(useLightingLocation, false);

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(cord));

	for (int i = 0; i < 3; ++i) {
		glUniform3f(objColorLocation, lines[i].color[0], lines[i].color[1], lines[i].color[2]);
		glBindVertexArray(lines[i].VAO);
		glDrawArrays(GL_LINES, 0, 2);
	}

	glm::mat4 orbitModel = glm::mat4(1.0f);
	// 카메라의 z 위치를 반지름으로 사용하여 원의 크기 조절
	orbitModel = glm::scale(orbitModel, glm::vec3(initLightPos.z, 1.0f, initLightPos.z));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(cord * orbitModel));

	glUniform3f(objColorLocation, 1.0f, 1.0f, 1.0f); // 궤도 색상 (흰색)
	glBindVertexArray(orbitVAO[0]);
	glDrawArrays(GL_LINE_LOOP, 0, orbitVertexCount[0]);

	if (m_toggle) glUniform1i(useLightingLocation, false);
	else glUniform1i(useLightingLocation, true);

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(rotY), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 finalModel = cord * model;
	glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(finalModel)));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(finalModel));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	if (!n_toggle) {
		// 1. 정육면체 그리기
		for (int i = 0; i < 6; ++i) {
			glUniform3f(objColorLocation, recs[i].color[0], recs[i].color[1], recs[i].color[2]);
			glBindVertexArray(recs[i].VAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
	}
	else {
		// 2. 사각뿔 그리기
		for (int i = 0; i < 4; ++i) {
			glUniform3f(objColorLocation, tris[i].color[0], tris[i].color[1], tris[i].color[2]);
			glBindVertexArray(tris[i].VAO);
			glDrawArrays(GL_TRIANGLES, 0, 3);
		}
		// 사각뿔 바닥 그리기 (recs[r_BOTTOM] 재사용)
		glUniform3f(objColorLocation, recs[r_BOTTOM].color[0], recs[r_BOTTOM].color[1], recs[r_BOTTOM].color[2]);
		glBindVertexArray(recs[r_BOTTOM].VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	glm::mat4 lightOrbit = glm::mat4(1.0f);
	lightOrbit = glm::rotate(lightOrbit, glm::radians(rotLY), glm::vec3(0.0f, 1.0f, 0.0f));
	lightPos = glm::vec3(lightOrbit * glm::vec4(initLightPos, 1.0f));

	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, glm::vec3(lightPos.x, lightPos.y, lightPos.z));
	lightModel = glm::scale(lightModel, glm::vec3(0.2f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(cord * lightModel * lightOrbit));

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
	if (y_toggle) {
		rotY += 0.5f;
		if (rotY > 360.0f) rotY -= 360.0f;
	}

	if (rotDir) {
		rotLY += rotDir * 1.0f;
		if (rotLY > 360.0f) rotLY -= 360.0f;
		else if (rotLY < -360.0f) rotLY += 360.0f;
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

void InitBuffer() {

}

void resetAll() {

}