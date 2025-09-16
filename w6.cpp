#include <iostream>
#include <Windows.h>
#include <time.h>
#include <conio.h>

using namespace std;

void initBoard(int board[30][30]) {
	for (int i = 0; i < 30; ++i)
		for (int j = 0; j < 30; ++j) 
			board[i][j] = 0;

	board[29][29] = -2;

	srand(time(NULL));
	int obs = (rand() % 7) + 4, cnt = 0;

	while (cnt < obs) {
		int x = rand() % 30, y = rand() % 30;
		if (!(y == 0 && x == 0) && !(y == 29 && x == 29) && board[y][x] == 0) {
			board[y][x] = -1;
			cnt++;
		}
	}
}

void printBoard(int board[30][30], int py, int px) {
	for (int i = 0; i < 30; ++i) {
		for (int j = 0; j < 30; ++j) {
			if (i == py && j == px) {
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
				cout << "*";
			}
			else if (board[i][j] == -2) {
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
				cout << "F";
			}
			else if (board[i][j] == -1) {
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 9);
				cout << "X";
			}
			else {
				if (board[i][j] > 0) {
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (board[i][j] / 10 + 2));
					cout << (board[i][j] % 10);
				}
				else {
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
					cout << board[i][j];
				}
			}
		}
		cout << endl;
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

}

int main() {
	int board[30][30];
	int py = 0, px = 0;
	char cmd;
	bool clear = false;
	initBoard(board);

	while (1) {
		printBoard(board, py, px);

		cmd = _getch();

		system("cls");

		switch (cmd) {
		case 'q' :
			return 0;
			break;

		case 13 :
			py = px = 0;
			initBoard(board);
			break;
			
		case 'w' :
			if (py > 0) {
				if (board[py - 1][px] == -1) cout << "장애물이 있어 이동할 수 없습니다." << endl;
				else {
					board[py][px] += 1;
					py--;
				}
			}
			else cout << "더 이상 위로 이동할 수 없습니다." << endl;
			break;

		case 's':
			if (py < 29) {
				if (board[py + 1][px] == -1) cout << "장애물이 있어 이동할 수 없습니다." << endl;
				else {
					board[py][px] += 1;
					py++;
				}
			}
			else cout << "더 이상 아래로 이동할 수 없습니다." << endl;
			break;

		case 'a':
			if (px > 0) {
				if (board[py][px - 1] == -1) cout << "장애물이 있어 이동할 수 없습니다." << endl;
				else {
					board[py][px] += 1;
					px--;
				}
			}
			else cout << "더 이상 왼쪽으로 이동할 수 없습니다." << endl;
			break;

		case 'd':
			if (px < 29) {
				if (board[py][px + 1] == -1) cout << "장애물이 있어 이동할 수 없습니다." << endl;
				else {
					board[py][px] += 1;
					px++;
				}
			}
			else cout << "더 이상 오른쪽으로 이동할 수 없습니다." << endl;
			break;

		}

		if (board[py][px] == -2) {
			printBoard(board, py, px);
			cout << "목적지 도착" << endl;
			cout << "enter를 눌러 재시작하거나 q를 눌러 종료" << endl;
			while (1) {
				cmd = _getch();
				if (cmd == 13) {
					py = px = 0;
					initBoard(board);
					break;
				}
				else if (cmd == 'q') return 0;
			}
			system("cls");
		}
	}
}