#include "Platform.h"

#include <termios.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <sys/select.h>

#include "rlutil.h"

#define CONSOLE_WIDTH 80
#define CONSOLE_HEIGHT 29

static struct termios s_originalTermios;
static bool s_termiosRestored = false;
static int s_savedRows = 0;
static int s_savedCols = 0;

void Platform::initConsole()
{
	// Save original terminal settings
	tcgetattr(STDIN_FILENO, &s_originalTermios);
	s_termiosRestored = false;

	// Save original terminal size
	s_savedRows = rlutil::trows();
	s_savedCols = rlutil::tcols();

	// Resize terminal to fixed game dimensions
	std::cout << "\033[8;" << CONSOLE_HEIGHT << ";" << CONSOLE_WIDTH << "t" << std::flush;

	// Set raw mode: char-at-a-time, no echo, blocking reads (VMIN=1)
	// VMIN=1 is needed so blocking reads work properly in menus.
	// The game loop uses select() in Input::pollKeys() to avoid blocking.
	struct termios raw = s_originalTermios;
	raw.c_lflag &= ~(ICANON | ECHO);
	raw.c_oflag |= (OPOST | ONLCR);
	raw.c_cc[VMIN] = 1;
	raw.c_cc[VTIME] = 0;
	tcsetattr(STDIN_FILENO, TCSANOW, &raw);

	rlutil::hidecursor();
	rlutil::setConsoleTitle("Tetris Console");
	rlutil::cls();
	rlutil::setColor(rlutil::WHITE);
	rlutil::setBackgroundColor(rlutil::BLACK);
	std::cout << std::flush;
}

void Platform::cleanupConsole()
{
	if (!s_termiosRestored)
	{
		tcsetattr(STDIN_FILENO, TCSANOW, &s_originalTermios);
		s_termiosRestored = true;
	}

	// Restore original terminal size
	if (s_savedRows > 0 && s_savedCols > 0)
		std::cout << "\033[8;" << s_savedRows << ";" << s_savedCols << "t" << std::flush;

	rlutil::cls();
	rlutil::showcursor();
	rlutil::resetColor();
	std::cout << std::flush;
}

void Platform::flushInput()
{
	tcflush(STDIN_FILENO, TCIFLUSH);
}

int Platform::getKey()
{
	unsigned char c;
	if (read(STDIN_FILENO, &c, 1) != 1)
		return 0; // KEY_ESCAPE as fallback

	if (c == 0x1B)
	{
		// Check if more bytes follow (escape sequence) with short timeout
		fd_set fds;
		struct timeval tv;
		FD_ZERO(&fds);
		FD_SET(STDIN_FILENO, &fds);
		tv.tv_sec = 0;
		tv.tv_usec = 50000; // 50ms
		if (::select(STDIN_FILENO + 1, &fds, nullptr, nullptr, &tv) > 0)
		{
			unsigned char seq[2];
			if (read(STDIN_FILENO, &seq[0], 1) == 1 && seq[0] == '[')
			{
				if (read(STDIN_FILENO, &seq[1], 1) == 1)
				{
					switch (seq[1])
					{
					case 'A': return 14; // KEY_UP
					case 'B': return 15; // KEY_DOWN
					case 'C': return 17; // KEY_RIGHT
					case 'D': return 16; // KEY_LEFT
					}
				}
			}
		}
		return 0; // KEY_ESCAPE (standalone)
	}

	if (c == '\r' || c == '\n')
		return 1; // KEY_ENTER

	if (c == ' ')
		return 32; // KEY_SPACE

	return c;
}

std::string Platform::getDataDir()
{
	std::string dir;
	const char* xdg = std::getenv("XDG_DATA_HOME");
	if (xdg && xdg[0] != '\0')
		dir = std::string(xdg) + "/TetrisConsole";
	else
		dir = std::string(std::getenv("HOME")) + "/.local/share/TetrisConsole";
	mkdir(dir.c_str(), 0755);
	return dir;
}
