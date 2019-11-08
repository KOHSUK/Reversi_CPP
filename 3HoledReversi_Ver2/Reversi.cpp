#include "pch.h"
#include "Reversi.h"
#include <string>
#include <time.h>
#define PLAYOUT_COUNT 1000

//-------------private------------------
void Reversi::applyOpponentsMove(wchar_t * move) {
	int pos = getPos(move);

	if ( pos != INVALID_MOVE ) searcher.move(pos, opponentsColor);

	searcher.printb();

}

int Reversi::setBoard(wchar_t * cBoard) {
	int i;
	BitBoard wb = 0ull;
	BitBoard bb = 0ull;
	BitBoard h = 0ull;

	wchar_t buffer[BUFSIZ];
	wcscpy_s(buffer, BUFSIZ, cBoard);

	for (i = 0; i < START_LEN; i++) {
		if (i == 2) {
			//if(cBoard[i] == L'B')
			switch (buffer[i]) {
			case L'B':
				myColor = BLACK;
				opponentsColor = WHITE;
				break;
			case L'W':
				myColor = WHITE;
				opponentsColor = BLACK;
				break;
			default:
				myColor = 0;
			}
		}
		else if (i == 3) {
			if (buffer[i] == L'1') {
				currentTurn = MYTURN;
			}
			else {
				currentTurn = OPPONENTSTURN;
			}
		}
		else if (i > 3) {
			BitBoard mask = 1ull;
			mask <<= (i - 4);
			switch (buffer[i]) {
			case L'B':
				bb |= mask;
				break;
			case L'W':
				wb |= mask;
				break;
			case L'H':
				h |= mask;
				break;
			default:
				break;
			}
		}
	}

	searcher.set_Board(wb, bb, h);
	searcher.set_Color(myColor);

	wprintf_s(L"\n");
	wprintf_s(L"My Disk Color : ");
	myColor == BLACK ? wprintf_s(L"BLACK[O]\n") : wprintf_s(L"WHITE[@]\n");
	wprintf_s(L"\n");

	searcher.printb();

	return currentTurn;
}



//-------------public------------------

wchar_t* Reversi::getNextMove() {

	wchar_t static NextMove[MYTURN_LEN + 1];

	int move = searcher.selectBestMove();

	if (move != INVALID_MOVE) searcher.move(move, myColor);
	searcher.printb();

	wchar_t wMove[MYTURN_LEN + 1] = { L'0' };

	wMove[0] = L'0';
	wMove[1] = L'2';
	if (myColor == WHITE) {
		wMove[2] = L'W';
	}
	else {
		wMove[2] = L'B';
	}

	wchar_t* charPos = getCharPos(move);
	wcscat_s(wMove, charPos);

	wcscpy_s(NextMove, wMove);

	return NextMove;

}

MessageType Reversi::processMessage(wchar_t * message) {
	size_t len = wcslen(message);
	if (len == START_LEN) {

		int turn = setBoard(message);

		if (turn == MYTURN) {
			return MessageType::GameStarts_MyTurn;
		}
		else if (turn == OPPONENTSTURN) {
			return MessageType::GameStarts_OpponentsTurn;
		}
		else {
			return MessageType::NONE;
		}

	}
	else if (len == MYTURN_LEN) {

		return MessageType::MyMove;

	}
	else if (len == OPPONENTSTURN_LEN) {

		switch ((int)message[5] - 48){
		case CONTINUE:
			applyOpponentsMove(message);
			return MessageType::OpponentsMove;
			break;
		case WIN:
			return MessageType::Win;
			break;
		case LOSE:
			return MessageType::Lose;
			break;
		case DRAW:
			return MessageType::Draw;
			break;
		default:
			return MessageType::NONE;
		}

	}
	else {

		return MessageType::NONE;

	}
}

Reversi::Reversi(wchar_t * cBoard) {
	srand((unsigned int)time(NULL));
	myColor = BLACK;
	opponentsColor = WHITE;
}

Reversi::Reversi()
{
	srand((unsigned int)time(NULL));
	myColor = BLACK;
	opponentsColor = WHITE;
}


Reversi::~Reversi()
{
}
