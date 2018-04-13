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
#include "fmod\fmod.hpp"
#include "fmod\fmod_errors.h"

using namespace std;

void initConsole(int width, int height);
void checkFMODError(FMOD_RESULT result);

int main()
{
	initConsole(660, 510);
	
	Random::init();

	Tetris tetris;

	//// FMOD
	FMOD_RESULT result;
	FMOD::System *system = NULL;

	result = FMOD::System_Create(&system);      // Create the main system object.
	checkFMODError(result);

	result = system->init(512, FMOD_INIT_NORMAL, 0);    // Initialize FMOD.
	checkFMODError(result);

	FMOD::Sound *sound, *sound_to_play;
	result = system->createStream("media/tetris.mp3", FMOD_LOOP_NORMAL | FMOD_2D, 0, &sound);
	checkFMODError(result);

	int numsubsounds;
	result = sound->getNumSubSounds(&numsubsounds);
	checkFMODError(result);

	if (numsubsounds)
	{
		sound->getSubSound(0, &sound_to_play);
		checkFMODError(result);
	}
	else
	{
		sound_to_play = sound;
	}

	FMOD::Channel *channel = 0;
	result = system->playSound(sound_to_play, 0, false, &channel);
	channel->setVolume(0.1);
	checkFMODError(result);

	////
	
	bool quit = false;
	while (!tetris.doExit())
	{
		tetris.step();
		system->update();
	}

	system->release();

    return 0;
}

void initConsole(int width, int height)
{
	HWND console = GetConsoleWindow();
	HWND desktop = GetDesktopWindow();
	RECT rectDesktop, rectConsole;
	GetWindowRect(desktop, &rectDesktop);
	MoveWindow(console, (rectDesktop.right - width) / 2, (rectDesktop.bottom - height) / 2, width, height, TRUE);
	SetWindowLong(console, GWL_STYLE, GetWindowLong(console, GWL_STYLE) & ~(WS_MAXIMIZEBOX | WS_SIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU));
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

void checkFMODError(FMOD_RESULT result)
{
	if (result != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		system("pause");
		exit(-1);
	}
}