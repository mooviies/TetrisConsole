#pragma once

#include <string>

#include "Tetrimino.h"

using namespace std;

class OTetrimino : public Tetrimino
{
public:
	OTetrimino(vector<vector<int>>& matrix);
	~OTetrimino() override;
	[[nodiscard]] int getColor() const override { return rlutil::YELLOW; }
};

