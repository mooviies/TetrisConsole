#pragma once

#include <cstddef>

#include "Panel.h"

class GameState;

class ScoreDisplay {
public:
	ScoreDisplay();

	void update(const GameState& state);
	void setPosition(int x, int y);
	void invalidate();
	void render();

private:
	Panel _panel;
	size_t _scoreValueRow;
	size_t _levelRow;
	size_t _linesRow;
};
