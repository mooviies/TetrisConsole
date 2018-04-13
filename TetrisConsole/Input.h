#pragma once

class Input
{
public:
	static bool left();
	static bool right();
	static bool hardDrop();
	static bool softDrop();
	static bool rotateClockwise();
	static bool rotateCounterClockwise();
	static bool hold();
	static bool pause();

private:
	Input();
	~Input();
};

