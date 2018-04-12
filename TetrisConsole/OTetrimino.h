#pragma once

#include <string>

#include "Tetrimino.h"

using namespace std;

class OTetrimino : public Tetrimino
{
public:
	OTetrimino(int** matrix);
	virtual ~OTetrimino();
	virtual int getColor() const { return rlutil::YELLOW; }
};

