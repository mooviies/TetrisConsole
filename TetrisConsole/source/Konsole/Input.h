#pragma once

class Input
{
public:
	static void init();
	static void cleanup();
	static void pollKeys();

	static bool left();
	static bool right();
	static bool hardDrop();
	static bool softDrop();
	static bool rotateClockwise();
	static bool rotateCounterClockwise();
	static bool hold();
	static bool pause();
	static bool select();

private:
	Input();
	~Input();
};
