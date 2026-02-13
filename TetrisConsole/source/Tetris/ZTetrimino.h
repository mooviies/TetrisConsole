#pragma once

#include "Tetrimino.h"

class ZTetrimino : public Tetrimino
{
public:
	ZTetrimino(std::vector<std::vector<int>>& matrix);
	~ZTetrimino() override;
	[[nodiscard]] int getColor() const override { return rlutil::RED; }
};

