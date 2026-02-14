#pragma once

#include "Panel.h"
#include "ScoreDisplay.h"
#include "HighScoreDisplay.h"
#include "PieceDisplay.h"
#include "PlayfieldDisplay.h"

class GameState;

class GameRenderer
{
public:
	GameRenderer();
	~GameRenderer();

	void invalidate();
	void refresh(GameState& state);
	void printMatrix(const GameState& state, bool visible = true);

private:
	void updatePositions();
	static void drawMuteIndicator();

	ScoreDisplay _score;
	HighScoreDisplay _highScore;
	PieceDisplay _next;
	PieceDisplay _hold;
	PlayfieldDisplay _playfield;
	Panel _nextQueuePanel;
};
