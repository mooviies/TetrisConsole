#pragma once

#include "Tetrimino.h"

class ITetrimino : public Tetrimino
{
public:
	ITetrimino(std::vector<std::vector<int>>& matrix);
	~ITetrimino() override;
	[[nodiscard]] int getColor() const override { return rlutil::LIGHTBLUE; }
	[[nodiscard]] Vector2i getStartingPosition() const override { return {19, 4}; }
};

