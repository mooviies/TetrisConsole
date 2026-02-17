#pragma once

#include "Icon.h"
#include "Constants.h"
#include "ScoreDisplay.h"
#include "PieceDisplay.h"
#include "PlayfieldDisplay.h"

class GameState;

class GameRenderer {
public:
    GameRenderer();
    ~GameRenderer();

    void configure(int previewCount, bool holdEnabled, bool showGoal);
    void invalidate();
    void render(const GameState &state, bool playfieldVisible = true);
    void renderTimer(const GameState &state);
    static void renderTitle(const std::string &subtitle);

private:
    void updatePositions();
    void drawMuteIndicator();

    ScoreDisplay _score;
    PieceDisplay _next;
    PieceDisplay _hold;
    PlayfieldDisplay _playfield;
    Icon _muteIcon;
    int _previewCount = 6;
    bool _holdEnabled = true;
    bool _wasShowingNotification{};
};
