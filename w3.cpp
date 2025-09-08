#include <iostream>
#include <vector>
#include <cmath>
#include <Windows.h>

using namespace std;

typedef struct POINT_3 {
	int x, y, z;
	double d;
	bool val;
} point_3;

int main() {
	point_3 points[10] = {};
	int end = 0, cnt = 0;
	char cmd;
	int ix, iy, iz;

	for (int i = 9; i >= 0; --i) {
		cout << i << " | ";

		if (points[i].val) cout << points[i].x << " | " << points[i].y << " | " << points[i].z << endl;
		else cout << "  |   |" << endl;
	}

	while (1) {
		

		cout << "명령어 입력 : ";
		cin >> cmd;

		system("cls");

		switch (cmd) {
		case 'q' :
			return 0;

			break;
		case '+' :
			cin >> ix >> iy >> iz;
			
			if (end <= 9 && !points[end].val) {
				points[end].x = ix;
				points[end].y = iy;
				points[end].z = iz;
				points[end].val = true;
				points[end].d = sqrt((ix * ix) + (iy * iy) + (iz * iz));
				end++;
				cnt++;
			}
			else if (cnt == 10) cout << "빈 공간이 없습니다." << endl;
			else {
				int idx = 8;
				while (1) {
					if (!points[idx].val) break;
					idx--;
				}

				for (int i = idx; i < 9; ++i)
					points[i] = points[i + 1];

				points[9].x = ix;
				points[9].y = iy;
				points[9].z = iz;
				points[9].val = true;
				points[9].d = sqrt((ix * ix) + (iy * iy) + (iz * iz));
				cnt++;
			}

			break;

		case '-' :
			if (end > 0) {
				points[end - 1].val = false;
				cnt--;
				end--;
				while (1) {
					if (!end || points[end - 1].val) break;
					end--;
				}
				if (!cnt) end = 0;
			}
			else {
				if (points[end].val) {
					points[end].val = false;
					cnt--;
				}
				else cout << "리스트가 비어 있습니다." << endl;
			}
			break;

		case 'a' :
			cout << "저장된 점의 개수 : " << cnt << endl;
			break;

		case 'e' :
			cin >> ix >> iy >> iz;

			if (cnt == 10) cout << "빈 공간이 없습니다." << endl;
			else if (end == cnt) {
				for (int i = end; i > 0; --i)
					points[i] = points[i - 1];

				points[0].x = ix;
				points[0].y = iy;
				points[0].z = iz;
				points[0].d = sqrt((ix * ix) + (iy * iy) + (iz * iz));
				points[0].val = true;
				end++;
				cnt++;
			}
			else {
				int idx = 1;
				while (1) {
					if (!points[idx].val) break;
					idx++;
				}
				for (int i = idx; i > 0; --i) 
					points[i] = points[i - 1];

				points[0].x = ix;
				points[0].y = iy;
				points[0].z = iz;
				points[0].d = sqrt((ix * ix) + (iy * iy) + (iz * iz));
				points[0].val = true;
				cnt++;
			}

			break;

		case 'd':
			if(!cnt) cout << "리스트가 비어 있습니다." << endl;
			else {
				for (int i = 0; i < end; ++i) {
					if (points[i].val) {
						points[i].val = false;
						cnt--;
						if (!cnt) end = 0;
						break;
					}
				}
			}

			break;

		case 'b' :
			if (!cnt) cout << "리스트가 비어 있습니다." << endl;
			else {
				point_3 tmp = points[9];

				for (int i = 9; i > 0; --i)
					points[i] = points[i - 1];

				points[0] = tmp;

				for (int i = 9; i >= 0; --i) {
					if (points[i].val) {
						end = i + 1;
						break;
					}
				}

			}
			break;

		case 'c' :
			for (int i = 0; i < 9; ++i)
				points[i].val = false;

			cnt = end = 0;

			break;

		case 'f' :
			point_3 tmp[10] = { };
			int idx = 0;

			while (cnt) {
				int minIdx = 0;
				double min;
				
				for (int i = 0; i < 9; ++i) {
					if (points[i].val) {
						min = points[i].d;
						minIdx = i;
						break;
					}
				}

				for (int i = 0; i < 9; ++i) {
					if (points[i].val) {
						if (points[i].d < min) {
							min = points[i].d;
							minIdx = i;
						}
					}
				}

				tmp[idx] = points[minIdx];
				points[minIdx].val = false;
				idx++;
				cnt--;
			}

			for (int i = 0; i < 9; ++i)
				points[i] = tmp[i];

			cnt = end = idx;

			cout << "*오름차순 정렬*" << endl;

			break;

		}
		for (int i = 9; i >= 0; --i) {
			cout << i << " | ";

			if (points[i].val) {
				cout << points[i].x << " | " << points[i].y << " | " << points[i].z;
				if (cmd == 'f') cout << " / 원점과의 거리 : " << points[i].d;
				cout << endl;
			}
			else cout << "  |   |" << endl;
		}
		
	}

 }