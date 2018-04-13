#pragma once

#include <string>

#include "Tetrimino.h"

using namespace std;

class ZTetrimino : public Tetrimino
{
public:
	ZTetrimino(vector<vector<int>>& matrix);
	virtual ~ZTetrimino();
	virtual int getColor() const { return rlutil::RED; }
};

