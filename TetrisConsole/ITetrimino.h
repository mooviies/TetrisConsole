#pragma once

#include <string>

#include "Tetrimino.h"

using namespace std;

class ITetrimino : public Tetrimino
{
public:
	ITetrimino(int** matrix);
	virtual ~ITetrimino();
	virtual int getColor() const { return rlutil::WHITE; }
};

