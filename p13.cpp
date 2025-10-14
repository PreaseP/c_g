#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <vector>
#include <cmath>
#include <random>

#define WinX 800
#define WinY 600

enum moveDir {
	STOP = 0, 
	LEFT, RIGHT,
	LU, LD, RU, RD
};

enum ShapeType
{
	S_POINT = 1,
	S_LINE,
	S_TRI,
	S_REC,
	S_PEN
};


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

typedef struct _SHAPE {
	GLfloat mid[2];
	GLfloat vPos[15];
	GLfloat RGB[3];
	GLuint VAO, VBO, EBO;
	int shapeType;
	int move;
} Shape;

void Mouse(int button, int state, int x, int y);
void Keyboard(unsigned char key, int x, int y);
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

GLuint VAO;

Shape shapes[15];

GLvoid InitBuffer();
GLvoid InitBuffer(Shape& s);
GLvoid UpdateShape(Shape& s);
GLvoid SetShapes(Shape s[]);
GLvoid SetShapeCord(Shape& s);

float vPositionList[] = {
	-1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, -1.0f, 0.0f
};

float linePos[] = {
	0.05f, 0.05f, 0.0f,
	-0.05f, -0.05f, 0.0f
};

float triPos[] = {
	0.0f, 0.05f, 0.0f,
	-0.05f, -0.05f, 0.0f,
	0.05f, -0.05f, 0.0f
};

float recPos[] = {
	-0.05f, 0.05f, 0.0f,
	-0.05f, -0.05f, 0.0f,
	0.05f, -0.05f, 0.0f,
	0.05f, 0.05f, 0.0f
};

float penPos[] = {
	0.0f, 0.05f, 0.0f,
	-0.05f, 0.01f, 0.0f,
	-0.03f, -0.05f, 0.0f,
	0.03f, -0.05f, 0.0f,
	0.05f, 0.01f, 0.0f
};

unsigned int index[] = {
	0, 1, 2, 3
};

GLuint vIndex[] = {
	0, 1, 2,
	0, 2, 3,
	0, 3, 4
};

GLchar* vertexSource, * fragmentSource;

int sel = -1;

bool s_toggle = false;

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
	SetShapes(shapes);

	//--- 세이더프로그램만들기
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutTimerFunc(16, TimerFunction, 1);
	glutMainLoop();
}

void Keyboard(unsigned char key, int x, int y)
{
	if(key == 'q') exit(0);
	else if (key == 'c') {
		SetShapes(shapes);
	}
	else if (key == 's') {
		if(s_toggle) s_toggle = false;
		else s_toggle = true;
	}
	drawScene();
}

void Mouse(int button, int state, int x, int y) {

	float mx = c2GLx(x);
	float my = c2GLy(y);

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		for (int i= 0; i < 15; ++i) {
			if (mx >= shapes[i].mid[0] - 0.05f && mx <= shapes[i].mid[0] + 0.05f &&
				my >= shapes[i].mid[1] - 0.05f && my <= shapes[i].mid[1] + 0.05f) {
				sel = i;
				shapes[sel].move = STOP;
				break;
			}
		}
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		if (sel >= 0) {
			sel = -1;
		}
	}
}

void Motion(int x, int y)
{
	float mx = c2GLx(x);
	float my = c2GLy(y);

	if (sel >= 0) {
		shapes[sel].mid[0] = mx;
		shapes[sel].mid[1] = my;

		SetShapeCord(shapes[sel]);

		for (int i =0; i< 15; ++i) {
			if (i == sel || shapes[i].shapeType == 0) continue;

			if (mx >= shapes[i].mid[0] - 0.05f && mx <= shapes[i].mid[0] + 0.05f &&
				my >= shapes[i].mid[1] - 0.05f && my <= shapes[i].mid[1] + 0.05f) {
				shapes[sel].move = rand() % 6 + LEFT;

				shapes[sel].shapeType = shapes[sel].shapeType + shapes[i].shapeType;
				shapes[i].shapeType = 0;
				if (shapes[sel].shapeType > S_PEN) shapes[sel].shapeType = S_POINT;

				SetShapeCord(shapes[sel]);

				sel = -1;

				break;
			}
		}
	}
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
	//--- 콜백 함수: 그리기콜백함수

	glClearColor(1.0, 1.0, 1.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	int vColor = glGetUniformLocation(shaderProgramID, "vColor");

	glUseProgram(shaderProgramID);


	glLineWidth(2.0f);
	glPointSize(8.0f);

	for (int i=0; i < 15; ++i) {
		glBindVertexArray(shapes[i].VAO);
		glUniform3f(vColor, shapes[i].RGB[0], shapes[i].RGB[1], shapes[i].RGB[2]);
		switch (shapes[i].shapeType) {
		case 0:
			break;
		case S_POINT:
			glDrawElements(GL_POINTS, shapes[i].shapeType, GL_UNSIGNED_INT, 0);
			break;
		case S_LINE:
			glDrawElements(GL_LINES, shapes[i].shapeType, GL_UNSIGNED_INT, 0);
			break;
		default:
			glDrawElements(GL_TRIANGLES, (3 * shapes[i].shapeType) - 6, GL_UNSIGNED_INT, 0);
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(s.vPos), s.vPos, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s.EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vIndex), vIndex, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

}

GLvoid UpdateShape(Shape& s) {
	
	glBindBuffer(GL_ARRAY_BUFFER, s.VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(s.vPos), s.vPos);
}

void TimerFunction(int value)
{
	if (!s_toggle) {
		for (int i = 0; i < 15; ++i) {
			if (shapes[i].shapeType == 0 || shapes[i].move == STOP) continue;
			switch (shapes[i].move) {
			case LEFT:
				shapes[i].mid[0] -= 0.015f;
				if (shapes[i].mid[0] < -0.9f) {
					shapes[i].mid[1] -= 0.1f;
					shapes[i].move = RIGHT;
					if (shapes[i].mid[1] < -0.9f) shapes[i].mid[1] += 0.9f;
				}
				break;
			case RIGHT:
				shapes[i].mid[0] += 0.015f;
				if (shapes[i].mid[0] > 0.9f) {
					shapes[i].mid[1] -= 0.1f;
					shapes[i].move = LEFT;
					if (shapes[i].mid[1] < -0.9f) shapes[i].mid[1] += 0.9f;
				}
				break;
			case LU:
				shapes[i].mid[0] -= 0.015f;
				shapes[i].mid[1] += 0.015f;
				if (shapes[i].mid[0] < -0.9f) {
					shapes[i].mid[0] += 0.015f;
					shapes[i].move = RU;
				}
				if (shapes[i].mid[1] > 0.9f) {
					shapes[i].mid[1] -= 0.015f;
					shapes[i].move = LD;
				}
				break;
			case LD:
				shapes[i].mid[0] -= 0.015f;
				shapes[i].mid[1] -= 0.015f;
				if (shapes[i].mid[0] < -0.9f) {
					shapes[i].mid[0] += 0.015f;
					shapes[i].move = RD;
				}
				if (shapes[i].mid[1] < -0.9f) {
					shapes[i].mid[1] += 0.015f;
					shapes[i].move = LU;
				}
				break;
			case RU:
				shapes[i].mid[0] += 0.015f;
				shapes[i].mid[1] += 0.015f;
				if (shapes[i].mid[0] > 0.9f) {
					shapes[i].mid[0] -= 0.015f;
					shapes[i].move = LU;
				}
				if (shapes[i].mid[1] > 0.9f) {
					shapes[i].mid[1] -= 0.015f;
					shapes[i].move = RD;
				}
				break;
			case RD:
				shapes[i].mid[0] += 0.015f;
				shapes[i].mid[1] -= 0.015f;
				if (shapes[i].mid[0] > 0.9f) {
					shapes[i].mid[0] -= 0.015f;
					shapes[i].move = LD;
				}
				if (shapes[i].mid[1] < -0.9f) {
					shapes[i].mid[1] += 0.015f;
					shapes[i].move = RU;
				}
				break;
			}

			SetShapeCord(shapes[i]);
		}
	}

	glutPostRedisplay();
	glutTimerFunc(16, TimerFunction, 1);
}

GLvoid SetShapes(Shape s[]) {

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(-0.7f, 0.7f);

	for (int i = 0; i < 15; ++i) {
		s[i].mid[0] = dis(gen);
		s[i].mid[1] = dis(gen);

		for (int j = 0; j < 3; ++j)
			s[i].RGB[j] = c2GLrgb(rand() % 256);

		s[i].shapeType = (i / 3) + 1;
		SetShapeCord(s[i]);

		s[i].move = STOP;
		InitBuffer(s[i]);
	}
}

GLvoid SetShapeCord(Shape& s) 
{
	switch (s.shapeType) {
	case S_POINT:
		s.vPos[0] = 0.0f + s.mid[0];
		s.vPos[1] = 0.0f + s.mid[1];
		s.vPos[2] = 0.0f;
		break;
	case S_LINE:
		for (int j = 0; j < 6; ++j) {
			s.vPos[j] = linePos[j];
			if (j % 3 == 0)
				s.vPos[j] += s.mid[0];
			else if (j % 3 == 1)
				s.vPos[j] += s.mid[1];
		}
		break;
	case S_TRI:
		for (int j = 0; j < 9; ++j) {
			s.vPos[j] = triPos[j];
			if (j % 3 == 0)
				s.vPos[j] += s.mid[0];
			else if (j % 3 == 1)
				s.vPos[j] += s.mid[1];
		}
		break;
	case S_REC:
		for (int j = 0; j < 12; ++j) {
			s.vPos[j] = recPos[j];
			if (j % 3 == 0)
				s.vPos[j] += s.mid[0];
			else if (j % 3 == 1)
				s.vPos[j] += s.mid[1];
		}
		break;
	case S_PEN:
		for (int j = 0; j < 15; ++j) {
			s.vPos[j] = penPos[j];
			if (j % 3 == 0)
				s.vPos[j] += s.mid[0];
			else if (j % 3 == 1)
				s.vPos[j] += s.mid[1];
		}
		break;
	}

	UpdateShape(s);
}