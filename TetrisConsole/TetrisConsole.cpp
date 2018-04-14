﻿#include "stdafx.h"

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <windows.h>
#include <conio.h>
#include <queue>

#include "Tetris.h"
#include "Random.h"
#include "Input.h"
#include "SoundEngine.h"
#include "Utility.h"
#include "Menu.h"
#include "Overseer.h"

using namespace std;

void initConsole(int width, int height);
void start();
void quitGame();
void incrementLevel();
void decrementLevel();

int main()
{
	initConsole(660, 510);
	
	Random::init();
	SoundEngine::init();

	Utility::afficherTitre("A classic in console!");

	vector<string> levels;
	for (int i = 1; i <= 15; i++)
		levels.push_back(Utility::valueToString(i, 2));

	vector<string> modes;
	modes.push_back("Extended");
	modes.push_back("Infinite");
	modes.push_back("Classic");

	Menu main("Main Menu");
	Menu options("Options");
	Menu newGame("New Game");
	Menu pause("Pause");
	Menu help("Help");
	Menu quit("Are you sure?");
	Menu gameover("Game Over");
	
	main.addOption("New Game", &newGame);
	main.addOption("Options", &options);
	main.addOption("Help", &help);
	main.addOption("Exit", &quit);

	newGame.addOption("Start", &start);
	newGame.addOptionWithValues("Level", levels);
	newGame.addOptionWithValues("Mode", modes);

	help.addOptionClose("Back");

	pause.addOptionClose("Resume");
	pause.addOption("Options", &options);
	pause.addOption("Exit", &quit);

	options.addOptionClose("Back");

	quit.addOption("Yes", &quitGame);
	quit.addOptionClose("No");

	main.open();

	Tetris tetris(pause);
	Overseer::setTetris(&tetris);
	
	while (!tetris.doExit())
	{
		tetris.step();
		SoundEngine::update();
	}

    return 0;
}

void initConsole(int width, int height)
{
	HWND console = GetConsoleWindow();
	HWND desktop = GetDesktopWindow();
	RECT rectDesktop, rectConsole;
	GetWindowRect(desktop, &rectDesktop);
	MoveWindow(console, (rectDesktop.right - width) / 2, (rectDesktop.bottom - height) / 2, width, height, TRUE);
	SetWindowLong(console, GWL_STYLE, GetWindowLong(console, GWL_STYLE) & ~(WS_MAXIMIZEBOX | WS_SIZEBOX));
	SetConsoleTitle(TEXT("Tetris Console"));

	system("color 0F");

	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_CURSOR_INFO     cursorInfo;
	GetConsoleCursorInfo(out, &cursorInfo);
	cursorInfo.bVisible = false;
	SetConsoleCursorInfo(out, &cursorInfo);
	
	CONSOLE_SCREEN_BUFFER_INFO sbInfo;
	COORD newSBSize;
	GetConsoleScreenBufferInfo(out, &sbInfo);
	newSBSize.X = 80;
	newSBSize.Y = 28;

	int status = SetConsoleScreenBufferSize(out, newSBSize);

	HANDLE input = GetStdHandle(STD_INPUT_HANDLE);

	DWORD prev_mode;
	GetConsoleMode(input, &prev_mode);
	SetConsoleMode(input, prev_mode & ~ENABLE_QUICK_EDIT_MODE);
}

void start()
{

}

void quitGame()
{
	exit(0);
}

void incrementLevel()
{

}

void decrementLevel()
{

}