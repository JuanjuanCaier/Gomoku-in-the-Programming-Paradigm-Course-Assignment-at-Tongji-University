#include<iostream>
#include<string>//读入
#include<chrono>//计时
#include<sstream>//读入
#include<algorithm>//排序
#include<vector>//找最佳落子
#include<unordered_map>//哈希表
#include<functional>
#include<iomanip>//输出

using namespace std;

//储存信息
#define NumberOfCheck 5//考虑的落子数目
#define Depth_ 5//递归深度
#define Time 1.2//极限时间（一般达不到）
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
int Board[14][14] = { 0 };//棋盘，0为空格，1为黑子，2为白子,3为棋盘边界,主要是防止越界操作
int Self = 0, Enemy = 0;//阵营
auto Start = chrono::high_resolution_clock::now();//计时，不过不在此处开始，仅作单位推断
Data Best(1, 1, INT_MIN);//最佳落子
unordered_map<size_t, int> HashTable;// 哈希表来存储已有的棋局哈希值

//输出操作，主要是为了调试
void Output() {
	cout << "DEBUG " << "      00 01 02 03 04 05 06 07 08 09 10 11" << endl;
	for (int i = 0; i <= 13; i++) {
		cout << "DEBUG " << setw(2) << i - 1 << ' ';
		for (int j = 0; j <= 13; j++) {
			if (Board[i][j] == 0) {
				cout << "   ";
			}
			else if (Board[i][j] == 1) {
				cout << "○ ";
			}
			else if (Board[i][j] == 2) {
				cout << "× ";
			}
			else {
				cout << "00 ";
			}
		}
		cout << endl;
	}
}

// 自定义比较函数，根据 score 从高到低排序
bool Compare(const Data& a, const Data& b) {
	return (a.Score > b.Score);
}

//棋盘初始化
void BoardInitialization() {
	Board[6][6] = 2;
	Board[7][6] = 1;
	Board[6][7] = 1;
	Board[7][7] = 2;
	//添加围墙
	for (int i = 0; i <= 13; i++) {
		Board[0][i] = Board[13][i] = Board[i][0] = Board[i][13] = 3;
	}
}

//检查给定位置是否可以落子,输入棋盘坐标;
bool PutCheck(const int& x, const int& y) {
	if ((x >= 1) && (x <= 12) && (y >= 1) && (y <= 12) && (Board[x][y] == 0)) {
		return 1;
	}
	return 0;
}

//检查五个子的情况
int CheckForFive(int One, int Two, int Three, int Four, int Five, int Opposite) {
	if ((One == Opposite) && (Two == Opposite) && (Three == Opposite) && (Four == Opposite) && (Five == Opposite)) {//连五
		return 500000;
	}
	if ((One == Opposite) && (Two == Opposite) && (Three == Opposite) && (Four == Opposite) && (Five == 0)) {//眠四
		return 1000;
	}
	if ((One == 0) && (Two == Opposite) && (Three == Opposite) && (Four == Opposite) && (Five == Opposite)) {//眠四
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

//检查六个子的情况
int CheckForSix(int One, int Two, int Three, int Four, int Five, int Six, int Opposite) {
	if ((One == 0) && (Two == Opposite) && (Three == Opposite) && (Four == Opposite) && (Five == Opposite) && (Six == 0)) {//活四
		return 10000;
	}
	if ((One == 0) && (Two == Opposite) && (Three == Opposite) && (Four == Opposite) && (Five == 0) && (Six == 0)) {//活三
		return 500;
	}
	if ((One == 0) && (Two == 0) && (Three == Opposite) && (Four == Opposite) && (Five == Opposite) && (Six == 0)) {//活三
		return 500;
	}
	if ((One == 0) && (Two == Opposite) && (Three == Opposite) && (Four == 0) && (Five == Opposite) && (Six == 0)) {
		return 500;
	}
	if ((One == 0) && (Two == Opposite) && (Three == 0) && (Four == Opposite) && (Five == Opposite) && (Six == 0)) {
		return 500;
	}
	if ((One == 0) && (Two == 0) && (Three == Opposite) && (Four == Opposite) && (Five == 0) && (Six == 0)) {//活二
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

//计算单个位置的某一方分数
int CountForSingle(int I, int J, int Opposite) {
	int Score = 0;
	for (int j = (J - 4), i = I; j <= J; j++) {
		if (j <= 7 && j >= 1) {//横向遍历6子
			Score += CheckForSix(Board[i][j], Board[i][j + 1], Board[i][j + 2], Board[i][j + 3], Board[i][j + 4], Board[i][j + 5], Opposite);
		}
		if (j <= 8 && j >= 1) {//横向遍历5子
			Score += CheckForFive(Board[i][j], Board[i][j + 1], Board[i][j + 2], Board[i][j + 3], Board[i][j + 4], Opposite);
		}
	}
	for (int i = (I - 4), j = J; i <= I; i++) {
		if (i <= 7 && i >= 1) {//纵向遍历6子
			Score += CheckForSix(Board[i][j], Board[i + 1][j], Board[i + 2][j], Board[i + 3][j], Board[i + 4][j], Board[i + 5][j], Opposite);
		}
		if (i <= 8 && i >= 1) {//纵向遍历5子
			Score += CheckForFive(Board[i][j], Board[i + 1][j], Board[i + 2][j], Board[i + 3][j], Board[i + 4][j], Opposite);
		}
	}
	for (int i = (I - 4), j = (J - 4); (i <= I && j <= J); i++, j++) {
		if ((i <= 7) && (j <= 7) && i >= 1 && j >= 1) {//向右下遍历6子
			Score += CheckForSix(Board[i][j], Board[i + 1][j + 1], Board[i + 2][j + 2], Board[i + 3][j + 3], Board[i + 4][j + 4], Board[i + 5][j + 5], Opposite);
		}
		if ((i <= 8) && (j <= 8) && i >= 1 && j >= 1) {//向右下遍历5子
			Score += CheckForFive(Board[i][j], Board[i + 1][j + 1], Board[i + 2][j + 2], Board[i + 3][j + 3], Board[i + 4][j + 4], Opposite);
		}
	}
	for (int i = (I - 4), j = (J + 4); (i <= I && j >= J); i++, j--) {
		if ((i <= 7) && (j >= 6) && i >= 1 && j <= 12) {//向左下遍历6子
			Score += CheckForSix(Board[i][j], Board[i + 1][j - 1], Board[i + 2][j - 2], Board[i + 3][j - 3], Board[i + 4][j - 4], Board[i + 5][j - 5], Opposite);
		}
		if ((i <= 8) && (j >= 5) && i >= 1 && j <= 12) {//向左下遍历5子
			Score += CheckForFive(Board[i][j], Board[i + 1][j - 1], Board[i + 2][j - 2], Board[i + 3][j - 3], Board[i + 4][j - 4], Opposite);
		}
	}
	return Score;
}

//计算我方-敌方分数并返回
int CountScore() {
	int  ScoreSelf = 0, ScoreEnemy = 0;
	for (int i = 1; i <= 12; i++) {
		for (int j = 1; j <= 12; j++) {//遍历棋盘
			ScoreSelf += CountForSingle(i, j, Self);
			ScoreEnemy += CountForSingle(i, j, Enemy);
		}
	}
	return ScoreSelf - ScoreEnemy;
}

// 函数用于添加重要落子位置
void AddImportantMove(vector<Data>& ImportantSeat) {
	for (int i = 1; i <= 12; i++) {
		for (int j = 1; j <= 12; j++) {//遍历棋盘
			if (PutCheck(i, j)) {//如果可以落子
				//落子模拟
				Board[i][j] = Self;
				int Score = CountForSingle(i, j, Self);
				//如果我方落在此处可以直接胜利
				if (Score >= 500000) {
					Score += 500000;
				}
				Board[i][j] = Enemy;
				Score = Score + CountForSingle(i, j, Enemy);
				Board[i][j] = 0;//落子模拟结束
				ImportantSeat.push_back(Data(i, j, Score));//放入vector
			}
		}
	}
	// 按照重要性分数排序
	stable_sort(ImportantSeat.begin(), ImportantSeat.end(), Compare);
	if (ImportantSeat.size() >= NumberOfCheck) {
		ImportantSeat.resize(NumberOfCheck);//剪切到
	}
	ImportantSeat.shrink_to_fit();//修改内存
}

//即将胜利或者即将失败时强制锁死，如果满足条件就输出1
bool LockCheck(vector<Data>& ImportantSeat) {
	for (size_t i = 0; i < ImportantSeat.size(); i++) {//最优子内查找
		if (ImportantSeat[i].Score >= 10000) {
			Best = ImportantSeat[i];
			return 1;
		}
	}
	return 0;
}

//将已有棋局转换为string并计算哈希值
size_t CalculateHash() {
	string boardState;
	for (int i = 1; i <= 12; i++) {
		for (int j = 1; j <= 12; j++) {
			boardState += to_string(Board[i][j]);
		}
	}
	return hash<string>()(boardState);
}

//MiniMax博弈
int MiniMax(int Opposite, int Alpha, int Beta, int Depth) {//alpha是下界，beta是上界	
	//开始模拟
	auto Duration = chrono::duration<double>(chrono::high_resolution_clock::now() - Start).count();//测量执行时间
	if ((Duration > Time) || (Depth <= 0)) {//时间太长或者不能继续则截断执行
		return CountScore();//返回当前局面分数
	}
	vector<Data> ImportantSeat;
	AddImportantMove(ImportantSeat);
	for (size_t i = 0; i < ImportantSeat.size(); i++) {//最优子内查找
		auto Duration = chrono::duration<double>(chrono::high_resolution_clock::now() - Start).count();//测量执行时间
		if ((Duration > Time) || (Depth <= 0)) {//时间太长或者不能继续则截断执行
			return CountScore();//返回当前局面分数
		}
		//落子模拟
		int Score = 0;
		Board[ImportantSeat[i].X][ImportantSeat[i].Y] = Opposite;
		size_t CurrentBoard = CalculateHash();//计算当前棋局的哈希值
		// 检查哈希表
	    // 检查哈希表（加入置换表）
		if (HashTable.find(CurrentBoard) != HashTable.end()) {//如果是老棋局
			Score = HashTable[CurrentBoard];//直接取分数
		}
		else {//新棋局再递归并记录
			Score = -MiniMax(3 - Opposite, -Beta, -Alpha, Depth - 1);//进入递归博弈
			HashTable[CurrentBoard] = Score;
		}
		Board[ImportantSeat[i].X][ImportantSeat[i].Y] = 0;//落子模拟结束
		if (Score >= Beta) {//找到比上限还好的不选，因为敌人不会容许
			return Beta;
		}
		if (Score > Alpha) {//找到比下限更好的，选更好的
			Alpha = Score;
		}
	}
	return Alpha;
}

//寻找最佳落子
void FindBest() {
	vector<Data> ImportantSeat;
	AddImportantMove(ImportantSeat);
	//如果威胁生命
	if (LockCheck(ImportantSeat)) {
		return;
	}
	//最优子内查找
	for (size_t i = 0; i < ImportantSeat.size(); i++) {
		//落子模拟
		Board[ImportantSeat[i].X][ImportantSeat[i].Y] = Self;
		int Score = 0;
		if (Depth_ % 2) {//如果是偶数深度
			Score = -MiniMax(3 - Self, -2147483647, 2147483647, Depth_);//进入递归博弈
		}
		else {
			Score = MiniMax(3 - Self, -2147483647, 2147483647, Depth_);//进入递归博弈
		}
		Board[ImportantSeat[i].X][ImportantSeat[i].Y] = 0;//落子模拟结束
		if (Score > Best.Score) {
			Best = Data(ImportantSeat[i].X, ImportantSeat[i].Y, Score);
		}
	}
}

//落子
void PutPieces() {
	FindBest();
	Board[Best.X][Best.Y] = Self;//在最佳位置落子
	cout << Best.X - 1 << ' ' << Best.Y - 1 << endl;//回复
	HashTable.clear();
}

//输入处理函数
void DealInput(string& Input) {
	if (Input == "TURN") {//最短的，放在前面防止后续检测越界
		Start = chrono::high_resolution_clock::now();//开始计时
		//重置
		Best = Data(1, 1, INT_MIN);
		PutPieces();
	}
	else if (Input.substr(0, 5) == "START") {//选择阵营
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
	else if (Input.substr(0, 5) == "PLACE") {//敌方落子
		//为了处理二位数坐标
		istringstream iss(Input.substr(6));
		int x = 0, y = 0;
		if (iss >> x >> y) {//读取坐标部分 
			Board[x + 1][y + 1] = Enemy;
		}
	}
	return;
}

int main() {
	string Input;
	BoardInitialization();
	while (getline(cin, Input)) {
		if (Input.substr(0, 3) == "END") {//读入END指令就退出
			break;
		}
		DealInput(Input);
	}
	return 0;
}