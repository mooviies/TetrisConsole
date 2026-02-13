#pragma once

#include "Tetrimino.h"

class LTetrimino : public Tetrimino
{
public:
	LTetrimino(std::vector<std::vector<int>>& matrix);
	~LTetrimino() override;
	[[nodiscard]] int getColor() const override { return rlutil::BROWN; }
};

