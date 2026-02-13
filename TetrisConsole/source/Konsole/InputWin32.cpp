#include "Input.h"

#include <Windows.h>

void Input::init()
{
}

void Input::cleanup()
{
}

void Input::pollKeys()
{
}

bool Input::left()
{
	return (GetKeyState(VK_LEFT) & 0x8000) ||
		(GetKeyState(VK_NUMPAD4) & 0x8000);
}

bool Input::right()
{
	return (GetKeyState(VK_RIGHT) & 0x8000) ||
		(GetKeyState(VK_NUMPAD6) & 0x8000);
}

bool Input::hardDrop()
{
	return (GetKeyState(VK_SPACE) & 0x8000) ||
		(GetKeyState(VK_NUMPAD8) & 0x8000);
}

bool Input::softDrop()
{
	return (GetKeyState(VK_DOWN) & 0x8000) ||
		(GetKeyState(VK_NUMPAD2) & 0x8000);
}

bool Input::rotateClockwise()
{
	return (GetKeyState(VK_UP) & 0x8000) ||
		(GetKeyState('X') & 0x8000) ||
		(GetKeyState(VK_NUMPAD1) & 0x8000) ||
		(GetKeyState(VK_NUMPAD5) & 0x8000) ||
		(GetKeyState(VK_NUMPAD9) & 0x8000);
}

bool Input::rotateCounterClockwise()
{
	return (GetKeyState(VK_CONTROL) & 0x8000) ||
		(GetKeyState('Z') & 0x8000) ||
		(GetKeyState(VK_NUMPAD3) & 0x8000) ||
		(GetKeyState(VK_NUMPAD7) & 0x8000);
}

bool Input::hold()
{
	return (GetKeyState(VK_SHIFT) & 0x8000) ||
		(GetKeyState('C') & 0x8000) ||
		(GetKeyState(VK_NUMPAD0) & 0x8000);
}

bool Input::pause()
{
	return (GetKeyState(VK_ESCAPE) & 0x8000) ||
		(GetKeyState(VK_F1) & 0x8000);
}

bool Input::mute()
{
	return GetKeyState('M') & 0x8000;
}

bool Input::select()
{
	return GetKeyState(VK_RETURN) & 0x8000;
}

Input::Input()
{
}


Input::~Input()
{
}
