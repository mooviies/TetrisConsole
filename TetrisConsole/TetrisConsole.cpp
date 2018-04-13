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
#include <mutex>

#include "Tetris.h"
#include "Random.h"
#include "Input.h"

using namespace std;

mutex readInputMutex;

void initConsole(int width, int height);
void afficherTitre(string sousTitre);

int main()
{
	initConsole(660, 510);
	
	Random::init();

	Tetris tetris = Tetris();
	afficherTitre("Un classic en console!");
	tetris.display();

	
	bool quit = false;
	while (!tetris.doExit())
	{
		tetris.step();
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