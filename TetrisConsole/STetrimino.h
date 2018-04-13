#pragma once

#include <string>

#include "Tetrimino.h"

using namespace std;

class STetrimino : public Tetrimino
{
public:
	STetrimino(vector<vector<int>>& matrix);
	virtual ~STetrimino();
	virtual int getColor() const { return rlutil::GREEN; }
};

