// ReversiHost.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include "pch.h"
#include "Reversi.h"
#include <iostream>
#include <fstream>
#include <windows.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <queue>
#include <time.h>
#include <locale.h>
#define BUF_SIZE 256
#define MAX_LEN 25
#define MAX_PATH2 1000

Reversi reversi;

//Player 
wchar_t inputDir[MAX_PATH2];

wchar_t outputDir[MAX_PATH2];

wchar_t inputDatPath[MAX_PATH2];

//module file name
wchar_t moduleFileName[_MAX_PATH];

wchar_t mDrive[_MAX_DRIVE], mDir[_MAX_DIR], mFName[_MAX_FNAME], mExt[_MAX_EXT];

std::queue<wchar_t*> fileQueue;

std::queue<wchar_t*> messageQueue;

void startGame();

void waitNextMessage();

void sendFile(wchar_t* move, int len);

void removeFiles();

void processMessageQueue();

bool makePaths();

void getOutputFName(wchar_t * outputFName);

void printStartMessage();

void printModuleName(wchar_t * moduleName);

void printWin();

void printLose();

void printDraw();

int main()
{
	setlocale(LC_ALL, "japanese");

	makePaths();

	startGame();

	int n;

	std::cin >> n;

	return 0;

}

bool makePaths() {

	//Get Current Directory
	GetModuleFileNameW(NULL, moduleFileName, _MAX_PATH);
	_wsplitpath_s(moduleFileName, mDrive, mDir, mFName, mExt);

	printModuleName(mFName);

	//Make Paths
	wcscpy_s(outputDir, sizeof(mDir), mDir);
	wcscat_s(outputDir, L"\output");

	wcscpy_s(inputDir, sizeof(mDir), mDir);
	wcscat_s(inputDir, L"\input");

	_wmakepath_s(inputDatPath, mDrive, inputDir, L"*", L"dat");

	return true;

}

void startGame() {

	wprintf_s(L"Watching : %s\n", inputDatPath);
	wprintf_s(L"...\n");

	while (true) {
		waitNextMessage();
	}

}

void waitNextMessage() {

	HANDLE hFind;

	WIN32_FIND_DATA win32fd;

	hFind = FindFirstFileW(inputDatPath, &win32fd);

	bool got_A_Message = false;

	if (hFind == INVALID_HANDLE_VALUE) {
	}
	else {
		do {
			if (win32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			}
			else {

				wchar_t txtpath[MAX_PATH];
				wchar_t datPath[MAX_PATH];
				wchar_t buffer[BUF_SIZE];

				//split datfile
				wchar_t drive[_MAX_DRIVE], dir[_MAX_DIR], fName[_MAX_FNAME], ext[_MAX_EXT];
				_wsplitpath_s(win32fd.cFileName, drive, dir, fName, ext);

				_wmakepath_s(txtpath, mDrive, inputDir, fName, L".txt");
				_wmakepath_s(datPath, mDrive, inputDir, fName, L".dat");

				//read message from txt file
				std::wifstream ifs(txtpath);
				if (!ifs.fail()) {

					while (ifs.getline(buffer, BUF_SIZE)) {
						wchar_t buffer2[BUF_SIZE];
						wcscpy_s(buffer2, buffer);
						messageQueue.push(buffer2);
					}

					fileQueue.push(txtpath);
					got_A_Message = true;
				}

				//push dat-file into delete-queue
				if (got_A_Message) {
					fileQueue.push(datPath);
				}

			}
			//if there is no file or a new message is got, 
		} while (FindNextFile(hFind, &win32fd) || !got_A_Message);
	}

	FindClose(hFind);

	//process recieved messages
	processMessageQueue();

	//waiting for host releasing files
	Sleep(20);
	removeFiles();

}

void removeFiles() {

	wchar_t * path;
	while (!fileQueue.empty()) {

		path = fileQueue.front();
		fileQueue.pop();

		if (_wremove(path) == 0) {
			//wprintf(L"Successfully Removed : %s\n", path);
		}
		else {
			wprintf(L"CANNOT_REMOVE_FILE_EXCEPTION : %s\n", path);
		}

	}
}

void processMessageQueue() {
	wchar_t * tmp;

	while (!messageQueue.empty()) {
		tmp = messageQueue.front();
		messageQueue.pop();

		wprintf_s(L"\n");
		wprintf_s(L"Message Recieved : %s\n", tmp);

		switch (reversi.processMessage(tmp)) {
		case MessageType::GameStarts_OpponentsTurn:
			printStartMessage();
			break;
		case MessageType::GameStarts_MyTurn:
			printStartMessage();
			sendFile(reversi.getNextMove(), MYTURN_LEN);
			break;
		case MessageType::OpponentsMove:
			sendFile(reversi.getNextMove(), MYTURN_LEN);
			break;
		case MessageType::MyMove:
			break;
		case MessageType::Win:
			printWin();
			break;
		case MessageType::Lose:
			printLose();
			break;
		case MessageType::Draw:
			printDraw();
			break;
		default:
			break;
		}

	}
}

void sendFile(wchar_t* move, int len) {

	wchar_t sendFName[_MAX_FNAME];
	getOutputFName(sendFName);
	wchar_t sendTxtPath[_MAX_PATH], sendDatPath[_MAX_PATH];
	_wmakepath_s(sendTxtPath, mDrive, outputDir, sendFName, L"txt");
	_wmakepath_s(sendDatPath, mDrive, outputDir, sendFName, L"dat");

	std::ofstream outputfile(sendTxtPath);
	for (int i = 0; i < len; i++) outputfile << (char)*move++;
	outputfile.close();

	std::ofstream outputfile2(sendDatPath);
	outputfile2.close();

}

void getOutputFName(wchar_t * outputFName) {
	wchar_t s_date[MAX_LEN];
	time_t nowDateTime;
	struct tm localTime;

	nowDateTime = time(NULL);

	localtime_s(&localTime, &nowDateTime);
	wcsftime(s_date, MAX_LEN, L"%Y%m%d%H%M%S", &localTime);
	wchar_t fileName[_MAX_FNAME] = L"reversi_";
	wcscat_s(fileName, s_date);
	wcscpy_s(outputFName, sizeof(fileName), fileName);
}

void printStartMessage() {
	wprintf_s(L"\n");
	wprintf_s(L"**************************************\n");
	wprintf_s(L"*               Start!               *\n");
	wprintf_s(L"**************************************\n");
}

void printModuleName(wchar_t * moduleName) {
	wprintf_s(L"=========================================================================================\n");
	wprintf_s(L"                               %s             \n", moduleName);
	wprintf_s(L"=========================================================================================\n");
	wprintf_s(L"\n");
}

void printWin() {
	wprintf_s(L"\n");
	wprintf_s(L"**************************************\n");
	wprintf_s(L"*               WIN!!                *\n");
	wprintf_s(L"**************************************\n");
	wprintf_s(L"\n");
}

void printLose() {
	wprintf_s(L"\n");
	wprintf_s(L"**************************************\n");
	wprintf_s(L"*               LOSE!!               *\n");
	wprintf_s(L"**************************************\n");
	wprintf_s(L"\n");
}

void printDraw() {
	wprintf_s(L"\n");
	wprintf_s(L"**************************************\n");
	wprintf_s(L"*                 DRAW!!             *\n");
	wprintf_s(L"**************************************\n");
	wprintf_s(L"\n");
}

// プログラムの実行: Ctrl + F5 または [デバッグ] > [デバッグなしで開始] メニュー
// プログラムのデバッグ: F5 または [デバッグ] > [デバッグの開始] メニュー

// 作業を開始するためのヒント: 
//    1. ソリューション エクスプローラー ウィンドウを使用してファイルを追加/管理します 
//   2. チーム エクスプローラー ウィンドウを使用してソース管理に接続します
//   3. 出力ウィンドウを使用して、ビルド出力とその他のメッセージを表示します
//   4. エラー一覧ウィンドウを使用してエラーを表示します
//   5. [プロジェクト] > [新しい項目の追加] と移動して新しいコード ファイルを作成するか、[プロジェクト] > [既存の項目の追加] と移動して既存のコード ファイルをプロジェクトに追加します
//   6. 後ほどこのプロジェクトを再び開く場合、[ファイル] > [開く] > [プロジェクト] と移動して .sln ファイルを選択します
