#define TURN 20
#define HINT 3

#include <iostream>
#include <time.h>
#include <Windows.h>

using namespace std;

typedef struct CELL {
	char ch;
	bool val;
	int color;
} cell;

int main() {
	cell board[5][5];
	string cmd;
	int cnt = 0, turn = TURN, score = 0, hint = HINT;

	srand(time(NULL));

	for (int i = 0; i < 5; ++i) {
		for (int j = 0; j < 5; ++j) {
			board[i][j].ch = ' ';
			board[i][j].val = false;
		}
	}

	while (cnt < 25) {
		int x = rand() % 5, y = rand() % 5;

		if (board[x][y].ch == ' ') {
			if (!cnt) {
				board[x][y].ch = 'a';
				board[x][y].color = board[x][y].ch - ('a' - 1);
			}
			else if (cnt == 24) board[x][y].ch = '@';
			else { 
				board[x][y].ch = 'a' + (cnt % 12);
				board[x][y].color = board[x][y].ch - ('a' - 1);
				if (board[x][y].color >= 7) (board[x][y].color)++;
			}
			cnt++;
		}
	}

	while (1) {
		cout << "  a b c d e" << endl;
		for (int i = 0; i < 5; ++i) {
			cout << i + 1 << " ";
			for (int j = 0; j < 5; ++j) {
				if (board[i][j].val) {
					if (board[i][j].ch == '@') {
						cout << board[i][j].ch << " ";
					}
					else {
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), board[i][j].color);
						cout << (char)toupper(board[i][j].ch) << " ";
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
					}
				}
				else cout << "* ";
			}
			cout << endl;
		}
		cout << "남은 횟수 : " << turn << " / 점수 : " << score << " / 힌트 갯수 : " << hint << endl;
		if (!turn) cout << "게임 종료!" << endl << "r을 입력해 재시작하거나 q를 입력해 종료하세요" << endl;
		cout << "명령어 입력 : ";
		cin >> cmd;

		system("cls");

		switch (cmd[0]) {
		case 'q':

			return 0;
			break;

		case 'r':
			cout << "게임을 초기화 합니다." << endl;

			cnt = 0;
			turn = TURN;
			score = 0;
			hint = HINT;

			for (int i = 0; i < 5; ++i) {
				for (int j = 0; j < 5; ++j) {
					board[i][j].ch = ' ';
					board[i][j].val = true;
				}
			}

			while (cnt < 25) {
				int x = rand() % 5, y = rand() % 5;

				if (board[x][y].ch == ' ') {
					if (!cnt) {
						board[x][y].ch = 'a';
						board[x][y].color = board[x][y].ch - ('a' - 1);
					}
					else if (cnt == 24) board[x][y].ch = '@';
					else {
						board[x][y].ch = 'a' + (cnt % 12);
						board[x][y].color = board[x][y].ch - ('a' - 1);
						if (board[x][y].color >= 7) (board[x][y].color)++;
					}
					cnt++;
				}
			}

			break;

		case 'h':
			if (hint) {
				hint--;
				cout << "  a b c d e" << endl;

				for (int i = 0; i < 5; ++i) {
					cout << i + 1 << " ";
					for (int j = 0; j < 5; ++j) {
						if (board[i][j].val) {
							if (board[i][j].ch == '@') {
								cout << board[i][j].ch << " ";
							}
							else {
								SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), board[i][j].color);
								cout << (char)toupper(board[i][j].ch) << " ";
								SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
							}
						}
						else cout << board[i][j].ch << " ";
					}
					cout << endl;
				}

				cout << "남은 횟수 : " << turn << " / 점수 : " << score << " / 힌트 갯수 : " << hint << endl;

				Sleep(5000);
				system("cls");
			}
			break;

		default:
			string tmp;
			cin >> tmp;
			int x1, x2, y1, y2;

			x1 = cmd[0] - 'a';
			x2 = tmp[0] - 'a';
			y1 = cmd[1] - '1';
			y2 = tmp[1] - '1';

			if (!turn) cout << "게임이 종료되었습니다." << endl;
			else if (y1 < 0 || y1 > 4 || y2 < 0 || y2 > 4 || x1 < 0 || x1 > 4 || x2 < 0 || x2 > 4)
				cout << "올바른 범위가 아닙니다." << endl;
			else {
				if (y1 == y2 && x1 == x2) cout << "같은 칸을 선택하였습니다." << endl;
				else if (!board[y1][x1].val && !board[y2][x2].val) {

					cout << "  a b c d e" << endl;

					for (int i = 0; i < 5; ++i) {
						cout << i + 1 << " ";
						for (int j = 0; j < 5; ++j) {
							if (i == y1 && j == x1) {
								if (board[i][j].ch == '@') {
									cout << board[i][j].ch << " ";
								}
								else {
									SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), board[i][j].color);
									cout << board[i][j].ch << " ";
									SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
								}
							}
							else if (i == y2 && j == x2) {
								if (board[i][j].ch == '@') {
									cout << board[i][j].ch << " ";
								}
								else {
									SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), board[i][j].color);
									cout << board[i][j].ch << " ";
									SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
								}
							}
							else if (board[i][j].val) {
								if (board[i][j].ch == '@') {
									cout << board[i][j].ch << " ";
								}
								else {
									SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), board[i][j].color);
									cout << (char)toupper(board[i][j].ch) << " ";
									SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
								}
							}
							else cout << "* ";
						}
						cout << endl;
					}

					Sleep(1500);
					system("cls");

					if (board[y1][x1].ch == board[y2][x2].ch) {
						board[y1][x1].val = true;
						board[y2][x2].val = true;
						score += 10;
					}
					else if (board[y1][x1].ch == '@') {
						board[y1][x1].val = true;
						board[y2][x2].val = true;

						for (int i = 0; i < 5; ++i) {
							for (int j = 0; j < 5; ++j) {
								if (board[i][j].ch == board[y2][x2].ch) board[i][j].val = true;
							}
						}
						score += 10;
					}
					else if (board[y2][x2].ch == '@') {
						board[y1][x1].val = true;
						board[y2][x2].val = true;

						for (int i = 0; i < 5; ++i) {
							for (int j = 0; j < 5; ++j) {
								if (board[i][j].ch == board[y1][x1].ch) board[i][j].val = true;
							}
						}
						score += 10;
					}

					turn--;
				}
				else cout << "이미 열린 칸입니다." << endl;
			}

			break;

		}
	}
}