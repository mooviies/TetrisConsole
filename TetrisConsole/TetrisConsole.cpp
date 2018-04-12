﻿#include "stdafx.h"

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <windows.h>
#include <strsafe.h>
#include <tchar.h>
#include <conio.h>
#include <queue>
#include <mutex>

#include "Tetris.h"

using namespace std;

mutex readInputMutex;

void initConsole(int width, int height);
void afficherTitre(string sousTitre);
DWORD WINAPI readInput(LPVOID lpParam);

int main()
{
	initConsole(660, 510);
	
	queue<int> inputBuffer;
	// Initialisation du générateur de hasard
	srand(time(NULL));

	Tetris tetris = Tetris();
	afficherTitre("Un classic en console!");
	tetris.display();

	DWORD inputThreadId;
	HANDLE inputThread = CreateThread(
		NULL,                   // default security attributes
		0,                      // use default stack size  
		readInput,       // thread function name
		&inputBuffer,          // argument to thread function 
		0,                      // use default creation flags 
		&inputThreadId);   // returns the thread identifier 

	
	bool quit = false;
	while (!quit)
	{
		int k = 0;
		if (readInputMutex.try_lock())
		{
			if (!inputBuffer.empty())
			{
				k = inputBuffer.front();
				inputBuffer.pop();
			}
			readInputMutex.unlock();
		}
		switch (k)
		{
		case 72:
		case 'X':
		case 'x':
		case '1':
		case '5':
		case '9':
			//cout << "ROTATE CLOCKWISE" << endl;
			tetris.rotateClockwise();
			tetris.refresh();
			break;
		case 'Z':
		case 'z':
		case '3':
		case '7':
			//cout << "ROTATE COUNTERCLOCKWISE" << endl;
			tetris.rotateCounterClockwise();
			tetris.refresh();
			break;
		case 75:
		case '4':
			//cout << "MOVE LEFT" << endl;
			tetris.moveLeft();
			tetris.refresh();
			break;
		case 77:
		case '6':
			//cout << "MOVE RIGHT" << endl;
			tetris.moveRight();
			tetris.refresh();
			break;
		case 80:
		case '2':
			//cout << "SOFT DROP" << endl;
			tetris.moveDown();
			tetris.refresh();
			break;
		case ' ':
		case '8':
			//cout << "HARD DROP" << endl;
			tetris.moveUp();
			tetris.refresh();
			break;
		case 'C':
		case 'c':
		case '0':
			//cout << "HOLD" << endl;
			break;
		case 27:
		case 59:
			//cout << "PAUSE" << endl;
			break;
		case 'Q':
		case 'q':
			quit = true;
			break;
		}
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

DWORD WINAPI readInput(LPVOID lpParam)
{
	queue<int>* inputBuffer = (queue<int>*)lpParam;
	
	TCHAR k;
	while (true)
	{
		k = _getch();
		readInputMutex.lock();
		inputBuffer->push(k);
		readInputMutex.unlock();
	}

	return 0;
}

void afficherTitre(string sousTitre)
{
	// Initialisation d'une variable qui vaut la longueur du sous-titre
	// Cette valeur est réutilisé plusieurs fois on évite donc de toujours
	// repassé par la fonction length() qui est plus lente que la simple
	// lecture d'un int
	int longueur = sousTitre.length();

	// Affichage du titre principale
	cout << "ÉÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ»º"
		 << "                                Tetris Console"
		 << "                                ºÌÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¹"
		 << "º";

	// On affiche des espace avant le sous-titre et de façon à ce qu'il soit centré
	for (int i = 0; i < 39 - (longueur / 2); i++)
	{
		cout << ' ';
	}

	// On affiche le sous-titre
	cout << sousTitre;

	// On vérifie si le nombre de lettre du sous-titre est pair ou impaire pour évité un bug d'affichage
	// Car s'il est impair alors le nombre d'espace avant et après le sous-titre sera différent
	if (longueur % 2 != 0)
		longueur++;

	// On affiche des espaces après le sous-titre
	for (int i = 0; i < 39 - (longueur / 2); i++)
	{
		cout << ' ';
	}
	// Puis on affiche la fermeture du cadre
	cout << "ºÈÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¼" << endl;
}