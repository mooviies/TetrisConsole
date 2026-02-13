#include "Platform.h"

#include <windows.h>
#include <conio.h>

#include "rlutil.h"

void Platform::initConsole()
{
	HWND console = GetConsoleWindow();
	HWND desktop = GetDesktopWindow();
	RECT rectDesktop;
	GetWindowRect(desktop, &rectDesktop);
	int width = 675, height = 515;
	MoveWindow(console, (rectDesktop.right - width) / 2, (rectDesktop.bottom - height) / 2, width, height, TRUE);
	SetWindowLong(console, GWL_STYLE, GetWindowLong(console, GWL_STYLE) & ~(WS_MAXIMIZEBOX | WS_SIZEBOX));
	SetConsoleTitle(TEXT("Tetris Console"));

	system("color 0F");

	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo(out, &cursorInfo);
	cursorInfo.bVisible = false;
	SetConsoleCursorInfo(out, &cursorInfo);

	CONSOLE_SCREEN_BUFFER_INFO sbInfo;
	COORD newSBSize;
	GetConsoleScreenBufferInfo(out, &sbInfo);
	newSBSize.X = 80;
	newSBSize.Y = 29;
	SetConsoleScreenBufferSize(out, newSBSize);

	HANDLE input = GetStdHandle(STD_INPUT_HANDLE);
	DWORD prev_mode;
	GetConsoleMode(input, &prev_mode);
	SetConsoleMode(input, prev_mode & ~ENABLE_QUICK_EDIT_MODE);
}

void Platform::cleanupConsole()
{
}

void Platform::flushInput()
{
	FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
}
