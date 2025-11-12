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

// 구 렌더링용 전역
GLuint sphereVAO = 0, sphereVBO = 0, sphereEBO = 0;
GLsizei sphereIndexCount = 0;

// 구 버텍스/인덱스 생성 함수 선언
void BuildSphere(float radius, int stacks, int slices, std::vector<float>& vertices, std::vector<unsigned int>& indices);

// 궤도 라인(원) 렌더링용
GLuint orbitVAO[2] = { 0, 0 };
GLuint orbitVBO[2] = { 0, 0 };
GLsizei orbitVertexCount[2] = { 0, 0 };

// 원(라인) 버텍스 생성 함수
void BuildCircle(float radius, int segments, std::vector<float>& verts);

typedef struct SPHERE {
	std::vector<float> vertices;
	std::vector<unsigned int> indices;
	GLuint VAO, VBO, EBO;
} mSph;

mSph spheres[2];

void resetAll();

GLvoid InitBufferAll();

glm::vec3 cameraPos = glm::vec3(0.0f, 0.5f, 7.0f);
glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

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

bool toggles[2] = { false, false };

enum toggleType { P_TOGGLE = 0, M_TOGGLE };

float radius[2] = { 3.0f, 1.5f };

GLfloat rotAngle[3] = { 0.0f, 0.0f, 0.0f };

float pos[3] = { 0.0f, 0.0f, 0.0f };

float rotZ = 0.0f;

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
	std::cout << "p/P: 직각투영/원근투영" << std::endl;
	std::cout << "m/M: 솔리드모델/와이어모델" << std::endl;
	std::cout << "w/a/s/d: 위의 도형들을 좌/우/상/하로 이동(x축과y축값이동–객체이동)" << std::endl;
	std::cout << "+/-: 위의 도형들을 앞/뒤로 이동(z축값이동–객체이동)" << std::endl;
	std::cout << "y/Y: 모든 궤도들의 반지름이 커진다/작아진다" << std::endl;
	std::cout << "z/Z: 중심의 구를 제외하고 행성, 달, 궤도가 z축에 대하여 양/음방향으로 일제히 회전" << std::endl;

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
	else if (key == 'm') toggles[M_TOGGLE] = false;
	else if (key == 'M') toggles[M_TOGGLE] = true;
	else if (key == 'p') toggles[P_TOGGLE] = false;
	else if (key == 'P') toggles[P_TOGGLE] = true;
	else if (key == 'w') pos[1] += 0.1f;
	else if (key == 's') pos[1] -= 0.1f;
	else if (key == 'a') pos[0] -= 0.1f;
	else if (key == 'd') pos[0] += 0.1f;
	else if (key == '+') pos[2] += 0.1f;
	else if (key == '-') pos[2] -= 0.1f;
	else if (key == 'y') {
		radius[0] += 0.1f;
		if (radius[0] > 5.0f) radius[0] = 5.0f;
		radius[1] += 0.05f;
		if (radius[1] > 2.5f) radius[1] = 2.5f;
	}
	else if (key == 'Y') {
		radius[0] -= 0.1f;
		if (radius[0] < 1.0f) radius[0] = 1.0f;
		radius[1] -= 0.05f;
		if (radius[1] < 0.5f) radius[1] = 0.5f;
	}
	else if (key == 'Z') {
		rotZ += 5.0f;
		if (rotZ >= 360.0f) rotZ -= 360.0f;
	}
	else if (key == 'z') {
		rotZ -= 5.0f;
		if (rotZ < 0.0f) rotZ += 360.0f;
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
	glUseProgram(shaderProgramID);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);

	if(toggles[M_TOGGLE])
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	unsigned int modelLoc = glGetUniformLocation(shaderProgramID, "model");
	unsigned int viewLoc = glGetUniformLocation(shaderProgramID, "view");
	unsigned int projLoc = glGetUniformLocation(shaderProgramID, "projection");

	glm::mat4 mT = glm::mat4(1.0f);

	glm::mat4 vT = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(vT));

	glm::mat4 pT = glm::mat4(1.0f);

	if (toggles[P_TOGGLE])
		pT = glm::perspective(glm::radians(45.0f), (float)WinX / (float)WinY, 0.1f, 100.0f);
	else
		pT = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 10.0f);

	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pT));

	glClearColor(1.0, 1.0, 1.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	int vColor = glGetUniformLocation(shaderProgramID, "vColor");

	glm::mat4 trnMat[2];

	trnMat[0] = glm::translate(glm::mat4(1.0f), glm::vec3(radius[0], 0.0f, 0.0f));
	trnMat[1] = glm::translate(glm::mat4(1.0f), glm::vec3(radius[1], 0.0f, 0.0f));

	glm::mat4 cord[3];

	cord[0] = glm::translate(glm::mat4(1.0f), glm::vec3(pos[0], pos[1], pos[2]));
	cord[1] = glm::rotate(cord[0], glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 1.0f));
	cord[2] = glm::rotate(cord[0], glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 1.0f));

	for (int i = 0; i < 3; i++) {
		cord[i] = glm::rotate(cord[i], glm::radians(rotZ), glm::vec3(0.0f, 0.0f, 1.0f));
	}

	glm::mat4 rotMatM[3];
	glm::mat4 rotMatS[3];

	for (int i = 0; i < 3; i++) {
		rotMatM[i] = glm::rotate(glm::mat4(1.0f), glm::radians(rotAngle[i]), glm::vec3(0.0f, 1.0f, 0.0f));
		rotMatS[i] = glm::rotate(glm::mat4(1.0f), glm::radians(-rotAngle[i] * 2.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(cord[0] * mT));

	// 구 그리기
	glUniform3f(vColor, 0.0f, 0.0f, 1.0f);
	glBindVertexArray(sphereVAO);
	glDrawElements(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glLineWidth(1.5f);

	for (int i = 0; i < 3; i++) {
		glm::mat4 orbitGreenM = cord[i] * glm::scale(glm::mat4(1.0f), glm::vec3(radius[0], 1.0f, radius[0])) * mT;
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(orbitGreenM));
		glUniform3f(vColor, 0.0f, 0.0f, 0.0f);
		glBindVertexArray(orbitVAO[0]);
		glDrawArrays(GL_LINE_LOOP, 0, orbitVertexCount[0]);
		glBindVertexArray(0);

		glm::mat4 greenModel = cord[i] * rotMatM[i] * trnMat[0] * mT;
		glm::mat4 orbitRedM = greenModel * glm::scale(glm::mat4(1.0f), glm::vec3(radius[1], 1.0f, radius[1]));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(orbitRedM));
		glUniform3f(vColor, 0.0f, 0.0f, 0.0f);
		glBindVertexArray(orbitVAO[1]);
		glDrawArrays(GL_LINE_LOOP, 0, orbitVertexCount[1]);
		glBindVertexArray(0);

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(cord[i] * rotMatM[i] * trnMat[0] * mT));

		glUniform3f(vColor, 0.0f, 1.0f, 0.0f);
		glBindVertexArray(spheres[0].VAO);
		glDrawElements(GL_TRIANGLES, spheres[0].indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(cord[i] * rotMatM[i] * trnMat[0] * rotMatS[i] * trnMat[1] * mT));

		glUniform3f(vColor, 1.0f, 0.0f, 0.0f);
		glBindVertexArray(spheres[1].VAO);
		glDrawElements(GL_TRIANGLES, spheres[1].indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
	glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

void TimerFunction(int value)
{
	for (int i = 0; i < 3; i++) {
		rotAngle[i] += 1.0f + i * 0.3f;
		if (rotAngle[i] >= 360.0f) rotAngle[i] -= 360.0f;
	}

	glutPostRedisplay();
	glutTimerFunc(16, TimerFunction, 1);
}

GLvoid InitBufferAll()
{
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

	for (int i = 0; i < 2; ++i) {
		BuildSphere(0.5f - i * 0.25f, 32, 64, spheres[i].vertices, spheres[i].indices);
		glGenVertexArrays(1, &spheres[i].VAO);
		glGenBuffers(1, &spheres[i].VBO);
		glGenBuffers(1, &spheres[i].EBO);
		glBindVertexArray(spheres[i].VAO);
		glBindBuffer(GL_ARRAY_BUFFER, spheres[i].VBO);
		glBufferData(GL_ARRAY_BUFFER, spheres[i].vertices.size() * sizeof(float), spheres[i].vertices.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, spheres[i].EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, spheres[i].indices.size() * sizeof(unsigned int), spheres[i].indices.data(), GL_STATIC_DRAW);
		glBindVertexArray(0);
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

void resetAll() {
	
}