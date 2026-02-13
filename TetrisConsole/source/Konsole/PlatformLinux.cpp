#include "Platform.h"

#include <termios.h>
#include <unistd.h>
#include <cstdio>

#include "rlutil.h"

static struct termios s_originalTermios;
static bool s_termiosRestored = false;

void Platform::initConsole()
{
	// Save original terminal settings
	tcgetattr(STDIN_FILENO, &s_originalTermios);
	s_termiosRestored = false;

	// Set raw mode for non-blocking input
	struct termios raw = s_originalTermios;
	raw.c_lflag &= ~(ICANON | ECHO);
	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 0;
	tcsetattr(STDIN_FILENO, TCSANOW, &raw);

	rlutil::hidecursor();
	rlutil::setConsoleTitle("Tetris Console");
	rlutil::cls();
}

void Platform::cleanupConsole()
{
	if (!s_termiosRestored)
	{
		tcsetattr(STDIN_FILENO, TCSANOW, &s_originalTermios);
		s_termiosRestored = true;
	}
	rlutil::showcursor();
	rlutil::resetColor();
}

void Platform::flushInput()
{
	tcflush(STDIN_FILENO, TCIFLUSH);
}
