#include "pch.h"
#include <cstring>
#include "Board.h"


//---------------<private>----------------------

BitBoard Board::translate(BitBoard pos, int index){
    switch (index) {
    case 0://left
        return (pos >> 1) & 0xfefefefefefefefe;
        break;
    case 1://right
        return (pos << 1) & 0x7f7f7f7f7f7f7f7f;
        break;
    case 2://upper
        return (pos >> 8) & 0x00ffffffffffffff;
        break;
    case 3://lower
        return (pos << 8) & 0xffffffffffffff00;
        break;
    case 4://upper left
        return (pos >> 9) & 0xfefefefefefefe00;
        break;
    case 5://upper right
        return (pos >> 7) & 0x7f7f7f7f7f7f7f00;
        break;
    case 6://lower left
        return (pos << 7) & 0x00fefefefefefefe;
        break;
    case 7://lower right
        return (pos << 9) & 0x007f7f7f7f7f7f7f;
        break;
    default:
        return 0ull;
        break;
    }
}

//---------------<public>----------------------
Board::Board(){
    wBoard = 0x0000001008000000;
	wSavePoint = wBoard;
    bBoard = 0x0000000810000000;
	bSavePoint = bBoard;
    holes = 0x0000000000000000;
}

Board::Board(BitBoard w, BitBoard b){
    wBoard = w;
	wSavePoint = wBoard;
    bBoard = b;
	bSavePoint = bBoard;
    holes = 0b0000000000000000000000000000000000000000000000000000000000000000;
}

Board::Board(BitBoard w, BitBoard b, BitBoard h){
    wBoard = w;
	wSavePoint = wBoard;
    bBoard = b;
	bSavePoint = bBoard;
    holes = h;  
}

BitBoard Board::getValidMoves(int color){
    BitBoard *mine = get_Board(color);
    BitBoard *opponents = get_Board(3 - color);
    BitBoard result = 0ull;
    BitBoard horizontalMask = 0x7e7e7e7e7e7e7e7e;
    BitBoard verticalMak = 0x00ffffffffffff00;
    BitBoard marginalMask = 0x007e7e7e7e7e7e00;
    BitBoard temp, target = 0ull;
    BitBoard emptyPlaces = ~( *mine | *opponents );

    target = horizontalMask & *opponents;

    //left
    temp = target & ( *mine >> 1 );
    temp |= target & ( temp >> 1 );
    temp |= target & ( temp >> 1 );
    temp |= target & ( temp >> 1 );
    temp |= target & ( temp >> 1 );
    temp |= target & ( temp >> 1 );
    result |= (temp >> 1) & emptyPlaces;

    //right
    temp = target & ( *mine << 1 );
    temp |= target & ( temp << 1 );
    temp |= target & ( temp << 1 );
    temp |= target & ( temp << 1 );
    temp |= target & ( temp << 1 );
    temp |= target & ( temp << 1 );
    result |= (temp << 1) & emptyPlaces;

    target = verticalMak & *opponents;

    //upper
    temp = target & ( *mine >> 8 );
    temp |= target & ( temp >> 8 );
    temp |= target & ( temp >> 8 );
    temp |= target & ( temp >> 8 );
    temp |= target & ( temp >> 8 );
    temp |= target & ( temp >> 8 );
    result |= (temp >> 8) & emptyPlaces;

    //lower
    temp = target & ( *mine << 8 );
    temp |= target & ( temp << 8 );
    temp |= target & ( temp << 8 );
    temp |= target & ( temp << 8 );
    temp |= target & ( temp << 8 );
    temp |= target & ( temp << 8 );
    result |= (temp << 8) & emptyPlaces;

    target = marginalMask & *opponents;

    //upper left
    temp = target & ( *mine >> 9 );
    temp |= target & ( temp >> 9 );
    temp |= target & ( temp >> 9 );
    temp |= target & ( temp >> 9 );
    temp |= target & ( temp >> 9 );
	temp |= target & (temp >> 9);
    result |= (temp >> 9) & emptyPlaces;

    //upper right
    temp = target & ( *mine >> 7 );
    temp |= target & ( temp >> 7 );
    temp |= target & ( temp >> 7 );
    temp |= target & ( temp >> 7 );
    temp |= target & ( temp >> 7 );
	temp |= target & (temp >> 7);
    result |= (temp >> 7) & emptyPlaces;

    //lower left
    temp = target & ( *mine << 7 );
    temp |= target & ( temp << 7 );
    temp |= target & ( temp << 7 );
    temp |= target & ( temp << 7 );
    temp |= target & ( temp << 7 );
	temp |= target & (temp << 7);
    result |= (temp << 7) & emptyPlaces;

    //lower right
    temp = target & ( *mine << 9 );
    temp |= target & ( temp << 9 );
    temp |= target & ( temp << 9 );
    temp |= target & ( temp << 9 );
    temp |= target & ( temp << 9 );
	temp |= target & (temp << 9);
    result |= (temp << 9) & emptyPlaces;

	result &= (~holes);

    return result;
}

//int Board::move(int pos, int color){
//    BitBoard move = 1ull << pos;
//    BitBoard *mine = get_Board(color);
//    BitBoard *opponents = get_Board(3 - color);
//    if(((*mine | *opponents) & move) > 0ull) return 0;
//
//    BitBoard result = 0,temp;
//    int i;
//    for(i=0;i<8;i++){
//        temp = 0ull;
//        BitBoard mask = translate(move, i);
//        while(mask != 0ull && (*opponents & mask) != 0ull){
//            temp |= mask;
//            mask = translate(mask, i);
//        }
//        if((mask & *mine) != 0ull){
//            result |= temp;
//        }
//    }
//
//    int cnt = count(result);
//    if(cnt > 0){
//        //flip
//        *mine ^= (result | move);
//        *opponents ^= result;
//    }
//
//    return cnt;
//}

int Board::move(int pos, int color){
	BitBoard mv = 1ull << pos;
	BitBoard *mine = get_Board(color);
	BitBoard *opponents = get_Board(3 - color);
	if (((*mine | *opponents) & mv) > 0ull) return 0;

	BitBoard result = 0ull, temp, mask, chkPos;
	
	//left
	temp = 0ull;
	chkPos = ((mv & 0xfefefefefefefefe) >> 1);
	mask = chkPos & 0xfefefefefefefefe;

	while (mask != 0ull && (*opponents & mask) != 0ull) {
		temp |= mask;
		chkPos >>= 1;
		mask = chkPos & 0xfefefefefefefefe;
	}
	if ((chkPos & *mine) != 0ull) {
		result |= temp;
	}

	//right
	temp = 0ull;
	chkPos = ((mv & 0x7f7f7f7f7f7f7f7f) << 1);
	mask = chkPos & 0x7f7f7f7f7f7f7f7f;

	while (mask != 0ull && (*opponents & mask) != 0ull) {
		temp |= mask;
		chkPos <<= 1;
		mask = chkPos & 0x7f7f7f7f7f7f7f7f;
	}
	if ((chkPos & *mine) != 0ull) {
		result |= temp;
	}

	//upper  
	temp = 0ull;
	chkPos = ((mv & 0xffffffffffffff00) >> 8);
	mask = chkPos & 0xffffffffffffff00;

	while (mask != 0ull && (*opponents & mask) != 0ull) {
		temp |= mask;
		chkPos >>= 8;
		mask = chkPos & 0xffffffffffffff00;
	}
	if ((chkPos & *mine) != 0ull) {
		result |= temp;
	}

	//lower
	temp = 0ull;
	chkPos = ((mv & 0x00ffffffffffffff) << 8);
	mask = chkPos & 0x00ffffffffffffff;

	while (mask != 0ull && (*opponents & mask) != 0ull) {
		temp |= mask;
		chkPos <<= 8;
		mask = chkPos & 0x00ffffffffffffff;
	}
	if ((chkPos & *mine) != 0ull) {
		result |= temp;
	}

	//upper left
	temp = 0ull;
	chkPos = ((mv & 0xfefefefefefefe00) >> 9);
	mask = chkPos & 0xfefefefefefefe00;

	while (mask != 0ull && (*opponents & mask) != 0ull) {
		temp |= mask;
		chkPos >>= 9;
		mask = chkPos & 0xfefefefefefefe00;
	}
	if ((chkPos & *mine) != 0ull) {
		result |= temp;
	}

	//upper right
	temp = 0ull;
	chkPos = ((mv & 0x7f7f7f7f7f7f7f00) >> 7);
	mask = chkPos & 0x7f7f7f7f7f7f7f00;

	while (mask != 0ull && (*opponents & mask) != 0ull) {
		temp |= mask;
		chkPos >>= 7;
		mask = chkPos & 0x7f7f7f7f7f7f7f00;
	}
	if ((chkPos & *mine) != 0ull) {
		result |= temp;
	}

	//lower left
	temp = 0ull;
	chkPos = ((mv & 0x00fefefefefefefe) << 7);
	mask = chkPos & 0x00fefefefefefefe;

	while (mask != 0ull && (*opponents & mask) != 0ull) {
		temp |= mask;
		chkPos <<= 7;
		mask = chkPos & 0x00fefefefefefefe;
	}
	if ((chkPos & *mine) != 0ull) {
		result |= temp;
	}

	//lower right
	temp = 0ull;
	chkPos = ((mv & 0x007f7f7f7f7f7f7f) << 9);
	mask = chkPos & 0x007f7f7f7f7f7f7f;

	while (mask != 0ull && (*opponents & mask) != 0ull) {
		temp |= mask;
		chkPos <<= 9;
		mask = chkPos & 0x007f7f7f7f7f7f7f;
	}
	if ((chkPos & *mine) != 0ull) {
		result |= temp;
	}

	int cnt = count(result);
	if(cnt > 0){
	    //flip
	    *mine ^= (result | mv);
	    *opponents ^= result;
	}
	
	return cnt;

}

int Board::getScore(int color) {
	int wCount = count(wBoard);
	int bCount = count(bBoard);

	switch (color)
	{
	case WHITE:
		if (wCount > bCount) {
			return 1;
		} else {
			return -1;
		}
		break;
	case BLACK:
		if (wCount < bCount) {
			return 1;
		} else {
			return -1;
		}
		break;
	default:
		return 0;
		break;
	}
}

void Board::printb(BitBoard anothoer){
    int i, j = 0;
    wchar_t num[8][5] = { L" 1 ", L" 2 ", L" 3 ", L" 4 ", L" 5 ", L" 6 ", L" 7 ", L" 8 "};
    wprintf(L"    A  B  C  D  E  F  G  H \n");
    for(i=0;i<BOARD_SIZE;i++){
        if( (i+1) % 8 == 1 ) wprintf(num[j++]);

        if(getBit(anothoer,i)==1ull){
            wprintf(L"[+]");
        }else if(getBit(wBoard,i)==1ull){
            wprintf(L"[@]");
        }else if(getBit(bBoard,i)==1ull){
            wprintf(L"[O]");
        }else{
            wprintf(L"[ ]");
        }

        if( (i+1) % 8 == 0 ) wprintf(L"\n");
    }
}

void Board::printb(){
    int i, j = 0;
    wchar_t num[8][5] = { L" 1 ", L" 2 ", L" 3 ", L" 4 ", L" 5 ", L" 6 ", L" 7 ", L" 8 "};
    wprintf(L"    A  B  C  D  E  F  G  H \n");
    for(i=0;i<BOARD_SIZE;i++){
        if( (i+1) % 8 == 1 ) wprintf(num[j++]);

        if(getBit(wBoard,i)==1ull){
            wprintf(L"[@]");
        }
		else if(getBit(bBoard,i)==1ull){
            wprintf(L"[O]");
		}
		else if (getBit(holes, i) == 1ull) {
			wprintf(L"[X]");
		}
		else{
            wprintf(L"[ ]");
        }
        if( (i+1) % 8 == 0 ) wprintf(L"\n");
    }
}

int count(BitBoard b){
    b = (b & 0x5555555555555555) + ((b >> 1) & 0x5555555555555555); //every 2bits
    b = (b & 0x3333333333333333) + ((b >> 2) & 0x3333333333333333); //every 4bits
    b = (b & 0x0f0f0f0f0f0f0f0f) + ((b >> 4) & 0x0f0f0f0f0f0f0f0f); //every 8bits
    b = (b & 0x00ff00ff00ff00ff) + ((b >> 8) & 0x00ff00ff00ff00ff); //every 16bits
    b = (b & 0x0000ffff0000ffff) + ((b >> 16) & 0x0000ffff0000ffff); //every 32bis
    b = (b & 0x00000000ffffffff) + ((b >> 32) & 0x00000000ffffffff); //every 64bits
    return (int)b;
}

void Board::set_Board(BitBoard wb, BitBoard bb){
    wBoard = wb;
    bBoard = bb;
}

void Board::set_Board(BitBoard wb, BitBoard bb, BitBoard h) {
	wBoard = wb;
	bBoard = bb;
	holes = h;
}

BitBoard *Board::get_Board(int color){
    switch (color)
    {
        case BLACK:
            return &bBoard;
            break;
        case WHITE:
            return &wBoard;
            break;
        default:
            return 0;
            break;
    }
}

void Board::get_Board(BitBoard *wb, BitBoard *bb){
    *wb = wBoard;
    *bb = bBoard;
}

void Board::save(){
    wSavePoint = wBoard;
    bSavePoint = bBoard;
}

void Board::rollback(){
    wBoard = wSavePoint;
    bBoard = bSavePoint;
}

//------------------------------------------

//int getPos(const wchar_t *pos){
//    int x,y;
//    if(wcscmp(&pos[1],L"A")) x = 0;
//    else if (wcscmp(&pos[1],L"A")) x = 1;
//    else if (wcscmp(&pos[1],L"A")) x = 2;
//    else if (wcscmp(&pos[1],L"A")) x = 3;
//    else if (wcscmp(&pos[1],L"A")) x = 4;
//    else if (wcscmp(&pos[1],L"A")) x = 5;
//    else if (wcscmp(&pos[1],L"A")) x = 6;
//    else if (wcscmp(&pos[1],L"A")) x = 7;
//    else return 99;
//
//    if(wcscmp(&pos[0],L"0")) y = 0;
//    else if (wcscmp(&pos[0],L"1")) y = 1;
//    else if (wcscmp(&pos[0],L"2")) y = 2;
//    else if (wcscmp(&pos[0],L"3")) y = 3;
//    else if (wcscmp(&pos[0],L"4")) y = 4;
//    else if (wcscmp(&pos[0],L"5")) y = 5;
//    else if (wcscmp(&pos[0],L"6")) y = 6;
//    else if (wcscmp(&pos[0],L"7")) y = 7;
//    else return 99;
//
//    return getPos(x, y);
//
//}

int getPos(wchar_t* move) {
	if (move[3] == L'0' || move[4] == L'0') return INVALID_MOVE;

	int x = (int)move[3] - 64;
	int y = (int)move[4] - 48;

	return getPos(x, y);
}

inline int getPos(int x, int y){
    return (x - 1) + (y - 1) * 8;
}

wchar_t charPos[2];
wchar_t * getCharPos(int pos) {
	int x, y;
	if (pos > 63 || pos < 0) {
		charPos[0] = (wchar_t)48;
		charPos[1] = (wchar_t)48;
	}
	else {
		y = (int)((pos + 8) / 8);
		x = ((pos + 8) % 8) + 1;
		charPos[0] = (wchar_t)(x + 64);
		charPos[1] = (wchar_t)(y + 48);
	}
	return charPos;
}

BitBoard getBits(BitBoard b, int pos, int n){
    return ( b >> pos ) & ~( ~0ull << n );
}

BitBoard getBit(BitBoard b, int pos){
    return (b >> pos) & 1ull;
}

void printBitBoard(BitBoard b){
    int i, j = 0;
    wchar_t num[8][5] = { L" 1 ", L" 2 ", L" 3 ", L" 4 ", L" 5 ", L" 6 ", L" 7 ", L" 8 "};
    wprintf(L"    1  2  3  4  5  6  7  8 \n");
    for(i=0;i<BOARD_SIZE;i++){
        if( (i+1) % 8 == 1 ) wprintf(num[j++]);
        if(getBit(b,i)==1ull){
            wprintf(L"[+]");
        }else{
            wprintf(L"[ ]");
        }
        if( (i+1) % 8 == 0 ) wprintf(L"\n");
    }
}