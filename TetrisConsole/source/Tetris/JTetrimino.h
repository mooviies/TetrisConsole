#pragma once

#include "Tetrimino.h"

using namespace std;

class JTetrimino : public Tetrimino
{
public:
	JTetrimino(vector<vector<int>>& matrix);
	~JTetrimino() override;
	[[nodiscard]] int getColor() const override { return rlutil::BLUE; }
};

