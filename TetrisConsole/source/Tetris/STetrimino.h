#pragma once

#include "Tetrimino.h"

using namespace std;

class STetrimino : public Tetrimino
{
public:
	STetrimino(vector<vector<int>>& matrix);
	~STetrimino() override;
	[[nodiscard]] int getColor() const override { return rlutil::GREEN; }
};

