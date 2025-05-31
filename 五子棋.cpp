#include<iostream>
#include<string>//����
#include<chrono>//��ʱ
#include<sstream>//����
#include<algorithm>//����
#include<vector>//���������
#include<unordered_map>//��ϣ��
#include<functional>
#include<iomanip>//���

using namespace std;

//������Ϣ
#define NumberOfCheck 5//���ǵ�������Ŀ
#define Depth_ 5//�ݹ����
#define Time 1.2//����ʱ�䣨һ��ﲻ����
struct Data
{
	int X;
	int Y;
	int Score;
	Data(int x = 1, int y = 1, int s = INT_MIN) {
		X = x;
		Y = y;
		Score = s;
	}
};
int Board[14][14] = { 0 };//���̣�0Ϊ�ո�1Ϊ���ӣ�2Ϊ����,3Ϊ���̱߽�,��Ҫ�Ƿ�ֹԽ�����
int Self = 0, Enemy = 0;//��Ӫ
auto Start = chrono::high_resolution_clock::now();//��ʱ���������ڴ˴���ʼ��������λ�ƶ�
Data Best(1, 1, INT_MIN);//�������
unordered_map<size_t, int> HashTable;// ��ϣ�����洢���е���ֹ�ϣֵ

//�����������Ҫ��Ϊ�˵���
void Output() {
	cout << "DEBUG " << "      00 01 02 03 04 05 06 07 08 09 10 11" << endl;
	for (int i = 0; i <= 13; i++) {
		cout << "DEBUG " << setw(2) << i - 1 << ' ';
		for (int j = 0; j <= 13; j++) {
			if (Board[i][j] == 0) {
				cout << "   ";
			}
			else if (Board[i][j] == 1) {
				cout << "�� ";
			}
			else if (Board[i][j] == 2) {
				cout << "�� ";
			}
			else {
				cout << "00 ";
			}
		}
		cout << endl;
	}
}

// �Զ���ȽϺ��������� score �Ӹߵ�������
bool Compare(const Data& a, const Data& b) {
	return (a.Score > b.Score);
}

//���̳�ʼ��
void BoardInitialization() {
	Board[6][6] = 2;
	Board[7][6] = 1;
	Board[6][7] = 1;
	Board[7][7] = 2;
	//���Χǽ
	for (int i = 0; i <= 13; i++) {
		Board[0][i] = Board[13][i] = Board[i][0] = Board[i][13] = 3;
	}
}

//������λ���Ƿ��������,������������;
bool PutCheck(const int& x, const int& y) {
	if ((x >= 1) && (x <= 12) && (y >= 1) && (y <= 12) && (Board[x][y] == 0)) {
		return 1;
	}
	return 0;
}

//�������ӵ����
int CheckForFive(int One, int Two, int Three, int Four, int Five, int Opposite) {
	if ((One == Opposite) && (Two == Opposite) && (Three == Opposite) && (Four == Opposite) && (Five == Opposite)) {//����
		return 500000;
	}
	if ((One == Opposite) && (Two == Opposite) && (Three == Opposite) && (Four == Opposite) && (Five == 0)) {//����
		return 1000;
	}
	if ((One == 0) && (Two == Opposite) && (Three == Opposite) && (Four == Opposite) && (Five == Opposite)) {//����
		return 1000;
	}
	if ((One == Opposite) && (Two == Opposite) && (Three == 0) && (Four == Opposite) && (Five == Opposite)) {
		return 1000;
	}
	if ((One == Opposite) && (Two == 0) && (Three == Opposite) && (Four == Opposite) && (Five == Opposite)) {
		return 1000;
	}
	if ((One == Opposite) && (Two == Opposite) && (Three == Opposite) && (Four == 0) && (Five == Opposite)) {
		return 1000;
	}
	return 0;
}

//��������ӵ����
int CheckForSix(int One, int Two, int Three, int Four, int Five, int Six, int Opposite) {
	if ((One == 0) && (Two == Opposite) && (Three == Opposite) && (Four == Opposite) && (Five == Opposite) && (Six == 0)) {//����
		return 10000;
	}
	if ((One == 0) && (Two == Opposite) && (Three == Opposite) && (Four == Opposite) && (Five == 0) && (Six == 0)) {//����
		return 500;
	}
	if ((One == 0) && (Two == 0) && (Three == Opposite) && (Four == Opposite) && (Five == Opposite) && (Six == 0)) {//����
		return 500;
	}
	if ((One == 0) && (Two == Opposite) && (Three == Opposite) && (Four == 0) && (Five == Opposite) && (Six == 0)) {
		return 500;
	}
	if ((One == 0) && (Two == Opposite) && (Three == 0) && (Four == Opposite) && (Five == Opposite) && (Six == 0)) {
		return 500;
	}
	if ((One == 0) && (Two == 0) && (Three == Opposite) && (Four == Opposite) && (Five == 0) && (Six == 0)) {//���
		return 12;
	}
	if ((One == 0) && (Two == 0) && (Three == Opposite) && (Four == 0) && (Five == Opposite) && (Six == 0)) {
		return 12;
	}
	if ((One == 0) && (Two == Opposite) && (Three == 0) && (Four == Opposite) && (Five == 0) && (Six == 0)) {
		return 12;
	}
	if ((One == 0) && (Two == 0) && (Three == 0) && (Four == Opposite) && (Five == 0) && (Six == 0)) {
		return 2;
	}
	if ((One == 0) && (Two == 0) && (Three == Opposite) && (Four == 0) && (Five == 0) && (Six == 0)) {
		return 2;
	}
	return 0;
}

//���㵥��λ�õ�ĳһ������
int CountForSingle(int I, int J, int Opposite) {
	int Score = 0;
	for (int j = (J - 4), i = I; j <= J; j++) {
		if (j <= 7 && j >= 1) {//�������6��
			Score += CheckForSix(Board[i][j], Board[i][j + 1], Board[i][j + 2], Board[i][j + 3], Board[i][j + 4], Board[i][j + 5], Opposite);
		}
		if (j <= 8 && j >= 1) {//�������5��
			Score += CheckForFive(Board[i][j], Board[i][j + 1], Board[i][j + 2], Board[i][j + 3], Board[i][j + 4], Opposite);
		}
	}
	for (int i = (I - 4), j = J; i <= I; i++) {
		if (i <= 7 && i >= 1) {//�������6��
			Score += CheckForSix(Board[i][j], Board[i + 1][j], Board[i + 2][j], Board[i + 3][j], Board[i + 4][j], Board[i + 5][j], Opposite);
		}
		if (i <= 8 && i >= 1) {//�������5��
			Score += CheckForFive(Board[i][j], Board[i + 1][j], Board[i + 2][j], Board[i + 3][j], Board[i + 4][j], Opposite);
		}
	}
	for (int i = (I - 4), j = (J - 4); (i <= I && j <= J); i++, j++) {
		if ((i <= 7) && (j <= 7) && i >= 1 && j >= 1) {//�����±���6��
			Score += CheckForSix(Board[i][j], Board[i + 1][j + 1], Board[i + 2][j + 2], Board[i + 3][j + 3], Board[i + 4][j + 4], Board[i + 5][j + 5], Opposite);
		}
		if ((i <= 8) && (j <= 8) && i >= 1 && j >= 1) {//�����±���5��
			Score += CheckForFive(Board[i][j], Board[i + 1][j + 1], Board[i + 2][j + 2], Board[i + 3][j + 3], Board[i + 4][j + 4], Opposite);
		}
	}
	for (int i = (I - 4), j = (J + 4); (i <= I && j >= J); i++, j--) {
		if ((i <= 7) && (j >= 6) && i >= 1 && j <= 12) {//�����±���6��
			Score += CheckForSix(Board[i][j], Board[i + 1][j - 1], Board[i + 2][j - 2], Board[i + 3][j - 3], Board[i + 4][j - 4], Board[i + 5][j - 5], Opposite);
		}
		if ((i <= 8) && (j >= 5) && i >= 1 && j <= 12) {//�����±���5��
			Score += CheckForFive(Board[i][j], Board[i + 1][j - 1], Board[i + 2][j - 2], Board[i + 3][j - 3], Board[i + 4][j - 4], Opposite);
		}
	}
	return Score;
}

//�����ҷ�-�з�����������
int CountScore() {
	int  ScoreSelf = 0, ScoreEnemy = 0;
	for (int i = 1; i <= 12; i++) {
		for (int j = 1; j <= 12; j++) {//��������
			ScoreSelf += CountForSingle(i, j, Self);
			ScoreEnemy += CountForSingle(i, j, Enemy);
		}
	}
	return ScoreSelf - ScoreEnemy;
}

// �������������Ҫ����λ��
void AddImportantMove(vector<Data>& ImportantSeat) {
	for (int i = 1; i <= 12; i++) {
		for (int j = 1; j <= 12; j++) {//��������
			if (PutCheck(i, j)) {//�����������
				//����ģ��
				Board[i][j] = Self;
				int Score = CountForSingle(i, j, Self);
				//����ҷ����ڴ˴�����ֱ��ʤ��
				if (Score >= 500000) {
					Score += 500000;
				}
				Board[i][j] = Enemy;
				Score = Score + CountForSingle(i, j, Enemy);
				Board[i][j] = 0;//����ģ�����
				ImportantSeat.push_back(Data(i, j, Score));//����vector
			}
		}
	}
	// ������Ҫ�Է�������
	stable_sort(ImportantSeat.begin(), ImportantSeat.end(), Compare);
	if (ImportantSeat.size() >= NumberOfCheck) {
		ImportantSeat.resize(NumberOfCheck);//���е�
	}
	ImportantSeat.shrink_to_fit();//�޸��ڴ�
}

//����ʤ�����߼���ʧ��ʱǿ������������������������1
bool LockCheck(vector<Data>& ImportantSeat) {
	for (size_t i = 0; i < ImportantSeat.size(); i++) {//�������ڲ���
		if (ImportantSeat[i].Score >= 10000) {
			Best = ImportantSeat[i];
			return 1;
		}
	}
	return 0;
}

//���������ת��Ϊstring�������ϣֵ
size_t CalculateHash() {
	string boardState;
	for (int i = 1; i <= 12; i++) {
		for (int j = 1; j <= 12; j++) {
			boardState += to_string(Board[i][j]);
		}
	}
	return hash<string>()(boardState);
}

//MiniMax����
int MiniMax(int Opposite, int Alpha, int Beta, int Depth) {//alpha���½磬beta���Ͻ�	
	//��ʼģ��
	auto Duration = chrono::duration<double>(chrono::high_resolution_clock::now() - Start).count();//����ִ��ʱ��
	if ((Duration > Time) || (Depth <= 0)) {//ʱ��̫�����߲��ܼ�����ض�ִ��
		return CountScore();//���ص�ǰ�������
	}
	vector<Data> ImportantSeat;
	AddImportantMove(ImportantSeat);
	for (size_t i = 0; i < ImportantSeat.size(); i++) {//�������ڲ���
		auto Duration = chrono::duration<double>(chrono::high_resolution_clock::now() - Start).count();//����ִ��ʱ��
		if ((Duration > Time) || (Depth <= 0)) {//ʱ��̫�����߲��ܼ�����ض�ִ��
			return CountScore();//���ص�ǰ�������
		}
		//����ģ��
		int Score = 0;
		Board[ImportantSeat[i].X][ImportantSeat[i].Y] = Opposite;
		size_t CurrentBoard = CalculateHash();//���㵱ǰ��ֵĹ�ϣֵ
		// ����ϣ��
	    // ����ϣ�������û���
		if (HashTable.find(CurrentBoard) != HashTable.end()) {//����������
			Score = HashTable[CurrentBoard];//ֱ��ȡ����
		}
		else {//������ٵݹ鲢��¼
			Score = -MiniMax(3 - Opposite, -Beta, -Alpha, Depth - 1);//����ݹ鲩��
			HashTable[CurrentBoard] = Score;
		}
		Board[ImportantSeat[i].X][ImportantSeat[i].Y] = 0;//����ģ�����
		if (Score >= Beta) {//�ҵ������޻��õĲ�ѡ����Ϊ���˲�������
			return Beta;
		}
		if (Score > Alpha) {//�ҵ������޸��õģ�ѡ���õ�
			Alpha = Score;
		}
	}
	return Alpha;
}

//Ѱ���������
void FindBest() {
	vector<Data> ImportantSeat;
	AddImportantMove(ImportantSeat);
	//�����в����
	if (LockCheck(ImportantSeat)) {
		return;
	}
	//�������ڲ���
	for (size_t i = 0; i < ImportantSeat.size(); i++) {
		//����ģ��
		Board[ImportantSeat[i].X][ImportantSeat[i].Y] = Self;
		int Score = 0;
		if (Depth_ % 2) {//�����ż�����
			Score = -MiniMax(3 - Self, -2147483647, 2147483647, Depth_);//����ݹ鲩��
		}
		else {
			Score = MiniMax(3 - Self, -2147483647, 2147483647, Depth_);//����ݹ鲩��
		}
		Board[ImportantSeat[i].X][ImportantSeat[i].Y] = 0;//����ģ�����
		if (Score > Best.Score) {
			Best = Data(ImportantSeat[i].X, ImportantSeat[i].Y, Score);
		}
	}
}

//����
void PutPieces() {
	FindBest();
	Board[Best.X][Best.Y] = Self;//�����λ������
	cout << Best.X - 1 << ' ' << Best.Y - 1 << endl;//�ظ�
	HashTable.clear();
}

//���봦����
void DealInput(string& Input) {
	if (Input == "TURN") {//��̵ģ�����ǰ���ֹ�������Խ��
		Start = chrono::high_resolution_clock::now();//��ʼ��ʱ
		//����
		Best = Data(1, 1, INT_MIN);
		PutPieces();
	}
	else if (Input.substr(0, 5) == "START") {//ѡ����Ӫ
		if (Input[6] == '1') {
			Self = 1;
			Enemy = 2;
		}
		else if (Input[6] == '2') {
			Self = 2;
			Enemy = 1;
		}
		cout << "OK" << endl;
	}
	else if (Input.substr(0, 5) == "PLACE") {//�з�����
		//Ϊ�˴����λ������
		istringstream iss(Input.substr(6));
		int x = 0, y = 0;
		if (iss >> x >> y) {//��ȡ���겿�� 
			Board[x + 1][y + 1] = Enemy;
		}
	}
	return;
}

int main() {
	string Input;
	BoardInitialization();
	while (getline(cin, Input)) {
		if (Input.substr(0, 3) == "END") {//����ENDָ����˳�
			break;
		}
		DealInput(Input);
	}
	return 0;
}