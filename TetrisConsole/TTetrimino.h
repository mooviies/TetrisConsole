#pragma once

#include <string>

#include "Tetrimino.h"

using namespace std;

class TTetrimino : public Tetrimino
{
public:
	TTetrimino(vector<vector<int>>& matrix);
	virtual ~TTetrimino();
	virtual int getColor() const { return rlutil::MAGENTA; }
};

