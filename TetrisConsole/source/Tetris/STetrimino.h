#pragma once

#include "Tetrimino.h"

class STetrimino : public Tetrimino
{
public:
	STetrimino(std::vector<std::vector<int>>& matrix);
	~STetrimino() override;
	[[nodiscard]] int getColor() const override { return rlutil::GREEN; }
};

