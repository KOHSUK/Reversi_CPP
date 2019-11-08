#include "pch.h"
#include "ReversiMoveSearcher.h"

//-------------private------------------

void ReversiMoveSearcher::addChild(NODE *pN, int move) {
	int n = pN->child_num;
	pN->child[n].move = move;
	pN->child[n].games = 0;
	pN->child[n].rate = 0;
	pN->child[n].nextNode = NODE_EMPTY;
	pN->child_num++;
}

//******************************
//現在の盤面をルートノードとして作成し、可能な手をChildノードとして追加する
//******************************
int ReversiMoveSearcher::createNode() {
	if (node_num == NODE_MAX) {
		wprintf_s(L"node count over exception!\n");
		exit(0);
	}

	//新しいノード
	NODE *pN = &node[node_num];
	pN->child_num = 0;

	BitBoard avlBoard = board.getValidMoves(myColor);
	std::vector<int> moves = getAvailableMoves(avlBoard, myColor);
	
	//現在の局面(NODE)で可能な手(CHILD)を作成
	for (auto itr = moves.begin(); itr != moves.end(); ++itr) {
		addChild(pN, *itr);
	}
	//パスも追加
	addChild(pN, PASS);

	node_num++;
	return node_num - 1;
}

int ReversiMoveSearcher::searchUCT(int color, int node_n) {

	//探索を開始するノード
	NODE *pN = &node[node_n];

	int select = -1;
	double max_ucb = -999;

	//可能な手の数だけループ
	for (int i = 0; i < pN->child_num; i++) {
		CHILD *c = &pN->child[i];
		double ucb = 0;

		if (c->games == 0) {
			ucb = 10000 + rand();
		}
		else {
			const double C = 0.31;
			ucb = c->rate + C * sqrt(log(pN->games_num) / c->games);
		}

		//一番UCB値の高い手を選択する
		if (ucb > max_ucb) {
			max_ucb = ucb;
			select = i;
		}
	}

	//選択出来なかった場合エラー
	if (select == -1) {
		wprintf_s(L"Error on <searchUCT>\n");
		exit(0);
	}

	//UCB値の最も高い手で打つ
	CHILD *c = &pN->child[select];
	board.move(c->move, color);

	//ゲーム木の展開
	int win;
	if (c->games == 0) {
		win = -playout(flip(color));
	}
	else {
		if (c->nextNode == NODE_EMPTY) c->nextNode = createNode();
		win = -searchUCT(flip(color), c->nextNode);
	}

	c->rate = (c->rate * c->games + win) / ((int)(c->games + 1));
	c->games++;
	pN->games_num++;
	return win;
}

int ReversiMoveSearcher::playout(int color) {
	int num = 0;
	int passCount = 0;
	int myColor = flip(color);

	while (passCount < 2) {

		if (moveAtRandom(color) == 0) passCount++;

		color = flip(color);
	}

	return board.getScore(color);

}

int ReversiMoveSearcher::moveAtRandom(int color) {
	BitBoard m = board.getValidMoves(color);
	if (m == 0ull) return 0;
	int cnt = count(m);
	std::vector<int> mv = getAvailableMoves(m, color);
	int score, maxScore = -10000;
	int maxIdx = mv[0];
	for (auto itr = mv.begin(); itr != mv.end(); ++itr) {
		score = positionWeight[*itr] + rand() % 10;
		if (score > maxScore) {
			maxScore = score;
			maxIdx = *itr;
		}
	}
	return board.move(maxIdx, color);
}

std::vector<int> ReversiMoveSearcher::getAvailableMoves(BitBoard m, int color) {
	std::vector<int> moves;

	if ((0b0000000000000000000000000000000000000000000000000000000000000001ull & m) > 0ull) moves.push_back(0);
	if ((0b0000000000000000000000000000000000000000000000000000000000000010ull & m) > 0ull) moves.push_back(1);
	if ((0b0000000000000000000000000000000000000000000000000000000000000100ull & m) > 0ull) moves.push_back(2);
	if ((0b0000000000000000000000000000000000000000000000000000000000001000ull & m) > 0ull) moves.push_back(3);
	if ((0b0000000000000000000000000000000000000000000000000000000000010000ull & m) > 0ull) moves.push_back(4);
	if ((0b0000000000000000000000000000000000000000000000000000000000100000ull & m) > 0ull) moves.push_back(5);
	if ((0b0000000000000000000000000000000000000000000000000000000001000000ull & m) > 0ull) moves.push_back(6);
	if ((0b0000000000000000000000000000000000000000000000000000000010000000ull & m) > 0ull) moves.push_back(7);
	if ((0b0000000000000000000000000000000000000000000000000000000100000000ull & m) > 0ull) moves.push_back(8);
	if ((0b0000000000000000000000000000000000000000000000000000001000000000ull & m) > 0ull) moves.push_back(9);
	if ((0b0000000000000000000000000000000000000000000000000000010000000000ull & m) > 0ull) moves.push_back(10);
	if ((0b0000000000000000000000000000000000000000000000000000100000000000ull & m) > 0ull) moves.push_back(11);
	if ((0b0000000000000000000000000000000000000000000000000001000000000000ull & m) > 0ull) moves.push_back(12);
	if ((0b0000000000000000000000000000000000000000000000000010000000000000ull & m) > 0ull) moves.push_back(13);
	if ((0b0000000000000000000000000000000000000000000000000100000000000000ull & m) > 0ull) moves.push_back(14);
	if ((0b0000000000000000000000000000000000000000000000001000000000000000ull & m) > 0ull) moves.push_back(15);
	if ((0b0000000000000000000000000000000000000000000000010000000000000000ull & m) > 0ull) moves.push_back(16);
	if ((0b0000000000000000000000000000000000000000000000100000000000000000ull & m) > 0ull) moves.push_back(17);
	if ((0b0000000000000000000000000000000000000000000001000000000000000000ull & m) > 0ull) moves.push_back(18);
	if ((0b0000000000000000000000000000000000000000000010000000000000000000ull & m) > 0ull) moves.push_back(19);
	if ((0b0000000000000000000000000000000000000000000100000000000000000000ull & m) > 0ull) moves.push_back(20);
	if ((0b0000000000000000000000000000000000000000001000000000000000000000ull & m) > 0ull) moves.push_back(21);
	if ((0b0000000000000000000000000000000000000000010000000000000000000000ull & m) > 0ull) moves.push_back(22);
	if ((0b0000000000000000000000000000000000000000100000000000000000000000ull & m) > 0ull) moves.push_back(23);
	if ((0b0000000000000000000000000000000000000001000000000000000000000000ull & m) > 0ull) moves.push_back(24);
	if ((0b0000000000000000000000000000000000000010000000000000000000000000ull & m) > 0ull) moves.push_back(25);
	if ((0b0000000000000000000000000000000000000100000000000000000000000000ull & m) > 0ull) moves.push_back(26);
	if ((0b0000000000000000000000000000000000001000000000000000000000000000ull & m) > 0ull) moves.push_back(27);
	if ((0b0000000000000000000000000000000000010000000000000000000000000000ull & m) > 0ull) moves.push_back(28);
	if ((0b0000000000000000000000000000000000100000000000000000000000000000ull & m) > 0ull) moves.push_back(29);
	if ((0b0000000000000000000000000000000001000000000000000000000000000000ull & m) > 0ull) moves.push_back(30);
	if ((0b0000000000000000000000000000000010000000000000000000000000000000ull & m) > 0ull) moves.push_back(31);
	if ((0b0000000000000000000000000000000100000000000000000000000000000000ull & m) > 0ull) moves.push_back(32);
	if ((0b0000000000000000000000000000001000000000000000000000000000000000ull & m) > 0ull) moves.push_back(33);
	if ((0b0000000000000000000000000000010000000000000000000000000000000000ull & m) > 0ull) moves.push_back(34);
	if ((0b0000000000000000000000000000100000000000000000000000000000000000ull & m) > 0ull) moves.push_back(35);
	if ((0b0000000000000000000000000001000000000000000000000000000000000000ull & m) > 0ull) moves.push_back(36);
	if ((0b0000000000000000000000000010000000000000000000000000000000000000ull & m) > 0ull) moves.push_back(37);
	if ((0b0000000000000000000000000100000000000000000000000000000000000000ull & m) > 0ull) moves.push_back(38);
	if ((0b0000000000000000000000001000000000000000000000000000000000000000ull & m) > 0ull) moves.push_back(39);
	if ((0b0000000000000000000000010000000000000000000000000000000000000000ull & m) > 0ull) moves.push_back(40);
	if ((0b0000000000000000000000100000000000000000000000000000000000000000ull & m) > 0ull) moves.push_back(41);
	if ((0b0000000000000000000001000000000000000000000000000000000000000000ull & m) > 0ull) moves.push_back(42);
	if ((0b0000000000000000000010000000000000000000000000000000000000000000ull & m) > 0ull) moves.push_back(43);
	if ((0b0000000000000000000100000000000000000000000000000000000000000000ull & m) > 0ull) moves.push_back(44);
	if ((0b0000000000000000001000000000000000000000000000000000000000000000ull & m) > 0ull) moves.push_back(45);
	if ((0b0000000000000000010000000000000000000000000000000000000000000000ull & m) > 0ull) moves.push_back(46);
	if ((0b0000000000000000100000000000000000000000000000000000000000000000ull & m) > 0ull) moves.push_back(47);
	if ((0b0000000000000001000000000000000000000000000000000000000000000000ull & m) > 0ull) moves.push_back(48);
	if ((0b0000000000000010000000000000000000000000000000000000000000000000ull & m) > 0ull) moves.push_back(49);
	if ((0b0000000000000100000000000000000000000000000000000000000000000000ull & m) > 0ull) moves.push_back(50);
	if ((0b0000000000001000000000000000000000000000000000000000000000000000ull & m) > 0ull) moves.push_back(51);
	if ((0b0000000000010000000000000000000000000000000000000000000000000000ull & m) > 0ull) moves.push_back(52);
	if ((0b0000000000100000000000000000000000000000000000000000000000000000ull & m) > 0ull) moves.push_back(53);
	if ((0b0000000001000000000000000000000000000000000000000000000000000000ull & m) > 0ull) moves.push_back(54);
	if ((0b0000000010000000000000000000000000000000000000000000000000000000ull & m) > 0ull) moves.push_back(55);
	if ((0b0000000100000000000000000000000000000000000000000000000000000000ull & m) > 0ull) moves.push_back(56);
	if ((0b0000001000000000000000000000000000000000000000000000000000000000ull & m) > 0ull) moves.push_back(57);
	if ((0b0000010000000000000000000000000000000000000000000000000000000000ull & m) > 0ull) moves.push_back(58);
	if ((0b0000100000000000000000000000000000000000000000000000000000000000ull & m) > 0ull) moves.push_back(59);
	if ((0b0001000000000000000000000000000000000000000000000000000000000000ull & m) > 0ull) moves.push_back(60);
	if ((0b0010000000000000000000000000000000000000000000000000000000000000ull & m) > 0ull) moves.push_back(61);
	if ((0b0100000000000000000000000000000000000000000000000000000000000000ull & m) > 0ull) moves.push_back(62);
	if ((0b1000000000000000000000000000000000000000000000000000000000000000ull & m) > 0ull) moves.push_back(63);
	return moves;
}

//-------------public------------------
int ReversiMoveSearcher::selectBestMove() {
	node_num = 0;
	int nextNode = createNode();

	int i;
	for (i = 0; i < UCT_LOOP; i++) {
		board.save();

		searchUCT(myColor, nextNode);

		board.rollback();
	}

	NODE* pN = &node[nextNode];
	int best_child = pN->child_num - 1;
	int max = -999;
	for (i = 0; i < pN->child_num; i++) {
		CHILD *c = &pN->child[i];
		if (c->games > max) {
			best_child = i;
			max = c->games;
		}
	}

	int bestMove = pN->child[best_child].move;

	return bestMove;

}

void ReversiMoveSearcher::set_Board(BitBoard wb, BitBoard bb, BitBoard h) {
	board.set_Board(wb, bb, h);
}

int ReversiMoveSearcher::move(int move, int color) {
	return board.move(move, color);
}

void ReversiMoveSearcher::printb() {
	board.printb();
}

int ReversiMoveSearcher::flip(int color) {
	return 3 - color;
}

void ReversiMoveSearcher::set_Color(int color) {
	myColor = color;
	opponentsColor = flip(color);
}

ReversiMoveSearcher::ReversiMoveSearcher(): node() {
	srand((unsigned int)time(NULL));
	myColor = BLACK;
	opponentsColor = WHITE;
}

ReversiMoveSearcher::~ReversiMoveSearcher() {

}