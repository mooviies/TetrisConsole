#include "Platform.h"

#include <windows.h>
#include <shlobj.h>
#include <conio.h>
#include <iostream>
#include <string>

#include "rlutil.h"

void Platform::initConsole()
{
	// Ignore Ctrl+C — quit through the in-app menu
	SetConsoleCtrlHandler(NULL, TRUE);

	HWND console = GetConsoleWindow();
	HWND desktop = GetDesktopWindow();
	RECT rectDesktop;
	GetWindowRect(desktop, &rectDesktop);
	int width = 675, height = 515;
	MoveWindow(console, (rectDesktop.right - width) / 2, (rectDesktop.bottom - height) / 2, width, height, TRUE);
	SetWindowLongPtr(console, GWL_STYLE, GetWindowLongPtr(console, GWL_STYLE) & ~(WS_MAXIMIZEBOX | WS_SIZEBOX));
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

void Platform::flushOutput()
{
	std::cout << std::flush;
}

int Platform::getKey()
{
	return rlutil::getkey();
}

bool Platform::wasResized()
{
	return false;
}

bool Platform::isTerminalTooSmall()
{
	return false;
}

void Platform::showResizePrompt()
{
}

int Platform::offsetX()
{
	return 0;
}

int Platform::offsetY()
{
	return 0;
}

void Platform::updateOffsets()
{
}

std::string Platform::getDataDir()
{
	char path[MAX_PATH];
	std::string dir;
	if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, path)))
		dir = std::string(path) + "\\TetrisConsole";
	else
		dir = ".\\TetrisConsole";
	CreateDirectoryA(dir.c_str(), NULL);
	return dir;
}
