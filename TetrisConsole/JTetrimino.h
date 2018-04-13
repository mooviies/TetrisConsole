#pragma once

#include <string>

#include "Tetrimino.h"

using namespace std;

class JTetrimino : public Tetrimino
{
public:
	JTetrimino(vector<vector<int>>& matrix);
	virtual ~JTetrimino();
	virtual int getColor() const { return rlutil::BLUE; }
};

