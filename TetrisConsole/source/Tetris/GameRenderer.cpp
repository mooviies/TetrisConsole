#include "GameRenderer.h"

#include "GameState.h"
#include "Platform.h"
#include "SoundEngine.h"
#include "Color.h"

namespace {
namespace Layout {
    constexpr int kScoreX = 4,      kScoreY = 12;
    constexpr int kPlayfieldX = 30, kPlayfieldY = 6;
    constexpr int kNextX = 59,      kNextY = 6;
    constexpr int kHoldX = 7,       kHoldY = 6;
    constexpr int kMuteX = 78,      kMuteY = 2;
}}

GameRenderer::GameRenderer()
    : _next(NEXT_PIECE_QUEUE_SIZE), _muteIcon("♪")
{
}

GameRenderer::~GameRenderer() = default;

void GameRenderer::updatePositions() {
    const int ox = Platform::offsetX();
    const int oy = Platform::offsetY();

    _score.setPosition(Layout::kScoreX + ox, Layout::kScoreY + oy);
    _playfield.setPosition(Layout::kPlayfieldX + ox, Layout::kPlayfieldY + oy);
    _next.setPosition(Layout::kNextX + ox, Layout::kNextY + oy);
    _hold.setPosition(Layout::kHoldX + ox, Layout::kHoldY + oy);
    _muteIcon.setPosition(Layout::kMuteX + ox, Layout::kMuteY + oy);
}

void GameRenderer::invalidate() {
    updatePositions();
    _score.invalidate();
    _playfield.invalidate();
    _next.invalidate();
    _hold.invalidate();
}

void GameRenderer::render(const GameState& state, const bool playfieldVisible) {
    _playfield.update(state, playfieldVisible);
    _next.update(state.peekTetriminos(NEXT_PIECE_QUEUE_SIZE));
    _hold.update({state.holdTetrimino()});
    _score.update(state);
    drawMuteIndicator();

    _score.render();
    _playfield.render();
    _next.render();
    _hold.render();
    Platform::flushOutput();
}

void GameRenderer::renderTimer(const GameState& state) {
    _score.updateTimer(state);
    _score.render();
    Platform::flushOutput();
}

void GameRenderer::renderTitle(const std::string& subtitle) {
    const int ox = Platform::offsetX();
    const int oy = Platform::offsetY();

    Panel title(78);
    title.addRow("Tetris Console", Align::CENTER);
    title.addSeparator();
    title.addRow(subtitle, Align::CENTER);
    title.setPosition(1 + ox, 1 + oy);
    title.render();
}

void GameRenderer::drawMuteIndicator() {
    switch (SoundEngine::getMuteState()) {
        case MuteState::UNMUTED:      _muteIcon.setColor(Color::WHITE);  break;
        case MuteState::MUSIC_MUTED:  _muteIcon.setColor(Color::YELLOW); break;
        case MuteState::ALL_MUTED:    _muteIcon.setColor(Color::RED);    break;
    }
    _muteIcon.draw();
}
