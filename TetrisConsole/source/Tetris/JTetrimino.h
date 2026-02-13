#pragma once

#include "Tetrimino.h"

class JTetrimino : public Tetrimino
{
public:
	JTetrimino(std::vector<std::vector<int>>& matrix);
	~JTetrimino() override;
	[[nodiscard]] int getColor() const override { return rlutil::BLUE; }
};

