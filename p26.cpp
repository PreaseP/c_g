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

bool m_toggle = false;

float rotDir = 0.0f;
float rotY = 0.0f;

// 구 렌더링용 전역
GLuint sphereVAO = 0, sphereVBO = 0, sphereEBO = 0;
GLsizei sphereIndexCount = 0;

// 구 버텍스/인덱스 생성 함수 선언
void BuildSphere(float radius, int stacks, int slices, std::vector<float>& vertices, std::vector<unsigned int>& indices);

void resetAll();
GLvoid InitBufferAll();
void InitBuffer();

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 7.0f);
glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 initLightPos = glm::vec3(0.0f, 0.0f, 2.0f);
glm::vec3 lightPos = initLightPos;

glm::vec3 colors[3] = {
	glm::vec3(1.0f, 1.0f, 1.0f),
	glm::vec3(0.5f, 1.0f, 0.5f),
	glm::vec3(0.5f, 0.25f, 0.75f)
};

int colorIdx = 0;

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
	else if (key == 'c') colorIdx = (colorIdx + 1) % 3;
	else if (key == 'r') rotDir = 1.0f;
	else if (key == 'R') rotDir = -1.0f;
	
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
	glUniform3f(lightColorLocation, colors[colorIdx].x, colors[colorIdx].y, colors[colorIdx].z);
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

	if (m_toggle) glUniform1i(useLightingLocation, false);
	else glUniform1i(useLightingLocation, true);

	glm::mat4 model = glm::mat4(1.0f);

	for (int i = 0; i < 3; ++i) {
		

		model = glm::mat4(1.0f);
		if (i == 0) {
			glUniform3f(objColorLocation, 1.0f, 0.0f, 0.0f); // 구 색상
			model = glm::scale(model, glm::vec3(1.0f)); // 구 크기 조절
		}
		else
		if (i == 1) {
			glUniform3f(objColorLocation, 0.0f, 1.0f, 0.0f); // 구 색상
			model = glm::translate(model, glm::vec3(-3.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.5f)); // 구 크기 조절
		}
		else {
			glUniform3f(objColorLocation, 0.0f, 0.0f, 1.0f); // 구 색상
			model = glm::translate(model, glm::vec3(-4.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.25f)); // 구 크기 조절
		}
		
		glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

		glBindVertexArray(sphereVAO);
		glDrawElements(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, 0);
	}
	// --- 조명 위치에 작은 구 그리기 ---
	glUniform1i(useLightingLocation, false); // 조명 비활성화
	glUniform3f(objColorLocation, colors[colorIdx].x, colors[colorIdx].y, colors[colorIdx].z); // 조명 색상 (흰색)

	glm::mat4 lightOrbit = glm::mat4(1.0f);
	lightOrbit = glm::rotate(lightOrbit, glm::radians(rotY), glm::vec3(0.0f, 1.0f, 0.0f));
	lightPos = glm::vec3(lightOrbit * glm::vec4(initLightPos, 1.0f));

	model = glm::mat4(1.0f);
	model = glm::translate(model, lightPos);
	model = glm::scale(model, glm::vec3(0.05f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glDrawElements(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);

	glutSwapBuffers();
}


GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

void TimerFunction(int value)
{
	if (rotDir) {
		rotY += 0.5f * rotDir;
		if (rotY >= 360.0f) rotY -= 360.0f;
		else if (rotY <= -360.0f) rotY += 360.0f;
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