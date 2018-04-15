#include "Graphics.h"

using namespace konsole;
using namespace std;

Graphics::Graphics(Screen& screen)
	: _screen(screen)
{
}

Graphics::~Graphics()
{
}

char konsole::Graphics::asciiToAnsi(char ascii)
{
	char t = ascii - FACTOR_ASCII_TO_ANSI;
	return t;
}

std::string konsole::Graphics::asciiToAnsi(const std::string & ascii)
{
	string ansi;
	for (int i = 0; i < ascii.length(); i++)
	{
		ansi += asciiToAnsi(ascii[i]);
	}
	return ansi;
}