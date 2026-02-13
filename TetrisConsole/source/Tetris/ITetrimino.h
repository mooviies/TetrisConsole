#pragma once

#include "Tetrimino.h"

using namespace std;

class ITetrimino : public Tetrimino
{
public:
	ITetrimino(vector<vector<int>>& matrix);
	~ITetrimino() override;
	[[nodiscard]] int getColor() const override { return rlutil::LIGHTBLUE; }
	[[nodiscard]] Vector2i getStartingPosition() const override { return {19, 4}; }
};

