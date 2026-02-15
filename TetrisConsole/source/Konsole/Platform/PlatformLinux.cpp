#include "Platform.h"

#include <termios.h>
#include <unistd.h>
#include <algorithm>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <sys/select.h>

#include "rlutil.h"

#define CONSOLE_WIDTH 80
#define CONSOLE_HEIGHT 29

static termios s_originalTermios;
static bool s_termiosRestored = false;
static volatile sig_atomic_t s_resized = 0;
static int s_offsetX = 0;
static int s_offsetY = 0;

static void handleSigwinch(int) {
	s_resized = 1;
}

void Platform::initConsole()
{
	// Save original terminal settings
	tcgetattr(STDIN_FILENO, &s_originalTermios);
	s_termiosRestored = false;

	// Enter alternate screen buffer (like vim) — restores main screen on exit
	std::cout << "\033[?1049h" << std::flush;

	// Register SIGWINCH handler for terminal resize detection.
	// No SA_RESTART: let read() in getKey() return EINTR so the menu loop
	// can immediately detect the resize and redraw without waiting for input.
	struct sigaction sa = {};
	sa.sa_handler = handleSigwinch;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGWINCH, &sa, nullptr);

	// Set raw mode: char-at-a-time, no echo, no signal generation, blocking reads.
	// ISIG disabled so Ctrl+C doesn't kill the process — quit via in-app menu.
	// VMIN=1 is needed so blocking reads work properly in menus.
	// The game loop uses select() in Input::pollKeys() to avoid blocking.
	struct termios raw = s_originalTermios;
	raw.c_lflag &= ~(ICANON | ECHO | ISIG);
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

	updateOffsets();
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
	// Leave alternate screen buffer — restores the original main screen
	std::cout << "\033[?1049l" << std::flush;
}

void Platform::flushInput()
{
	tcflush(STDIN_FILENO, TCIFLUSH);
}

void Platform::flushOutput()
{
	std::cout << std::flush;
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
		timeval tv{};
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
						default: ;
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

int Platform::getKeyTimeout(int timeoutMs)
{
	fd_set fds;
	timeval tv{};
	FD_ZERO(&fds);
	FD_SET(STDIN_FILENO, &fds);
	tv.tv_sec = timeoutMs / 1000;
	tv.tv_usec = (timeoutMs % 1000) * 1000;
	if (::select(STDIN_FILENO + 1, &fds, nullptr, nullptr, &tv) > 0)
		return getKey();
	return -1;
}

bool Platform::wasResized()
{
	if (!s_resized)
		return false;

	s_resized = 0;

	if (isTerminalTooSmall()) {
		showResizePrompt();
		return false;
	}

	updateOffsets();
	rlutil::cls();
	rlutil::setColor(rlutil::WHITE);
	rlutil::setBackgroundColor(rlutil::BLACK);
	return true;
}

void Platform::showResizePrompt()
{
	rlutil::cls();
	int row = std::max(1, rlutil::trows() / 2);
	rlutil::locate(1, row);
	rlutil::setColor(rlutil::WHITE);
	std::cout << "  Please resize terminal to "
	          << CONSOLE_WIDTH << "x" << CONSOLE_HEIGHT << std::flush;
}

bool Platform::isTerminalTooSmall()
{
	return rlutil::tcols() < CONSOLE_WIDTH || rlutil::trows() < CONSOLE_HEIGHT;
}

int Platform::offsetX()
{
	return s_offsetX;
}

int Platform::offsetY()
{
	return s_offsetY;
}

void Platform::updateOffsets()
{
	int cols = rlutil::tcols();
	int rows = rlutil::trows();
	s_offsetX = std::max(0, (cols - CONSOLE_WIDTH) / 2);
	s_offsetY = std::max(0, (rows - CONSOLE_HEIGHT) / 2);
}

std::string Platform::getDataDir()
{
	std::string dir;
	const char* xdg = std::getenv("XDG_DATA_HOME");
	if (xdg && xdg[0] != '\0')
		dir = std::string(xdg) + "/TetrisConsole";
	else {
		const char* home = std::getenv("HOME");
		dir = std::string(home ? home : "/tmp") + "/.local/share/TetrisConsole";
	}
	mkdir(dir.c_str(), 0755);
	return dir;
}
