#pragma once

class Platform
{
public:
	static void initConsole();
	static void cleanupConsole();
	static void flushInput();

private:
	Platform();
	~Platform();
};
