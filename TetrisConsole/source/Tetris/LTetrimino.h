#pragma once

#include "Tetrimino.h"

using namespace std;

class LTetrimino : public Tetrimino
{
public:
	LTetrimino(vector<vector<int>>& matrix);
	~LTetrimino() override;
	[[nodiscard]] int getColor() const override { return rlutil::BROWN; }
};

