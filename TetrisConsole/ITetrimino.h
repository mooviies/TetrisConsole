#pragma once

#include <string>

#include "Tetrimino.h"

using namespace std;

class ITetrimino : public Tetrimino
{
public:
	ITetrimino(vector<vector<int>>& matrix);
	virtual ~ITetrimino();
	virtual int getColor() const { return rlutil::LIGHTBLUE; }
	virtual Vector2i getStartingPosition() const { return Vector2i(19, 4); }
};

