#pragma once

#include <memory>
#include <cstddef>

#include "Panel.h"

class GameState;
class PiecePreview;

class GameRenderer
{
public:
	GameRenderer();
	~GameRenderer();

	void invalidate();
	void refresh(GameState& state);
	static void printMatrix(const GameState& state, bool visible = true);

private:
	void updatePositions();
	static void printLine(const GameState& state, int line, bool visible);
	void printPreview(const GameState& state) const;
	void printScore(const GameState& state);

	Panel _scorePanel;
	Panel _playfieldPanel;
	Panel _nextPanel;
	Panel _nextQueuePanel;
	Panel _holdPanel;
	Panel _highScorePanel;

	size_t _scoreValueRow;
	size_t _levelRow;
	size_t _linesRow;
	size_t _highScoreValueRow;

	std::shared_ptr<PiecePreview> _nextPiece;
	std::shared_ptr<PiecePreview> _holdPiece;
	size_t _nextPieceRow;
	size_t _holdPieceRow;
};
