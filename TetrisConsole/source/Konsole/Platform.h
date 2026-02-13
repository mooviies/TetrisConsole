#pragma once

#include <string>

class Platform
{
public:
	static void initConsole();
	static void cleanupConsole();
	static void flushInput();
	static int getKey();
	static bool wasResized();
	static bool isTerminalTooSmall();
	static int offsetX();
	static int offsetY();
	static void updateOffsets();
	static std::string getDataDir();

	Platform() = delete;
	~Platform() = delete;
};
