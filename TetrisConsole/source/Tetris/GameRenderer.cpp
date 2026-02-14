#include "GameRenderer.h"

#include "GameState.h"
#include "Platform.h"
#include "SoundEngine.h"
#include "Color.h"

GameRenderer::GameRenderer()
    : _next("Next"), _hold("Hold"), _nextQueuePanel(12), _muteIcon("♪")
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
    _muteIcon.setPosition(78 + ox, 2 + oy);
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

void GameRenderer::render(GameState& state, const bool playfieldVisible) {
    // Highscore must be updated before rendering so the panel shows the current value
    if (state._score > state._highscore)
        state._hasBetterHighscore = true;
    if (state._hasBetterHighscore)
        state._highscore = state._score;

    _playfield.update(state, playfieldVisible);
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
    Platform::flushOutput();
}

void GameRenderer::drawMuteIndicator() {
    switch (SoundEngine::getMuteState()) {
        case MuteState::UNMUTED:      _muteIcon.setColor(Color::WHITE);  break;
        case MuteState::MUSIC_MUTED:  _muteIcon.setColor(Color::YELLOW); break;
        case MuteState::ALL_MUTED:    _muteIcon.setColor(Color::RED);    break;
    }
    _muteIcon.draw();
}
