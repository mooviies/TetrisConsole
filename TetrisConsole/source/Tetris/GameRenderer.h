#pragma once

#include "Panel.h"
#include <cstddef>

class GameState;

class GameRenderer
{
public:
	GameRenderer();
	~GameRenderer();

	void display() const;
	void refresh(GameState& state);
	static void printMatrix(const GameState& state, bool visible = true);

private:
	static void printLine(const GameState& state, int line, bool visible);
	static void printPreview(const GameState& state);
	void printScore(const GameState& state);

	Panel _scorePanel;
	Panel _playfieldPanel;
	Panel _nextPanel;
	Panel _holdPanel;
	Panel _highScorePanel;

	size_t _scoreValueRow;
	size_t _levelRow;
	size_t _linesRow;
	size_t _highScoreValueRow;
};
