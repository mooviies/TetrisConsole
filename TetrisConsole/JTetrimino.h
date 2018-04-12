#pragma once

#include <string>

#include "Tetrimino.h"

using namespace std;

class JTetrimino : public Tetrimino
{
public:
	JTetrimino(int** matrix);
	virtual ~JTetrimino();
	virtual int getColor() const { return rlutil::BLUE; }
};

