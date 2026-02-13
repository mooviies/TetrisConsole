#pragma once

class Platform
{
public:
	static void initConsole();
	static void cleanupConsole();
	static void flushInput();
	static int getKey();

private:
	Platform();
	~Platform();
};
