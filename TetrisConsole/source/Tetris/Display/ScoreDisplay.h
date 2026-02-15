#pragma once

#include <cstddef>

#include "Panel.h"

class GameState;

class ScoreDisplay {
public:
	ScoreDisplay();

	void update(const GameState& state);
	void updateTimer(const GameState& state);
	void setPosition(int x, int y);
	void invalidate();
	void render();

private:
	Panel _panel;
	size_t _scoreValueRow;
	size_t _timeValueRow;
	size_t _levelRow;
	size_t _goalRow;
	size_t _linesRow;
	size_t _tpmRow;
	size_t _lpmRow;
	size_t _tetrisRow;
	size_t _combosRow;
	size_t _tSpinsRow;
};
