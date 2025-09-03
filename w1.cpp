#include <iostream>
#include <cstdlib>
#include <time.h>
#include <iomanip>

int getDet(int mat[4][4]) {
	return mat[0][0] * (mat[1][1] * mat[2][2] * mat[3][3] + mat[1][2] * mat[2][3] * mat[3][1] + mat[1][3] * mat[2][1] * mat[3][2]
			- (mat[3][1] * mat[2][2] * mat[1][3] + mat[3][2] * mat[2][3] * mat[1][1] + mat[3][3] * mat[2][1] * mat[1][2]))
		- (mat[0][1] * (mat[1][0] * mat[2][2] * mat[3][3] + mat[1][2] * mat[2][3] * mat[3][0] + mat[1][3] * mat[2][0] * mat[3][2]
			- (mat[3][0] * mat[2][2] * mat[1][3] + mat[3][2] * mat[2][3] * mat[1][0] + mat[3][3] * mat[2][0] * mat[1][2])))
		+ mat[0][2] * (mat[1][0] * mat[2][1] * mat[3][3] + mat[1][1] * mat[2][3] * mat[3][0] + mat[1][3] * mat[2][0] * mat[3][1]
			- (mat[3][0] * mat[2][1] * mat[1][3] + mat[3][1] * mat[2][3] * mat[1][0] + mat[3][3] * mat[2][0] * mat[1][1]))
		- (mat[0][3] * (mat[1][0] * mat[2][1] * mat[3][2] + mat[1][1] * mat[2][2] * mat[3][0] + mat[1][2] * mat[2][0] * mat[3][1]
			- (mat[3][0] * mat[2][1] * mat[1][2] + mat[3][1] * mat[2][2] * mat[1][0] + mat[3][2] * mat[2][0] * mat[1][1])));
}

int main() {
	srand(time(NULL));
	std::cout << std::right;
	int m1[4][4], m2[4][4];
	int t1[4][4], t2[4][4];
	int min1 = 0, min2 = 0;
	int max1 = 0, max2 = 0;
	int det1 = 0, det2 = 0;
	int mul = 0;
	char cmd;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			m1[i][j] = rand() % 9 + 1;
			m2[i][j] = rand() % 9 + 1;
		}
	}

	while (1) {
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				std::cout << m1[i][j] << " ";
			}
			std::cout << "  ";
			for (int j = 0; j < 4; ++j) {
				std::cout << m2[i][j] << " ";
			}
			std::cout << std::endl;
		}

		std::cout << "원하는 연산 입력 : ";
		std::cin >> cmd;
		switch (cmd) {
		case 'q' :
			return 0;
			break;

		case 's' :
			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 4; ++j) {
					m1[i][j] = rand() % 9 + 1;
					m2[i][j] = rand() % 9 + 1;
				}
			}
			min1 = min2 = max1 = max2 = det1 = det2 = mul = 0;
			break;

		case 'm':
			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 4; ++j) {
					std::cout << m1[i][j] << " ";
				}
				if (i == 1) std::cout << "· ";
				else std::cout << "  ";
				for (int j = 0; j < 4; ++j) {
					std::cout << m2[i][j] << " ";
				}
				if (i == 1) std::cout << "= ";
				else std::cout << "  ";
				for (int j = 0; j < 4; ++j) {
					std::cout << m1[i][0] * m2[0][j] + m1[i][1] * m2[1][j] + m1[i][2] * m2[2][j] + m1[i][3] * m2[3][j] << " ";
				}
				std::cout << std::endl;
			}
			break;

		case 'a':
			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 4; ++j) {
					std::cout << m1[i][j] << " ";
				}
				if (i == 1) std::cout << "+ ";
				else std::cout << "  ";
				for (int j = 0; j < 4; ++j) {
					std::cout << m2[i][j] << " ";
				}
				if (i == 1) std::cout << "= ";
				else std::cout << "  ";
				for (int j = 0; j < 4; ++j) {
					std::cout << m1[i][j] + m2[i][j] << " ";
				}
				std::cout << std::endl;
			}
			break;

		case 'd':
			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 4; ++j) {
					std::cout << m1[i][j] << " ";
				}
				if (i == 1) std::cout << "- ";
				else std::cout << "  ";
				for (int j = 0; j < 4; ++j) {
					std::cout << m2[i][j] << " ";
				}
				if (i == 1) std::cout << "= ";
				else std::cout << "  ";
				for (int j = 0; j < 4; ++j) {
					std::cout << m1[i][j] - m2[i][j] << " ";
				}
				std::cout << std::endl;
			}
			break;

		case 'r':
			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 4; ++j) {
					std::cout << m1[i][j] << " ";
				}
				std::cout << "  ";
				for (int j = 0; j < 4; ++j) {
					std::cout << m2[i][j] << " ";
				}
				std::cout << std::endl;
			}
			if (!det1 && !det2) {
				det1 = getDet(m1);
				det2 = getDet(m2);
			}
			std::cout << "첫 번째 행렬의 행렬식 : " << det1 << std::endl;
			std::cout << "두 번째 행렬의 행렬식 : " << det2 << std::endl;
			break;

		case 't':
			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 4; ++j) {
					t1[i][j] = m1[j][i];
					t2[i][j] = m2[j][i];
				}
				for (int j = 0; j < 4; ++j) {
					std::cout << m1[i][j] << " ";
				}
				if (i == 1) std::cout << "->";
				else std::cout << "  ";
				for (int j = 0; j < 4; ++j) {
					std::cout << t1[i][j] << " ";
				}
				std::cout << std::endl;
			}
			std::cout << std::endl;
			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 4; ++j) {
					std::cout << m2[i][j] << " ";
				}
				if (i == 1) std::cout << "->";
				else std::cout << "  ";
				for (int j = 0; j < 4; ++j) {
					std::cout << t2[i][j] << " ";
				}
				std::cout << std::endl;
			}
			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 4; ++j) {
					m1[i][j] = t1[i][j];
					m2[i][j] = t2[i][j];
				}
			}
			if (!det1 && !det2) {
				det1 = getDet(m1);
				det2 = getDet(m2);
			}
			std::cout << "첫 번째 행렬의 행렬식 : " << det1 << std::endl;
			std::cout << "두 번째 행렬의 행렬식 : " << det2 << std::endl;
			break;

		case 'e':
			if (min1 && min2) {
				for (int i = 0; i < 4; ++i) {
					for (int j = 0; j < 4; ++j) {
						m1[i][j] += min1;
						m2[i][j] += min2;
					}
				}
				for (int i = 0; i < 4; ++i) {
					for (int j = 0; j < 4; ++j) {
						std::cout << m1[i][j] << " ";
					}
					std::cout << "  ";
					for (int j = 0; j < 4; ++j) {
						std::cout << m2[i][j] << " ";
					}
					std::cout << std::endl;
				}
				min1 = 0;
				min2 = 0;
			}
			else {
				min1 = m1[0][0];
				min2 = m2[0][0];
				for (int i = 0; i < 4; ++i) {
					for (int j = 0; j < 4; ++j) {
						if (min1 > m1[i][j]) min1 = m1[i][j];
						if (min2 > m2[i][j]) min2 = m2[i][j];
					}
				}
				for (int i = 0; i < 4; ++i) {
					for (int j = 0; j < 4; ++j) {
						m1[i][j] -= min1;
						std::cout << m1[i][j] << " ";
					}
					std::cout << std::endl;
				}
				std::cout << "최솟값 : " << min1 << std::endl;
				for (int i = 0; i < 4; ++i) {
					for (int j = 0; j < 4; ++j) {
						m2[i][j] -= min2;
						std::cout << m2[i][j] << " ";
					}
					std::cout << std::endl;
				}
				std::cout << "최솟값 : " << min2 << std::endl;
			}
			break;

		case 'f':
			if (max1 && max2) {
				for (int i = 0; i < 4; ++i) {
					for (int j = 0; j < 4; ++j) {
						m1[i][j] -= max1;
						m2[i][j] -= max2;
					}
				}
				for (int i = 0; i < 4; ++i) {
					for (int j = 0; j < 4; ++j) {
						std::cout << m1[i][j] << " ";
					}
					std::cout << "  ";
					for (int j = 0; j < 4; ++j) {
						std::cout << m2[i][j] << " ";
					}
					std::cout << std::endl;
				}
				max1 = 0;
				max2 = 0;
			}
			else {
				max1 = m1[0][0];
				max2 = m2[0][0];
				for (int i = 0; i < 4; ++i) {
					for (int j = 0; j < 4; ++j) {
						if (max1 < m1[i][j]) max1 = m1[i][j];
						if (max2 < m2[i][j]) max2 = m2[i][j];
					}
				}
				for (int i = 0; i < 4; ++i) {
					for (int j = 0; j < 4; ++j) {
						m1[i][j] += max1;
						std::cout << m1[i][j] << " ";
					}
					std::cout << std::endl;
				}
				std::cout << "최댓값 : " << max1 << std::endl;
				for (int i = 0; i < 4; ++i) {
					for (int j = 0; j < 4; ++j) {
						m2[i][j] += max2;
						std::cout << m2[i][j] << " ";
					}
					std::cout << std::endl;
				}
				std::cout << "최댓값 : " << max2 << std::endl;
			}
			break;

		default:
			if (cmd >= '1' && cmd <= '9') {
				if (!mul) mul = cmd - '0';
				else {
					if (mul == cmd - '0') mul = 0;
					else mul = cmd - '0';
				}
				if (!mul) std::cout << "원래대로 출력합니다.";
				else std::cout << "입력한 숫자 : " << mul;
				std::cout << std::endl;
				for (int i = 0; i < 4; ++i) {
					for (int j = 0; j < 4; ++j) {
						if (mul && (m1[i][j] % mul == 0)) {
							std::cout << "X" << " ";
						} 
						else std::cout << m1[i][j] << " ";
					}
					std::cout << "  ";
					for (int j = 0; j < 4; ++j) {
						if (mul && (m2[i][j] % mul == 0)) {
							std::cout << "X" << " ";
						}
						else std::cout << m2[i][j] << " ";
					}
					std::cout << std::endl;
				}

			}
			else std::cout << "올바른 명령어가 아닙니다." << std::endl;
			break;

		}
		std::cout << "-------------------------------------------" << std::endl;
	}
}