#pragma once

#include <string>

#include "Tetrimino.h"

class OTetrimino : public Tetrimino
{
public:
	OTetrimino(std::vector<std::vector<int>>& matrix);
	~OTetrimino() override;
	[[nodiscard]] int getColor() const override { return rlutil::YELLOW; }
};

