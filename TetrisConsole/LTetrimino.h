#pragma once

#include <string>

#include "Tetrimino.h"

using namespace std;

class LTetrimino : public Tetrimino
{
public:
	LTetrimino(vector<vector<int>>& matrix);
	virtual ~LTetrimino();
	virtual int getColor() const { return rlutil::BROWN; }
};

