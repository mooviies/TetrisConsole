#pragma once

#include <string>

class Platform
{
public:
	static void initConsole();
	static void cleanupConsole();
	static void flushInput();
	static int getKey();
	static std::string getDataDir();

	Platform() = delete;
	~Platform() = delete;
};
