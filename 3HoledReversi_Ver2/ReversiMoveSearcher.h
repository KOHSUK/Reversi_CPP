#pragma once
#include "Board.h"
#include <time.h>
#include <stdlib.h>
#include <vector>


const int CHILD_MAX = 61;
const int NODE_MAX = 10000;
const int UCT_LOOP = 1000;

typedef struct child {
	int move;
	int games;
	double rate;
	int nextNode;
} CHILD;

typedef struct node {
	int child_num;
	CHILD child[CHILD_MAX];
	int games_num;
}NODE;

class ReversiMoveSearcher {
private:

	int positionWeight[64] = { 30, -12,   0,  -1,  -1,   0,  -12,  30,
							  -12, -15,  -3,  -3,  -3,  -3,  -15, -12,
								0,  -3,   0,  -1,  -1,   0,   -3,   0,
							   -1,  -3,  -1,  -1,  -1,  -1,   -3,  -1,
							   -1,  -3,  -1,  -1,  -1,  -1,   -3,  -1,
								0,  -3,   0,  -1,  -1,   0,   -3,   0,
							  -12, -15,  -3,  -3,  -3,  -3,  -15, -12,
							   30, -12,   0,  -1,  -1,   0,  -12,  30 };

	Board board;

	NODE node[NODE_MAX];

	int node_num = 0;

	const int NODE_EMPTY = -1;

	int opponentsColor;

	int myColor;
	
	int createNode();

	void addChild(NODE *pN, int move);

	int searchUCT(int color, int node_n);

	int playout(int color);

	int moveAtRandom(int color);

	std::vector<int> getAvailableMoves(BitBoard m, int color);

public:

	inline int flip(int color);

	int selectBestMove();

	void set_Board(BitBoard wb, BitBoard bb, BitBoard h);

	void set_Color(int myColor);

	int move(int move, int color);

	void printb();

	ReversiMoveSearcher();

	~ReversiMoveSearcher();

};