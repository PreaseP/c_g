#define initSize 30
#define MIN 10
#define MAX 40

#include <iostream>
#include <Windows.h>

using namespace std;

typedef struct myRect {
	int x1, x2;
	int y1, y2;
} Rt;

void setBoard(int board[40][40], Rt rects[2], int sizeY, int sizeX) {
	for (int i = 0; i < 40; ++i)
		for (int j = 0; j < 40; ++j)
			board[i][j] = 0;

	for (int y = rects[0].y1; y <= rects[0].y2; ++y) {
		for (int x = rects[0].x1; x <= rects[0].x2; ++x) {
			int iy = y % sizeY;
			while (iy < 0) iy += sizeY;
			int ix = x % sizeX;
			while (ix < 0) ix += sizeX;
			board[iy][ix] += 1;
		}
	}

	for (int y = rects[1].y1; y <= rects[1].y2; ++y) {
		for (int x = rects[1].x1; x <= rects[1].x2; ++x) {
			int iy = y % sizeY;
			while (iy < 0) iy += sizeY;
			int ix = x % sizeX;
			while (ix < 0) ix += sizeX;
			board[iy][ix] += 2;
		}
	}

	for (int i = 0; i < sizeY; ++i) {
		for (int j = 0; j < sizeX; ++j) {
			switch (board[i][j]) {
			case 1:
				cout << '0';
				break;

			case 2:
				cout << '+';
				break;

			case 3:
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
				cout << '#';
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
				break;

			default:
				cout << '.';
				break;

			}
		}
		cout << endl;
	}
}

int main() {
	int board[40][40];
	int sizeX = initSize, sizeY = initSize;
	Rt rects[2];
	char cmd;

	// 0 = 빈칸, 1 = 첫번째 사각형, 2 = 두번째 사각형, 3 = 충돌

	cout << "첫번째 사각형 입력(x1, y1, x2, y2) : ";
	cin >> rects[0].x1 >> rects[0].y1 >> rects[0].x2 >> rects[0].y2;
	if (rects[0].x1 > rects[0].x2) {
		int tmp = rects[0].x2;
		rects[0].x2 = rects[0].x1;
		rects[0].x1 = tmp;
	}
	if (rects[0].y1 > rects[0].y2) {
		int tmp = rects[0].y2;
		rects[0].y2 = rects[0].y1;
		rects[0].y1 = tmp;
	}
	cout << "두번째 사각형 입력(x1, y1, x2, y2) : ";
	cin >> rects[1].x1 >> rects[1].y1 >> rects[1].x2 >> rects[1].y2;
	if (rects[1].x1 > rects[1].x2) {
		int tmp = rects[1].x2;
		rects[1].x2 = rects[1].x1;
		rects[1].x1 = tmp;
	}
	if (rects[1].y1 > rects[1].y2) {
		int tmp = rects[1].y2;
		rects[1].y2 = rects[1].y1;
		rects[1].y1 = tmp;
	}

	setBoard(board, rects, sizeY, sizeX);

	while (1) {
		cout << "명령어 입력 : ";
		cin >> cmd;

		system("cls");

		switch (cmd) {
		case 'q' :
			return 0;
			break;

		case 'c' :
			if (sizeX < MAX) {
				sizeX++;
				sizeY++;
			}
			else cout << "더 이상 늘릴 수 없습니다." << endl;
			break;

		case 'd' :
			if (sizeX > MIN) {
				sizeX--;
				sizeY--;
			}
			else cout << "더 이상 줄일 수 없습니다." << endl;
			break;

		case 'r' :
			sizeY = initSize;
			sizeX = initSize;

			cout << "첫번째 사각형 입력(x1, y1, x2, y2) : ";
			cin >> rects[0].x1 >> rects[0].y1 >> rects[0].x2 >> rects[0].y2;
			if (rects[0].x1 > rects[0].x2) {
				int tmp = rects[0].x2;
				rects[0].x2 = rects[0].x1;
				rects[0].x1 = tmp;
			}
			if (rects[0].y1 > rects[0].y2) {
				int tmp = rects[0].y2;
				rects[0].y2 = rects[0].y1;
				rects[0].y1 = tmp;
			}
			cout << "두번째 사각형 입력(x1, y1, x2, y2) : ";
			cin >> rects[1].x1 >> rects[1].y1 >> rects[1].x2 >> rects[1].y2;
			if (rects[1].x1 > rects[1].x2) {
				int tmp = rects[1].x2;
				rects[1].x2 = rects[1].x1;
				rects[1].x1 = tmp;
			}
			if (rects[1].y1 > rects[1].y2) {
				int tmp = rects[1].y2;
				rects[1].y2 = rects[1].y1;
				rects[1].y1 = tmp;
			}

			break;

		case 'b' :
			cout << "첫번째 사각형의 면적 : " << rects[0].x2 - rects[0].x1 + 1 << " X " << rects[0].y2 - rects[0].y1 + 1 << " = " <<
				(rects[0].x2 - rects[0].x1 + 1) * (rects[0].y2 - rects[0].y1 + 1) << endl;
			cout << "두번째 사각형의 면적 : " << rects[1].x2 - rects[1].x1 + 1 << " X " << rects[1].y2 - rects[1].y1 + 1 << " = " <<
				(rects[1].x2 - rects[1].x1 + 1) * (rects[1].y2 - rects[1].y1 + 1) << endl;
			break;

		case 'x' :
			(rects[0].x1)++;
			(rects[0].x2)++;
			break;

		case '1' :
			(rects[1].x1)++;
			(rects[1].x2)++;
			break;

		case 'X' :
			(rects[0].x1)--;
			(rects[0].x2)--;
			break;
			
		case '2' :
			(rects[1].x1)--;
			(rects[1].x2)--;
			break;

		case 'y' :
			(rects[0].y1)++;
			(rects[0].y2)++;
			break; 

		case '3' :
			(rects[1].y1)++;
			(rects[1].y2)++;
			break;

		case 'Y' :
			(rects[0].y1)--;
			(rects[0].y2)--;
			break;

		case '4' :
			(rects[1].y1)--;
			(rects[1].y2)--;
			break;

		case 's' :
			(rects[0].x2)++;
			(rects[0].y2)++;
			break;

		case '5' :
			(rects[1].x2)++;
			(rects[1].y2)++;
			break;

		case 'S' :
			if(rects[0].x2 - rects[0].x1 > 0) (rects[0].x2)--;
			if(rects[0].y2 - rects[0].y1 > 0) (rects[0].y2)--;
			break;

		case '6' :
			if (rects[1].x2 - rects[1].x1 > 0) (rects[1].x2)--;
			if (rects[1].y2 - rects[1].y1 > 0) (rects[1].y2)--;
			break;

		case 'i' :
			(rects[0].x2)++;
			break;

		case '7' :
			(rects[1].x2)++;
			break;

		case 'I' :
			if (rects[0].x2 - rects[0].x1 > 0) (rects[0].x2)--;
			break;

		case '8' :
			if (rects[1].x2 - rects[1].x1 > 0) (rects[1].x2)--;
			break;

		case 'j' :
			(rects[0].y2)++;
			break;

		case '9' :
			(rects[1].y2)++;
			break;

		case 'J' :
			if (rects[0].y2 - rects[0].y1 > 0) (rects[0].y2)--;
			break;

		case '0' :
			if (rects[1].y2 - rects[1].y1 > 0) (rects[1].y2)--;
			break;

		case 'a' :
			(rects[0].x2)++;
			if (rects[0].y2 - rects[0].y1 > 0) (rects[0].y2)--;
			break;

		case '-' :
			(rects[1].x2)++;
			if (rects[1].y2 - rects[1].y1 > 0) (rects[1].y2)--;
			break;

		case 'A' :
			if (rects[0].x2 - rects[0].x1 > 0) (rects[0].x2)--;
			(rects[0].y2)++;
			break;

		case '+' :
			if (rects[1].x2 - rects[1].x1 > 0) (rects[1].x2)--;
			(rects[1].y2)++;
			break;
		}

		setBoard(board, rects, sizeY, sizeX);

	}
}