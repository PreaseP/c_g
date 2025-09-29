#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <vector>

#define WinX 800
#define WinY 600

#define POINT 1
#define LINE 2
#define TRIANGLE 3
#define RECTANGLE 4

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

float cross(float x1, float y1, float x2, float y2) {
	return x1 * y2 - y1 * x2;
}

typedef struct _SHAPE {
	std::vector<GLfloat> vPosition;
	std::vector<GLuint> vIndex;
	GLfloat color[3];
	int mode;
	GLuint VAO, VBO, EBO;
} Shape;

bool insideTriangle(const Shape& s, float mx, float my);
bool nearLine(const Shape& s, float mx, float my, float eps);
bool isInside(const Shape& s, float mx, float my);
void Mouse(int button, int state, int x, int y);
void Keyboard(unsigned char key, int x, int y);
void Motion(int x, int y);

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

GLuint VAO;

std::vector<Shape> shapes;

int sel = -1;

GLvoid InitBuffer();
GLvoid InitBuffer(Shape& s);

float vPositionList[] = {
	0.5f, 0.5f, 0.0f,
	0.5f, -0.5f, 0.0f,
	-0.5f, -0.5f, 0.0f,
	-0.5f, 0.5f, 0.0f
};

unsigned int index[] = {
	0, 1, 3,
	1, 2, 3
};

GLchar* vertexSource, * fragmentSource;

//--- 메인 함수
void main(int argc, char** argv)
{
	//--- 윈도우생성하기

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Example1");

	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	glewInit();

	//--- 세이더읽어와서세이더프로그램만들기: 사용자정의함수호출
	make_shaderProgram();
	InitBuffer();

	//--- 세이더프로그램만들기
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(Mouse);
	glutMainLoop();
}

void Keyboard(unsigned char key, int x, int y)
{	
	GLfloat mx = c2GLx(x);
	GLfloat my = c2GLy(y);

	if (key == 'q') {
		exit(0);
	}
	else if (key == 'p') {
		if (shapes.size() >= 10) return;
		Shape s;
		s.mode = POINT;
		s.vPosition.push_back(c2GLx(x - 10));
		s.vPosition.push_back(c2GLy(y + 10));
		s.vPosition.push_back(0.0f);
		s.vPosition.push_back(c2GLx(x - 10));
		s.vPosition.push_back(c2GLy(y - 10));
		s.vPosition.push_back(0.0f);
		s.vPosition.push_back(c2GLx(x + 10));
		s.vPosition.push_back(c2GLy(y - 10));
		s.vPosition.push_back(0.0f);
		s.vPosition.push_back(c2GLx(x + 10));
		s.vPosition.push_back(c2GLy(y + 10));
		s.vPosition.push_back(0.0f);
		s.vIndex.push_back(0);
		s.vIndex.push_back(1);
		s.vIndex.push_back(2);
		s.vIndex.push_back(0);
		s.vIndex.push_back(2);
		s.vIndex.push_back(3);
		s.color[0] = c2GLrgb(rand() % 256);
		s.color[1] = c2GLrgb(rand() % 256);
		s.color[2] = c2GLrgb(rand() % 256);
		InitBuffer(s);
		shapes.push_back(s);
	}
	else if (key == 'l') {
		if (shapes.size() >= 10) return;
		Shape s;
		s.mode = LINE;
		s.vPosition.push_back(c2GLx(x));
		s.vPosition.push_back(c2GLy(y + 50));
		s.vPosition.push_back(0.0f);
		s.vPosition.push_back(c2GLx(x));
		s.vPosition.push_back(c2GLy(y - 50));
		s.vPosition.push_back(0.0f);
		s.vIndex.push_back(0);
		s.vIndex.push_back(1);
		s.color[0] = c2GLrgb(rand() % 256);
		s.color[1] = c2GLrgb(rand() % 256);
		s.color[2] = c2GLrgb(rand() % 256);
		InitBuffer(s);
		shapes.push_back(s);
	}
	else if (key == 't') {
		if (shapes.size() >= 10) return;
		Shape s;
		s.mode = TRIANGLE;
		s.vPosition.push_back(c2GLx(x - 50));
		s.vPosition.push_back(c2GLy(y + 30));
		s.vPosition.push_back(0.0f);
		s.vPosition.push_back(c2GLx(x + 50));
		s.vPosition.push_back(c2GLy(y + 30));
		s.vPosition.push_back(0.0f);
		s.vPosition.push_back(c2GLx(x));
		s.vPosition.push_back(c2GLy(y - 40));
		s.vPosition.push_back(0.0f);
		s.vIndex.push_back(0);
		s.vIndex.push_back(1);
		s.vIndex.push_back(2);
		s.color[0] = c2GLrgb(rand() % 256);
		s.color[1] = c2GLrgb(rand() % 256);
		s.color[2] = c2GLrgb(rand() % 256);
		InitBuffer(s);
		shapes.push_back(s);
	}
	else if (key == 'r') {
		if (shapes.size() >= 10) return;
		Shape s;
		s.mode = RECTANGLE;
		s.vPosition.push_back(c2GLx(x - 50));
		s.vPosition.push_back(c2GLy(y + 50));
		s.vPosition.push_back(0.0f);
		s.vPosition.push_back(c2GLx(x - 50));
		s.vPosition.push_back(c2GLy(y - 50));
		s.vPosition.push_back(0.0f);
		s.vPosition.push_back(c2GLx(x + 50));
		s.vPosition.push_back(c2GLy(y - 50));
		s.vPosition.push_back(0.0f);
		s.vPosition.push_back(c2GLx(x + 50));
		s.vPosition.push_back(c2GLy(y + 50));
		s.vPosition.push_back(0.0f);
		s.vIndex.push_back(0);
		s.vIndex.push_back(1);
		s.vIndex.push_back(2);
		s.vIndex.push_back(0);
		s.vIndex.push_back(2);
		s.vIndex.push_back(3);
		s.color[0] = c2GLrgb(rand() % 256);
		s.color[1] = c2GLrgb(rand() % 256);
		s.color[2] = c2GLrgb(rand() % 256);
		InitBuffer(s);
		shapes.push_back(s);
	}
	else if (key == 'w') {
		if (sel >= 0) {
			switch (shapes[sel].mode) {
				case TRIANGLE:
					shapes[sel].vPosition[1] += 0.01f;
					shapes[sel].vPosition[4] += 0.01f;
					shapes[sel].vPosition[7] += 0.01f;
					break;
				case LINE:
					shapes[sel].vPosition[1] += 0.01f;
					shapes[sel].vPosition[4] += 0.01f;
					break;
				default:
					shapes[sel].vPosition[1] += 0.01f;
					shapes[sel].vPosition[4] += 0.01f;
					shapes[sel].vPosition[7] += 0.01f;
					shapes[sel].vPosition[10] += 0.01f;
					break;
			}
			InitBuffer(shapes[sel]);
		}
	}
	else if (key == 'a') {
		if (sel >= 0) {
			switch (shapes[sel].mode) {
			case TRIANGLE:
				shapes[sel].vPosition[0] -= 0.01f;
				shapes[sel].vPosition[3] -= 0.01f;
				shapes[sel].vPosition[6] -= 0.01f;
				break;
			case LINE:
				shapes[sel].vPosition[0] -= 0.01f;
				shapes[sel].vPosition[3] -= 0.01f;
				break;
			default:
				shapes[sel].vPosition[0] -= 0.01f;
				shapes[sel].vPosition[3] -= 0.01f;
				shapes[sel].vPosition[6] -= 0.01f;
				shapes[sel].vPosition[9] -= 0.01f;
				break;
			}
			InitBuffer(shapes[sel]);
		}
	}
	else if (key == 's') {
		if (sel >= 0) {
			switch (shapes[sel].mode) {
			case TRIANGLE:
				shapes[sel].vPosition[1] -= 0.01f;
				shapes[sel].vPosition[4] -= 0.01f;
				shapes[sel].vPosition[7] -= 0.01f;
				break;
			case LINE:
				shapes[sel].vPosition[1] -= 0.01f;
				shapes[sel].vPosition[4] -= 0.01f;
				break;
			default:
				shapes[sel].vPosition[1] -= 0.01f;
				shapes[sel].vPosition[4] -= 0.01f;
				shapes[sel].vPosition[7] -= 0.01f;
				shapes[sel].vPosition[10] -= 0.01f;
				break;
			}
			InitBuffer(shapes[sel]);
		}
	}
	else if (key == 'd') {
		if (sel >= 0) {
			switch (shapes[sel].mode) {
			case TRIANGLE:
				shapes[sel].vPosition[0] += 0.01f;
				shapes[sel].vPosition[3] += 0.01f;
				shapes[sel].vPosition[6] += 0.01f;
				break;
			case LINE:
				shapes[sel].vPosition[0] += 0.01f;
				shapes[sel].vPosition[3] += 0.01f;
				break;
			default:
				shapes[sel].vPosition[0] += 0.01f;
				shapes[sel].vPosition[3] += 0.01f;
				shapes[sel].vPosition[6] += 0.01f;
				shapes[sel].vPosition[9] += 0.01f;
				break;
			}
			InitBuffer(shapes[sel]);
		}
	}
	else if (key == 'u') {
		if (sel >= 0) {
			switch (shapes[sel].mode) {
			case TRIANGLE:
				shapes[sel].vPosition[0] -= 0.01f;
				shapes[sel].vPosition[1] += 0.01f;
				shapes[sel].vPosition[3] -= 0.01f;
				shapes[sel].vPosition[4] += 0.01f;
				shapes[sel].vPosition[6] -= 0.01f;
				shapes[sel].vPosition[7] += 0.01f;
				break;
			case LINE:
				shapes[sel].vPosition[0] -= 0.01f;
				shapes[sel].vPosition[1] += 0.01f;
				shapes[sel].vPosition[3] -= 0.01f;
				shapes[sel].vPosition[4] += 0.01f;
				break;
			default:
				shapes[sel].vPosition[0] -= 0.01f;
				shapes[sel].vPosition[1] += 0.01f;
				shapes[sel].vPosition[3] -= 0.01f;
				shapes[sel].vPosition[4] += 0.01f;
				shapes[sel].vPosition[6] -= 0.01f;
				shapes[sel].vPosition[7] += 0.01f;
				shapes[sel].vPosition[9] -= 0.01f;
				shapes[sel].vPosition[10] += 0.01f;
				break;
			}
			InitBuffer(shapes[sel]);
		}
	}
	else if (key == 'h') {
		if (sel >= 0) {
			switch (shapes[sel].mode) {
			case TRIANGLE:
				shapes[sel].vPosition[0] += 0.01f;
				shapes[sel].vPosition[1] += 0.01f;
				shapes[sel].vPosition[3] += 0.01f;
				shapes[sel].vPosition[4] += 0.01f;
				shapes[sel].vPosition[6] += 0.01f;
				shapes[sel].vPosition[7] += 0.01f;
				break;
			case LINE:
				shapes[sel].vPosition[0] += 0.01f;
				shapes[sel].vPosition[1] += 0.01f;
				shapes[sel].vPosition[3] += 0.01f;
				shapes[sel].vPosition[4] += 0.01f;
				break;
			default:
				shapes[sel].vPosition[0] += 0.01f;
				shapes[sel].vPosition[1] += 0.01f;
				shapes[sel].vPosition[3] += 0.01f;
				shapes[sel].vPosition[4] += 0.01f;
				shapes[sel].vPosition[6] += 0.01f;
				shapes[sel].vPosition[7] += 0.01f;
				shapes[sel].vPosition[9] += 0.01f;
				shapes[sel].vPosition[10] += 0.01f;
				break;
			}
			InitBuffer(shapes[sel]);
		}
	}
	else if (key == 'j') {
		if (sel >= 0) {
			switch (shapes[sel].mode) {
			case TRIANGLE:
				shapes[sel].vPosition[0] -= 0.01f;
				shapes[sel].vPosition[1] -= 0.01f;
				shapes[sel].vPosition[3] -= 0.01f;
				shapes[sel].vPosition[4] -= 0.01f;
				shapes[sel].vPosition[6] -= 0.01f;
				shapes[sel].vPosition[7] -= 0.01f;
				break;
			case LINE:
				shapes[sel].vPosition[0] -= 0.01f;
				shapes[sel].vPosition[1] -= 0.01f;
				shapes[sel].vPosition[3] -= 0.01f;
				shapes[sel].vPosition[4] -= 0.01f;
				break;
			default:
				shapes[sel].vPosition[0] -= 0.01f;
				shapes[sel].vPosition[1] -= 0.01f;
				shapes[sel].vPosition[3] -= 0.01f;
				shapes[sel].vPosition[4] -= 0.01f;
				shapes[sel].vPosition[6] -= 0.01f;
				shapes[sel].vPosition[7] -= 0.01f;
				shapes[sel].vPosition[9] -= 0.01f;
				shapes[sel].vPosition[10] -= 0.01f;
				break;
			}
			InitBuffer(shapes[sel]);
		}
	}
	else if (key == 'k') {
		if (sel >= 0) {
			switch (shapes[sel].mode) {
			case TRIANGLE:
				shapes[sel].vPosition[0] += 0.01f;
				shapes[sel].vPosition[1] -= 0.01f;
				shapes[sel].vPosition[3] += 0.01f;
				shapes[sel].vPosition[4] -= 0.01f;
				shapes[sel].vPosition[6] += 0.01f;
				shapes[sel].vPosition[7] -= 0.01f;
				break;
			case LINE:
				shapes[sel].vPosition[0] += 0.01f;
				shapes[sel].vPosition[1] -= 0.01f;
				shapes[sel].vPosition[3] += 0.01f;
				shapes[sel].vPosition[4] -= 0.01f;
				break;
			default:
				shapes[sel].vPosition[0] += 0.01f;
				shapes[sel].vPosition[1] -= 0.01f;
				shapes[sel].vPosition[3] += 0.01f;
				shapes[sel].vPosition[4] -= 0.01f;
				shapes[sel].vPosition[6] += 0.01f;
				shapes[sel].vPosition[7] -= 0.01f;
				shapes[sel].vPosition[9] += 0.01f;
				shapes[sel].vPosition[10] -= 0.01f;
				break;
			}
			InitBuffer(shapes[sel]);
		}
	}
	else if (key == 'c') {
		shapes.clear();
	}

	drawScene();
}

void Mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		float mx = c2GLx(x);
		float my = c2GLy(y);

		sel = -1;
		for (int i = 0; i < shapes.size(); ++i) {
			if (isInside(shapes[i], mx, my)) {
				sel = i;
				break;
			}
		}
	}
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
	if(!result)
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

	glClearColor(1.0, 1.0, 1.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	int vColor = glGetUniformLocation(shaderProgramID, "vColor");

	glUseProgram(shaderProgramID);
	
	glUniform3f(vColor, 0.0f, 1.0f, 0.0f);
	
	// glBindVertexArray(VAO);
	// glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	
	for (int i=0; i < shapes.size(); ++i) {
		glUniform3f(vColor, shapes[i].color[0], shapes[i].color[1], shapes[i].color[2]);
		glBindVertexArray(shapes[i].VAO);
		switch (shapes[i].mode) {
		default:
			glDrawElements(GL_TRIANGLES, shapes[i].vIndex.size(), GL_UNSIGNED_INT, 0);
			break;
		case LINE:
			glDrawElements(GL_LINES, shapes[i].vIndex.size(), GL_UNSIGNED_INT, 0);
			break;
		}
	}

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

GLvoid InitBuffer() 
{
  GLuint VBO_pos, EBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO_pos);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_pos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vPositionList), vPositionList, GL_STATIC_DRAW);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);
}

GLvoid InitBuffer(Shape& s)
{
	glGenVertexArrays(1, &s.VAO);
	glGenBuffers(1, &s.VBO);
	glGenBuffers(1, &s.EBO);

	glBindVertexArray(s.VAO);

	glBindBuffer(GL_ARRAY_BUFFER, s.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * s.vPosition.size(), s.vPosition.data(), GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s.EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * s.vIndex.size(), s.vIndex.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);
}

bool insideRectangle(const Shape& s, float mx, float my) {
	float xmin = s.vPosition[0], xmax = s.vPosition[0];
	float ymin = s.vPosition[1], ymax = s.vPosition[1];

	for (int i = 0; i < s.vPosition.size(); i += 3) {
		xmin = std::min(xmin, s.vPosition[i]);
		xmax = std::max(xmax, s.vPosition[i]);
		ymin = std::min(ymin, s.vPosition[i + 1]);
		ymax = std::max(ymax, s.vPosition[i + 1]);
	}
	return (mx >= xmin && mx <= xmax && my >= ymin && my <= ymax);
}

bool insideTriangle(const Shape& s, float mx, float my) {
	float x1 = s.vPosition[0], y1 = s.vPosition[1];
	float x2 = s.vPosition[3], y2 = s.vPosition[4];
	float x3 = s.vPosition[6], y3 = s.vPosition[7];

	float c1 = cross(x2 - x1, y2 - y1, mx - x1, my - y1);
	float c2 = cross(x3 - x2, y3 - y2, mx - x2, my - y2);
	float c3 = cross(x1 - x3, y1 - y3, mx - x3, my - y3);

	bool has_neg = (c1 < 0) || (c2 < 0) || (c3 < 0);
	bool has_pos = (c1 > 0) || (c2 > 0) || (c3 > 0);

	return !(has_neg && has_pos);
}

bool nearLine(const Shape& s, float mx, float my, float eps = 0.02f) {
	float x1 = s.vPosition[0], y1 = s.vPosition[1];
	float x2 = s.vPosition[3], y2 = s.vPosition[4];

	float dx = x2 - x1;
	float dy = y2 - y1;
	float len2 = dx * dx + dy * dy;

	float t = ((mx - x1) * dx + (my - y1) * dy) / len2;
	if (t < 0) t = 0;
	else if (t > 1) t = 1;

	float px = x1 + t * dx;
	float py = y1 + t * dy;

	float dist2 = (mx - px) * (mx - px) + (my - py) * (my - py);
	return dist2 <= eps * eps;
}

bool isInside(const Shape& s, float mx, float my) {
	switch (s.mode) {
	case RECTANGLE:
	case POINT:
		return insideRectangle(s, mx, my);
	case TRIANGLE:
		return insideTriangle(s, mx, my);
	case LINE:
		return nearLine(s, mx, my);
	}
	return false;
}