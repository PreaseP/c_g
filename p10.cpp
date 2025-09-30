#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <vector>
#include <cmath>

#define WinX 800
#define WinY 600

#define STOP 0

#define LU 11
#define LD 12
#define RU 13
#define RD 14

#define LEFT 21
#define RIGHT 22

#define RLEFT 31
#define RRIGHT 32
#define RUP 33
#define RDOWN 34

#define CIRCLE 41

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

int checkPos(int x, int y) {
	int pos = 0;

	if (x > WinX / 2) pos += 1;
	if (y > WinY / 2) pos += 2;

	return pos;
}

typedef struct _SHAPE {
	int mid[2];
	std::vector<GLfloat> vPosition;
	std::vector<GLuint> vIndex;
	GLfloat color[3];
	GLuint VAO, VBO, EBO;
	int speed;
	int size[2];
	int moveMode;
	int zigzagDir;
	int rectDir;
	int last[4];
	int cirDir;
	float radian;
	float r;
} Shape;

void Mouse(int button, int state, int x, int y);
void Keyboard(unsigned char key, int x, int y);
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

GLuint VAO;

Shape shapes[4][4];

int rightIdx[4] = { 1, 1, 1, 1 };
int mode = GL_TRIANGLES;

GLvoid InitBuffer();
GLvoid InitBuffer(Shape& s);
GLvoid SetShape(int pos, int idx, int x, int y);
GLvoid UpdateShape(Shape& s);
GLvoid resetShape(Shape& s);

bool init = true;

float vPositionList[] = {
	-1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, -1.0f, 0.0f
};

unsigned int index[] = {
	0, 1, 2, 3
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

	SetShape(0, 0, WinX / 4, WinY / 4);
	SetShape(1, 0, WinX * 3 / 4, WinY / 4);
	SetShape(2, 0, WinX / 4, WinY * 3 / 4);
	SetShape(3, 0, WinX * 3 / 4, WinY * 3 / 4);
	init = false;

	//--- 세이더프로그램만들기
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(Mouse);
	glutTimerFunc(16, TimerFunction, 1);
	glutMainLoop();
}

void Keyboard(unsigned char key, int x, int y)
{	
	GLfloat mx = c2GLx(x);
	GLfloat my = c2GLy(y);

	if (key == 'q') exit(0);
	else if(key == 'a') mode = GL_TRIANGLES;
	else if(key == 'b') mode = GL_LINE_LOOP;
	else if(key == 'r') {
		for (int i = 0; i < 4; ++i)
			rightIdx[i] = 1;
	}
	else if (key == 'c') {
		init = true;
		SetShape(0, 0, WinX / 4, WinY / 4);
		SetShape(1, 0, WinX * 3 / 4, WinY / 4);
		SetShape(2, 0, WinX / 4, WinY * 3 / 4);
		SetShape(3, 0, WinX * 3 / 4, WinY * 3 / 4);
		init = false;
		rightIdx[0] = rightIdx[1] = rightIdx[2] = rightIdx[3] = 1;
	}
	else if (key == '1') {
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < rightIdx[i]; ++j) {
				resetShape(shapes[i][j]);
				if (shapes[i][j].moveMode == STOP)
					shapes[i][j].moveMode = LU + rand() % 4;
			}
		}
	}
	else if (key == '2') {
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < rightIdx[i]; ++j) {
				resetShape(shapes[i][j]);

				if (shapes[i][j].zigzagDir == 0) {
					if (checkPos(shapes[i][j].mid[0], shapes[i][j].mid[1]) >= 2)
						shapes[i][j].zigzagDir = -1;
					else
						shapes[i][j].zigzagDir = 1;
				}

				if (shapes[i][j].moveMode == STOP)
					shapes[i][j].moveMode = LEFT + rand() % 2;
			}
		}
	}
	else if (key == '3') {
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < rightIdx[i]; ++j) {
				resetShape(shapes[i][j]);

				switch (checkPos(shapes[i][j].mid[0], shapes[i][j].mid[1])) {
					case 0:
						shapes[i][j].moveMode = RDOWN;
						break;
					case 1:
						shapes[i][j].moveMode = RLEFT;
						break;
					case 2:
						shapes[i][j].moveMode = RRIGHT;
						break;
					case 3:
						shapes[i][j].moveMode = RUP;
						break;
				}

				if (shapes[i][j].mid[0] > WinX / 2 - 50 && shapes[i][j].mid[0] < WinX / 2 + 50 &&
					shapes[i][j].mid[1] > WinY / 2 - 50 && shapes[i][j].mid[1] < WinY / 2 + 50) {
					shapes[i][j].rectDir = -1;
					shapes[i][j].last[0] = WinX / 2 - 50;
					shapes[i][j].last[1] = WinY / 2 - 50;
					shapes[i][j].last[2] = WinX / 2 + 50;
					shapes[i][j].last[3] = WinY / 2 + 50;
				}
				else shapes[i][j].rectDir = 1;

			}
		}
	}
	else if (key == '4') {
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < rightIdx[i]; ++j) {
				resetShape(shapes[i][j]);

				shapes[i][j].moveMode = CIRCLE;
				shapes[i][j].cirDir = 1;
			}
		}
	}
	

	drawScene();
}

void Mouse(int button, int state, int x, int y) {
	int pos = checkPos(x, y);

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		// std::cout << "pos: " << pos << ", x: " << x << ", y: " << y << std::endl;
		SetShape(pos, 0, x, y);
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		if (rightIdx[pos] < 4) {
			SetShape(pos, rightIdx[pos], x, y);
			rightIdx[pos]++;
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
	
	glUniform3f(vColor, 0.0f, 1.0f, 1.0f);
	
	glBindVertexArray(VAO);
	glDrawElements(GL_LINES, 4, GL_UNSIGNED_INT, 0);
	
	for (int i=0; i < 4; ++i) {
		for (int j = 0; j < rightIdx[i]; ++j) {
			glUniform3f(vColor, shapes[i][j].color[0], shapes[i][j].color[1], shapes[i][j].color[2]);
			glBindVertexArray(shapes[i][j].VAO);
			glDrawElements(mode, shapes[i][j].vIndex.size(), GL_UNSIGNED_INT, 0);
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

GLvoid SetShape(int pos, int idx, int x, int y)
{
	shapes[pos][idx].vPosition.clear();
	shapes[pos][idx].vIndex.clear();
	shapes[pos][idx].mid[0] = x;
	shapes[pos][idx].mid[1] = y;
	shapes[pos][idx].speed = rand() % 5 + 5;
	shapes[pos][idx].moveMode = STOP;
	shapes[pos][idx].zigzagDir = 0;
	shapes[pos][idx].rectDir = 0;
	shapes[pos][idx].last[0] = 50;
	shapes[pos][idx].last[1] = 50;
	shapes[pos][idx].last[2] = WinX - 50;
	shapes[pos][idx].last[3] = WinY - 50;
	shapes[pos][idx].cirDir = 0;
	shapes[pos][idx].radian = atan2f((float)(y - WinY / 2), (float)(x - WinX / 2));
	shapes[pos][idx].r = sqrt((float)((x - WinX / 2) * (x - WinX / 2)) + (float)((y - WinY / 2) * (y - WinY / 2)));

	for (int i = 0; i < 3; ++i)
		shapes[pos][idx].color[i] = c2GLrgb(rand() % 256);

	if (idx == 0) {
		if (init) {
			shapes[pos][idx].size[0] = 30;
			shapes[pos][idx].size[1] = 30;
		}
		else {
			shapes[pos][idx].size[0] = rand() % 40 + 5;
			shapes[pos][idx].size[1] = rand() % 40 + 5;
		}
	}
	else {
		shapes[pos][idx].size[0] = 30;
		shapes[pos][idx].size[1] = 30;
	}
	
	shapes[pos][idx].vPosition.push_back(c2GLx(shapes[pos][idx].mid[0] - shapes[pos][idx].size[0]));
	shapes[pos][idx].vPosition.push_back(c2GLy(shapes[pos][idx].mid[1] + shapes[pos][idx].size[1]));
	shapes[pos][idx].vPosition.push_back(0.0f);
	shapes[pos][idx].vPosition.push_back(c2GLx(x));
	shapes[pos][idx].vPosition.push_back(c2GLy(y - shapes[pos][idx].size[1]));
	shapes[pos][idx].vPosition.push_back(0.0f);
	shapes[pos][idx].vPosition.push_back(c2GLx(x + shapes[pos][idx].size[0]));
	shapes[pos][idx].vPosition.push_back(c2GLy(y + shapes[pos][idx].size[1]));
	shapes[pos][idx].vPosition.push_back(0.0f);
	shapes[pos][idx].vIndex.push_back(0);
	shapes[pos][idx].vIndex.push_back(2);
	shapes[pos][idx].vIndex.push_back(1);

	InitBuffer(shapes[pos][idx]);
}

GLvoid UpdateShape(Shape& s) {
	s.vPosition.clear();

	s.vPosition.push_back(c2GLx(s.mid[0] - s.size[0]));
	s.vPosition.push_back(c2GLy(s.mid[1] + s.size[1]));
	s.vPosition.push_back(0.0f);
	s.vPosition.push_back(c2GLx(s.mid[0]));
	s.vPosition.push_back(c2GLy(s.mid[1] - s.size[1]));
	s.vPosition.push_back(0.0f);
	s.vPosition.push_back(c2GLx(s.mid[0] + s.size[0]));
	s.vPosition.push_back(c2GLy(s.mid[1] + s.size[1]));
	s.vPosition.push_back(0.0f);

	glBindBuffer(GL_ARRAY_BUFFER, s.VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * s.vPosition.size(), s.vPosition.data());
}

GLvoid resetShape(Shape& s) {
	s.moveMode = STOP;
}

void TimerFunction(int value)
{
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < rightIdx[i]; ++j) {
			switch (shapes[i][j].moveMode) {
			case LU:
				shapes[i][j].mid[0] -= shapes[i][j].speed;
				shapes[i][j].mid[1] -= shapes[i][j].speed;
				if (shapes[i][j].mid[0] < 0)
					shapes[i][j].moveMode = RU;
				else if (shapes[i][j].mid[1] < 0)
					shapes[i][j].moveMode = LD;
				break;
			case LD:
				shapes[i][j].mid[0] -= shapes[i][j].speed;
				shapes[i][j].mid[1] += shapes[i][j].speed;
				if (shapes[i][j].mid[0] < 0)
					shapes[i][j].moveMode = RD;
				else if (shapes[i][j].mid[1] > WinY)
					shapes[i][j].moveMode = LU;
				break;
			case RU:
				shapes[i][j].mid[0] += shapes[i][j].speed;
				shapes[i][j].mid[1] -= shapes[i][j].speed;
				if (shapes[i][j].mid[0] > WinX)
					shapes[i][j].moveMode = LU;
				else if (shapes[i][j].mid[1] < 0)
					shapes[i][j].moveMode = RD;
				break;
			case RD:
				shapes[i][j].mid[0] += shapes[i][j].speed;
				shapes[i][j].mid[1] += shapes[i][j].speed;
				if (shapes[i][j].mid[0] > WinX)
					shapes[i][j].moveMode = LD;
				else if (shapes[i][j].mid[1] > WinY)
					shapes[i][j].moveMode = RU;
				break;
			case LEFT:
				shapes[i][j].mid[0] -= shapes[i][j].speed;
				if(shapes[i][j].mid[0] < 0) {
					shapes[i][j].moveMode = RIGHT;
					if (shapes[i][j].zigzagDir == 1) {
						shapes[i][j].mid[1] += 50;
						if (shapes[i][j].mid[1] > WinY) {
							shapes[i][j].zigzagDir = -1;
							shapes[i][j].mid[1] -= 100;
						}
					}
					else if (shapes[i][j].zigzagDir == -1) {
						shapes[i][j].mid[1] -= 50;
						if (shapes[i][j].mid[1] < 0) {
							shapes[i][j].zigzagDir = 1;
							shapes[i][j].mid[1] += 100;
						}
					}
				}
				break;
			case RIGHT:
				shapes[i][j].mid[0] += shapes[i][j].speed;
				if (shapes[i][j].mid[0] > WinX) {
					shapes[i][j].moveMode = LEFT;
					if (shapes[i][j].zigzagDir == 1) {
						shapes[i][j].mid[1] += 50;
						if (shapes[i][j].mid[1] > WinY) {
							shapes[i][j].zigzagDir = -1;
							shapes[i][j].mid[1] -= 50;
						}
					}
					else if (shapes[i][j].zigzagDir == -1) {
						shapes[i][j].mid[1] -= 50;
						if (shapes[i][j].mid[1] < 0) {
							shapes[i][j].zigzagDir = 1;
							shapes[i][j].mid[1] += 50;
						}
					}
				}
				break;
			case RLEFT:
				shapes[i][j].mid[0] -= shapes[i][j].speed;
				if (shapes[i][j].mid[0] > WinX / 2 - 50 && shapes[i][j].mid[0] < WinX / 2 + 50 &&
					shapes[i][j].mid[1] > WinY / 2 - 50 && shapes[i][j].mid[1] < WinY / 2 + 50 && shapes[i][j].rectDir == 1) {
					shapes[i][j].rectDir = -1;
					shapes[i][j].moveMode = RRIGHT;
				}

				if (shapes[i][j].rectDir == -1 && shapes[i][j].mid[0] < 0) {
					shapes[i][j].rectDir = 1;
					shapes[i][j].moveMode = RRIGHT;
				}

				if (shapes[i][j].mid[0] < shapes[i][j].last[0]) {
					if (shapes[i][j].rectDir == 1) {
						shapes[i][j].last[1] += 40;
						shapes[i][j].moveMode = RDOWN;
					}
					else if (shapes[i][j].rectDir == -1) {
						shapes[i][j].last[1] -= 40;
						shapes[i][j].moveMode = RUP;
					}
				}
				break;
			case RRIGHT:
				shapes[i][j].mid[0] += shapes[i][j].speed;
				if (shapes[i][j].mid[0] > WinX / 2 - 50 && shapes[i][j].mid[0] < WinX / 2 + 50 &&
					shapes[i][j].mid[1] > WinY / 2 - 50 && shapes[i][j].mid[1] < WinY / 2 + 50 && shapes[i][j].rectDir == 1) {
					shapes[i][j].rectDir = -1;
					shapes[i][j].moveMode = RLEFT;
				}

				if (shapes[i][j].rectDir == -1 && shapes[i][j].mid[0] > WinX) {
					shapes[i][j].rectDir = 1;
					shapes[i][j].moveMode = RLEFT;
				}

				if (shapes[i][j].mid[0] > shapes[i][j].last[2]) {
					if (shapes[i][j].rectDir == 1) {
						shapes[i][j].last[3] -= 40;
						shapes[i][j].moveMode = RUP;
					}
					else if (shapes[i][j].rectDir == -1) {
						shapes[i][j].last[3] += 40;
						shapes[i][j].moveMode = RDOWN;
					}
				}
				break;
			case RUP:
				shapes[i][j].mid[1] -= shapes[i][j].speed;
				if (shapes[i][j].mid[0] > WinX / 2 - 50 && shapes[i][j].mid[0] < WinX / 2 + 50 &&
					shapes[i][j].mid[1] > WinY / 2 - 50 && shapes[i][j].mid[1] < WinY / 2 + 50 && shapes[i][j].rectDir == 1) {
					shapes[i][j].rectDir = -1;
					shapes[i][j].moveMode = RDOWN;
				}

				if (shapes[i][j].rectDir == -1 && shapes[i][j].mid[1] < 0) {
					shapes[i][j].rectDir = 1;
					shapes[i][j].moveMode = RDOWN;
				}

				if (shapes[i][j].mid[1] < shapes[i][j].last[1]) {
					if (shapes[i][j].rectDir == 1) {
						shapes[i][j].last[0] += 40;
						shapes[i][j].moveMode = RLEFT;
					}
					else if (shapes[i][j].rectDir == -1) {
						shapes[i][j].last[0] -= 40;
						shapes[i][j].moveMode = RRIGHT;
					}
				}
				break;
			case RDOWN:
				shapes[i][j].mid[1] += shapes[i][j].speed;
				if (shapes[i][j].mid[0] > WinX / 2 - 50 && shapes[i][j].mid[0] < WinX / 2 + 50 &&
					shapes[i][j].mid[1] > WinY / 2 - 50 && shapes[i][j].mid[1] < WinY / 2 + 50 && shapes[i][j].rectDir == 1) {
					shapes[i][j].rectDir = -1;
					shapes[i][j].moveMode = RUP;
				}

				if (shapes[i][j].rectDir == -1 && shapes[i][j].mid[1] > WinY) {
					shapes[i][j].rectDir = 1;
					shapes[i][j].moveMode = RUP;
				}

				if (shapes[i][j].mid[1] > shapes[i][j].last[3]) {
					if (shapes[i][j].rectDir == 1) {
						shapes[i][j].last[2] -= 40;
						shapes[i][j].moveMode = RRIGHT;
					}
					else if (shapes[i][j].rectDir == -1) {
						shapes[i][j].last[2] += 40;
						shapes[i][j].moveMode = RLEFT;
					}
				}
				break;
				case CIRCLE:
					
					shapes[i][j].mid[0] = (int)(WinX / 2 + shapes[i][j].r * cos(shapes[i][j].radian));
					shapes[i][j].mid[1] = (int)(WinY / 2 + shapes[i][j].r * sin(shapes[i][j].radian));

					if (shapes[i][j].cirDir == 1) {
						shapes[i][j].radian += (float)shapes[i][j].speed / 100;
						shapes[i][j].r -= 0.3f;
						if (shapes[i][j].r < 0.0f) shapes[i][j].cirDir = -1;
					}
					else if (shapes[i][j].cirDir == -1) {
						shapes[i][j].radian -= (float)shapes[i][j].speed / 100;
						shapes[i][j].r += 0.3f;
						if (shapes[i][j].r >= sqrt((float)((WinX / 2) * (WinX / 2)) + (float)((WinY / 2) * (WinY / 2)))) shapes[i][j].cirDir = 1;
					}


					if (shapes[i][j].radian > 2 * 3.141592) shapes[i][j].radian -= 2 * 3.141592;
			}

			UpdateShape(shapes[i][j]);
		}
	}

	glutPostRedisplay();
	glutTimerFunc(16, TimerFunction, 1);
}