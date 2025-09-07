#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cctype>
#include <sstream>
#include <algorithm>
#include <Windows.h>

using namespace std;

int cntAlpha(const string& str) {
	int cnt = 0;

	for (char c : str)
		if (isalpha((unsigned char)c)) cnt++;

	return cnt;
}

bool equalWord(const string& a, const string& b) {
	if (a.size() != b.size()) return false;
	for (int i = 0; i < a.size(); ++i)
		if (toupper((unsigned char)a[i]) != toupper((unsigned char)b[i])) return false;

	return true;
}

int main() {
	vector<string> strs;
	string fileName, line;
	ifstream file;
	int toggles[8] = { }; // a, c, d, e, f, g, h, i
	char cmd;

	cout << "���ϸ� �Է� : ";
	cin >> fileName;

	file.open(fileName);

	if (file.fail()) {
		cout << "���� ���� ����" << endl;
		return 0;
	}

	cout << "���� ���� ����" << endl;

	while (getline(file, line))
		strs.push_back(line);
	
	for (int i = 0; i < strs.size(); ++i)
		cout << i + 1 << "��° ���� : " << strs[i] << endl;

	while (1)
	{
		cout << "��ɾ� �Է� : ";
		cin >> cmd;

		system("cls");

		switch (cmd) {
		case 'q':
			file.close();
			return 0;
			break;

		case 'a':
			for (int i = 0; i < 8; ++i) {
				if (i == 0) {
					if (!toggles[i]) toggles[i] = 1;
					else toggles[i] = 0;
				}
				else toggles[i] = 0;
			}

			if (toggles[0]) {
				for (int i = 0; i < strs.size(); ++i) {
					cout << i + 1 << "��° ���� : ";
					for (char c : strs[i]) 
						cout << (char)toupper((unsigned char)c);
					cout << endl;
				}
			}
			else
				for (int i = 0; i < strs.size(); ++i)
					cout << i + 1 << "��° ���� : " << strs[i] << endl;
			break;

		case 'b' :
			for (int i = 0; i < 8; ++i) 
				toggles[i] = 0;

			for (int i = 0; i < strs.size(); ++i) {
				istringstream check(strs[i]);
				string tmp;
				int cnt = 0;

				while (check >> tmp)
					cnt++;

				cout << i + 1 << "��° ���� : " << strs[i] << " / �ܾ� ���� : " << cnt << "��" << endl;
			}

			break;

		case 'c' :
			for (int i = 0; i < 8; ++i) {
				if (i == 1) {
					if (!toggles[i]) toggles[i] = 1;
					else toggles[i] = 0;
				}
				else toggles[i] = 0;
			}

			if (toggles[1]) {
				for (int i = 0; i < strs.size(); ++i) {
					istringstream check(strs[i]);
					string tmp;
					int cnt = 0;

					cout << i + 1 << "��° ���� : ";

					while (check >> tmp) {
						if (!tmp.empty() && isupper((unsigned char)tmp[0])) {
							cnt++;
							SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
						}
						else SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

						cout << tmp << " ";
					}

					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
					cout << " / �빮�ڷ� �����ϴ� �ܾ� ���� : " << cnt << "��" << endl;
					
				}
			}
			else
				for (int i = 0; i < strs.size(); ++i)
					cout << i + 1 << "��° ���� : " << strs[i] << endl;
			break;

		case 'd':
			for (int i = 0; i < 8; ++i) {
				if (i == 2) {
					if (!toggles[i]) toggles[i] = 1;
					else toggles[i] = 0;
				}
				else toggles[i] = 0;
			}

			if (toggles[2]) {
				for (int i = 0; i < strs.size(); ++i) {
					string tmp = strs[i];

					cout << i + 1 << "��° ���� : ";
					reverse(tmp.begin(), tmp.end());
					cout << tmp << endl;
				}
			}
			else
				for (int i = 0; i < strs.size(); ++i)
					cout << i + 1 << "��° ���� : " << strs[i] << endl;

			break;

		case 'e':
			for (int i = 0; i < 8; ++i) {
				if (i == 3) {
					if (!toggles[i]) toggles[i] = 1;
					else toggles[i] = 0;
				}
				else toggles[i] = 0;
			}

			if (toggles[3]) {
				for (int i = 0; i < strs.size(); ++i) {
					string tmp = strs[i];

					cout << i + 1 << "��° ���� : ";
					replace(tmp.begin(), tmp.end(), ' ', '/');
					cout << tmp << endl;
				}
			}
			else
				for (int i = 0; i < strs.size(); ++i)
					cout << i + 1 << "��° ���� : " << strs[i] << endl;

			break;

		case 'f':
			for (int i = 0; i < 8; ++i) {
				if (i == 4) {
					if (!toggles[i]) toggles[i] = 1;
					else toggles[i] = 0;
				}
				else toggles[i] = 0;
			}

			if (toggles[4]) {
				for (int i = 0; i < strs.size(); ++i) {
					string tmp = strs[i];

					for (char& c : tmp)
						if (c == '/') c = ' ';

					istringstream check(tmp);
					vector<string> words;
					string word;
					while (check >> word)
						words.push_back(word);

					cout << i + 1 << "��° ���� : ";
					for (int i = words.size() - 1; i >= 0; --i)
						cout << words[i] << " ";
					cout << endl;
				}
			}
			else
				for (int i = 0; i < strs.size(); ++i)
					cout << i + 1 << "��° ���� : " << strs[i] << endl;

			break;

		case 'g':
			for (int i = 0; i < 8; ++i) {
				if (i == 5) {
					if (!toggles[i]) toggles[i] = 1;
					else toggles[i] = 0;
				}
				else toggles[i] = 0;
			}

			if (toggles[5]) {
				char fromC, toC;
				
				for (int i = 0; i < strs.size(); ++i)
					cout << i + 1 << "��° ���� : " << strs[i] << endl;

				cout << "�ٲ� ���� �Է� : ";
				cin >> fromC;
				cout << "���Ӱ� �Է��� ���� �Է� : ";
				cin >> toC;

				for (int i = 0; i < strs.size(); ++i) {
					string tmp = strs[i];

					for (char& c : tmp)
						if (c == fromC) c = toC;

					cout << i + 1 << "��° ���� : " << tmp << endl;
				}
			}
			else
				for (int i = 0; i < strs.size(); ++i)
					cout << i + 1 << "��° ���� : " << strs[i] << endl;

			break;

		case 'h':
			for (int i = 0; i < 8; ++i) {
				if (i == 6) {
					if (!toggles[i]) toggles[i] = 1;
					else toggles[i] = 0;
				}
				else toggles[i] = 0;
			}

			if (toggles[6]) {
				for (int i = 0; i < strs.size(); ++i) {
					cout << i + 1 << "��° ���� : ";
					for (char c : strs[i]) {
						cout << c;
						if (c >= '0' && c <= '9') cout << endl;
					}
					cout << endl;
				}
			}
			else
				for (int i = 0; i < strs.size(); ++i)
					cout << i + 1 << "��° ���� : " << strs[i] << endl;

			break;

		case 'i':
			for (int i = 0; i < 8; ++i) {
				if (i == 7) {
					if (!toggles[i]) toggles[i] = 1;
					else if (toggles[i] == 1) toggles[i] = -1;
					else toggles[i] = 0;
				}
				else toggles[i] = 0;
			}

			if (toggles[7]) {
				vector<string> sortStrs = strs;

				if (toggles[7] == 1) {
					stable_sort(sortStrs.begin(), sortStrs.end(), [](const string& a, const string& b) {return cntAlpha(a) < cntAlpha(b); });
					cout << "�������� ����" << endl;
				}
				else {
					stable_sort(sortStrs.begin(), sortStrs.end(), [](const string& a, const string& b) {return cntAlpha(a) > cntAlpha(b); });
					cout << "�������� ����" << endl;
				}

				for (int i = 0; i < sortStrs.size(); ++i)
					cout << i + 1 << "��° ���� : " << sortStrs[i] << " / ���ĺ� ���� : " << cntAlpha(sortStrs[i]) << endl;
			}
			else
				for (int i = 0; i < strs.size(); ++i)
					cout << i + 1 << "��° ���� : " << strs[i] << endl;

			break;

		case 'j':
			for (int i = 0; i < strs.size(); ++i)
				cout << i + 1 << "��° ���� : " << strs[i] << endl;

			cout << "�ܾ� �Է� : ";
			cin >> line;

			for (int i = 0; i < strs.size(); ++i) {
				istringstream check(strs[i]);
				string tmp;
				int cnt = 0;

				cout << i + 1 << "��° ���� : ";

				while (check >> tmp) {
					if (equalWord(line, tmp)) {
						cnt++;
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
					}
					else SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

					cout << tmp << " ";
				}

				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
				cout << " / ��ġ�ϴ� �ܾ� ���� : " << cnt << "��" << endl;
			}

			break;

		}
	}
 }