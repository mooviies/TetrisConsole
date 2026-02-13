#pragma once

#include "Tetrimino.h"

using namespace std;

class ZTetrimino : public Tetrimino
{
public:
	ZTetrimino(vector<vector<int>>& matrix);
	~ZTetrimino() override;
	[[nodiscard]] int getColor() const override { return rlutil::RED; }
};

