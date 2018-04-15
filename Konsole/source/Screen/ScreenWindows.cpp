#include "ScreenWindows.h"

#include <iostream>
#include <Windows.h>

using namespace konsole;
using namespace std;

ScreenWindows::ScreenWindows(uint width, uint height)
	: Screen(width, height), FONT_WIDTH(8), FONT_HEIGHT(16)
{
	
}

ScreenWindows::~ScreenWindows()
{
	
}

void ScreenWindows::onInitialize()
{
	_console = GetConsoleWindow();
	_desktop = GetDesktopWindow();
	
	_output = GetStdHandle(STD_OUTPUT_HANDLE);
	_input = GetStdHandle(STD_INPUT_HANDLE);

	CONSOLE_CURSOR_INFO     cursorInfo;
	GetConsoleCursorInfo(_output, &cursorInfo);
	cursorInfo.bVisible = false;
	SetConsoleCursorInfo(_output, &cursorInfo);

	DWORD prev_mode;
	GetConsoleMode(_input, &prev_mode);
	SetConsoleMode(_input, prev_mode & ~ENABLE_QUICK_EDIT_MODE);

	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(cfi);
	cfi.nFont = 0;
	cfi.dwFontSize.X = FONT_WIDTH;
	cfi.dwFontSize.Y = FONT_HEIGHT;
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = FW_NORMAL;
	std::wcscpy(cfi.FaceName, L"Consolas");
	SetCurrentConsoleFontEx(_output, FALSE, &cfi);

	setTitle(TEXT("My Console"));
}

void ScreenWindows::onSetTitle(const std::wstring& title)
{
	SetConsoleTitle(title.c_str());
}

void ScreenWindows::onSetSize(uint width, uint height)
{
	int screenWidth = width * FONT_WIDTH;
	int screenHeight = height * FONT_HEIGHT;

	RECT rectDesktop, rectConsole;
	GetWindowRect(_desktop, &rectDesktop);
	MoveWindow(_console, (rectDesktop.right - screenWidth) / 2, (rectDesktop.bottom - screenHeight) / 2, screenWidth, screenHeight, TRUE);
	SetWindowLong(_console, GWL_STYLE, GetWindowLong(_console, GWL_STYLE) & ~(WS_MAXIMIZEBOX | WS_SIZEBOX));

	CONSOLE_SCREEN_BUFFER_INFO sbInfo;
	COORD newSBSize;
	GetConsoleScreenBufferInfo(_output, &sbInfo);
	newSBSize.X = width;
	newSBSize.Y = height;

	_SMALL_RECT Rect;
	Rect.Top = 0;
	Rect.Left = 0;
	Rect.Bottom = height - 1;
	Rect.Right = width - 1;

	if (!SetConsoleScreenBufferSize(_output, newSBSize))
	{
		cout << GetLastError() << endl;
	}
	SetConsoleWindowInfo(_output, TRUE, &Rect);
}

void ScreenWindows::onSetColor(Color color)
{

}

void ScreenWindows::onSetBackgroundColor(Color color)
{

}

void ScreenWindows::onSetCursorPosition(uint x, uint y)
{

}
