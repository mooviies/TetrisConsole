#pragma once

#include "Tetris.h"

class Overseer
{
public:
	static void setTetris(Tetris* tetris);
	static Tetris& getTetris();

private:
	Overseer();
	~Overseer();

	static Tetris* _tetris;
};

