#pragma once

#include "Panel.h"
#include "Icon.h"
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
	void render(GameState& state, bool playfieldVisible = true);

private:
	void updatePositions();
	void drawMuteIndicator();

	ScoreDisplay _score;
	HighScoreDisplay _highScore;
	PieceDisplay _next;
	PieceDisplay _hold;
	PlayfieldDisplay _playfield;
	Panel _nextQueuePanel;
	Icon _muteIcon;
};
