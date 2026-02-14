#pragma once

#include <string>

class Platform
{
public:
	static void initConsole();
	static void cleanupConsole();
	static void flushInput();
	static void flushOutput();
	static int getKey();
	static bool wasResized();
	static bool isTerminalTooSmall();
	static void showResizePrompt();
	static int offsetX();
	static int offsetY();
	static void updateOffsets();
	static std::string getDataDir();

	Platform() = delete;
	~Platform() = delete;
};
