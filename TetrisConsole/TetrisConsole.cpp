#include "stdafx.h"

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
void setGameSettings(OptionChoice optionChoice);
void quitGame(OptionChoice optionChoice);
void incrementLevel(OptionChoice optionChoice);
void decrementLevel(OptionChoice optionChoice);

int main()
{
	initConsole(675, 515);
	
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

	Menu main("MAIN MENU");
	Menu options("SETTINGS");
	Menu newGame("NEW GAME");
	Menu pause("PAUSE");
	Menu help("HELP");
	Menu quit("Are you sure?");
	Menu gameover("GAME OVER", "New High Score!");
	
	main.addOption("New Game", &newGame);
	//main.addOption("Settings", &options);
	//main.addOption("Help", &help);
	main.addOption("Exit", &quit);

	newGame.addOptionCloseAllMenu("Start", &setGameSettings);
	newGame.addOptionWithValues("Level", levels);
	newGame.addOptionWithValues("Mode", modes);

	//help.addOptionClose("Back");

	options.addOptionWithValues("Level", levels);
	options.addOptionWithValues("Mode", modes);
	options.addOptionCloseAllMenu("Accept", &setGameSettings);
	options.addOptionClose("Cancel");

	pause.addOptionClose("Resume");
	pause.addOptionClose("Restart");
	pause.addOption("Exit Game", &quit);

	quit.addOption("Yes", &quitGame);
	quit.addOptionClose("No");

	gameover.addOptionClose("Retry");
	gameover.addOption("Settings", &options);
	gameover.addOption("Exit Game", &quit);

	Tetris tetris(pause, gameover);
	Overseer::setTetris(&tetris);

	main.open();
	tetris.start();
	
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
	newSBSize.Y = 29;

	int status = SetConsoleScreenBufferSize(out, newSBSize);

	HANDLE input = GetStdHandle(STD_INPUT_HANDLE);

	DWORD prev_mode;
	GetConsoleMode(input, &prev_mode);
	SetConsoleMode(input, prev_mode & ~ENABLE_QUICK_EDIT_MODE);
}

void setGameSettings(OptionChoice optionChoice)
{
	string levelName = optionChoice.values["Level"];
	string modeName = optionChoice.values["Mode"];

	int level = stoi(levelName);
	MODE mode;
	if (modeName == "Extended")
	{
		mode = EXTENDED;
	}
	else if (modeName == "Infinite")
	{
		mode = EXTENDED_INFINITY;
	}
	else if (modeName == "Classic")
	{
		mode = CLASSIC;
	}

	Overseer::getTetris().setStartingLevel(level);
	Overseer::getTetris().setMode(mode);
}

void quitGame(OptionChoice optionChoice)
{
	exit(0);
}

void incrementLevel(OptionChoice optionChoice)
{

}

void decrementLevel(OptionChoice optionChoice)
{

}