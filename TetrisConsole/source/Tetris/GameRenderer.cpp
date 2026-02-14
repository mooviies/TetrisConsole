#include "GameRenderer.h"

#include <iostream>

#include "GameState.h"
#include "Platform.h"
#include "SoundEngine.h"
#include "rlutil.h"

using namespace std;

GameRenderer::GameRenderer()
    : _next("Next"), _hold("Hold"), _nextQueuePanel(12)
{
    for (int i = 0; i < 12; i++)
        _nextQueuePanel.addRow("");
}

GameRenderer::~GameRenderer() = default;

void GameRenderer::updatePositions() {
    const int ox = Platform::offsetX();
    const int oy = Platform::offsetY();

    _score.setPosition(5 + ox, 14 + oy);
    _playfield.setPosition(30 + ox, 6 + oy);
    _next.setPosition(59 + ox, 6 + oy);
    _nextQueuePanel.setPosition(59 + ox, 14 + oy);
    _highScore.setPosition(5 + ox, 23 + oy);
    _hold.setPosition(7 + ox, 6 + oy);
}

void GameRenderer::invalidate() {
    updatePositions();
    _score.invalidate();
    _playfield.invalidate();
    _next.invalidate();
    _nextQueuePanel.invalidate();
    _highScore.invalidate();
    _hold.invalidate();
}

void GameRenderer::refresh(GameState& state) {
    // Highscore must be updated before rendering so the panel shows the current value
    if (state._score > state._highscore)
        state._hasBetterHighscore = true;
    if (state._hasBetterHighscore)
        state._highscore = state._score;

    _playfield.update(state);
    _next.update(state.peekTetrimino());
    _hold.update(state._holdTetrimino);
    _score.update(state);
    _highScore.update(state._highscore);
    drawMuteIndicator();

    _score.render();
    _playfield.render();
    _highScore.render();
    _next.render();
    _nextQueuePanel.render();
    _hold.render();
    cout << flush;
}

void GameRenderer::printMatrix(const GameState& state, const bool visible) {
    _playfield.update(state, visible);
    _playfield.render();
    cout << flush;
}

void GameRenderer::drawMuteIndicator() {
    const int ox = Platform::offsetX();
    const int oy = Platform::offsetY();
    rlutil::locate(78 + ox, 2 + oy);
    switch (SoundEngine::getMuteState()) {
        case MuteState::UNMUTED:      rlutil::setColor(rlutil::WHITE);  break;
        case MuteState::MUSIC_MUTED:  rlutil::setColor(rlutil::YELLOW); break;
        case MuteState::ALL_MUTED:    rlutil::setColor(rlutil::RED);    break;
    }
    cout << "♪";
    rlutil::setColor(rlutil::WHITE);
}
