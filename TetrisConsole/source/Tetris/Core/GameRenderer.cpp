#include "GameRenderer.h"

#include <algorithm>
#include <iostream>
#include <vector>

#include "GameState.h"
#include "Platform.h"
#include "SoundEngine.h"
#include "Color.h"
#include "rlutil.h"

namespace {
namespace Layout {
constexpr int kScoreX = 4, kScoreY = 12;
constexpr int kPlayfieldX = 30, kPlayfieldY = 6;
constexpr int kNextX = 59, kNextY = 6;
constexpr int kHoldX = 7, kHoldY = 6;
constexpr int kMuteX = 78, kMuteY = 2;
constexpr int kSideNotifWidth = 12;                         // next-piece queue interior width
constexpr int kSideNotifBaseY = kPlayfieldY + VISIBLE_ROWS; // 2nd-to-last playfield row
} // namespace Layout

std::vector<std::string> wrapText(const std::string &text, int maxWidth) {
    std::vector<std::string> lines;
    if (static_cast<int>(text.length()) <= maxWidth) {
        lines.push_back(text);
        return lines;
    }
    auto pos = text.rfind(' ', static_cast<size_t>(maxWidth));
    if (pos == std::string::npos) {
        lines.push_back(text.substr(0, static_cast<size_t>(maxWidth)));
        lines.push_back(text.substr(static_cast<size_t>(maxWidth)));
    } else {
        lines.push_back(text.substr(0, pos));
        lines.push_back(text.substr(pos + 1));
    }
    return lines;
}

void renderCenteredLine(int x, int y, int width, const std::string &text, int color) {
    const auto textLen = static_cast<int>(text.length());
    const int leftPad = (width - textLen) / 2;
    const int rightPad = width - textLen - leftPad;
    rlutil::locate(x, y);
    rlutil::setBackgroundColor(Color::BLACK);
    rlutil::setColor(color);
    std::cout << std::string(static_cast<size_t>(leftPad), ' ') << text
              << std::string(static_cast<size_t>(rightPad), ' ');
}
} // namespace

GameRenderer::GameRenderer() : _muteIcon("♪") {
}

GameRenderer::~GameRenderer() = default;

void GameRenderer::configure(const int previewCount, const bool holdEnabled, const bool showGoal) {
    _next.clear();
    _previewCount = previewCount;
    if (previewCount > 0) _next.rebuild(static_cast<size_t>(previewCount));

    updatePositions();

    if (holdEnabled != _holdEnabled) {
        if (!holdEnabled) _hold.clear();
        _holdEnabled = holdEnabled;
    }

    _score.configure(showGoal);
}

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
    if (_previewCount > 0) _next.invalidate();
    if (_holdEnabled) _hold.invalidate();
}

void GameRenderer::render(const GameState &state, const bool playfieldVisible) {
    _playfield.update(state, playfieldVisible);
    if (_previewCount > 0)
        _next.update(playfieldVisible ? state.peekTetriminos(static_cast<size_t>(_previewCount))
                                      : std::vector<const Tetrimino *>{});
    if (_holdEnabled)
        _hold.update(playfieldVisible ? std::vector<const Tetrimino *>{state.pieces.hold}
                                      : std::vector<const Tetrimino *>{});
    _score.update(state);
    drawMuteIndicator();

    _score.render();
    _playfield.render();
    if (_previewCount > 0) _next.render();
    if (_holdEnabled) _hold.render();

    // Side notification overlay (levels 11+): render over bottom of next-piece queue
    const auto &lc = state.lineClear;
    const bool hasNotification = state.phase == GamePhase::Animate && state.stats.level > OVERLAY_LEVEL_THRESHOLD &&
                                 (!lc.notificationText.empty() || !lc.comboText.empty());

    if (hasNotification) {
        const int ox = Platform::offsetX();
        const int oy = Platform::offsetY();
        const int baseX = Layout::kNextX + 1 + ox;
        const int baseY = Layout::kSideNotifBaseY + oy;

        if (!lc.notificationText.empty()) {
            auto lines = wrapText(lc.notificationText, Layout::kSideNotifWidth);
            const int startY = baseY - static_cast<int>(lines.size()) + 1;
            for (size_t i = 0; i < lines.size(); i++)
                renderCenteredLine(baseX, startY + static_cast<int>(i), Layout::kSideNotifWidth, lines[i],
                                   lc.notificationColor);
        }

        if (!lc.comboText.empty())
            renderCenteredLine(baseX, baseY + 1, Layout::kSideNotifWidth, lc.comboText, lc.comboColor);

        rlutil::setColor(Color::WHITE);
        rlutil::setBackgroundColor(Color::BLACK);
    }

    if (_wasShowingNotification && !hasNotification) {
        _next.invalidate();
        if (_previewCount > 0) _next.render();
    }
    _wasShowingNotification = hasNotification;

    Platform::flushOutput();
}

void GameRenderer::renderTimer(const GameState &state) {
    _score.updateTimer(state);
    _score.render();
    Platform::flushOutput();
}

void GameRenderer::renderTitle(const std::string &subtitle) {
    const int ox = Platform::offsetX();
    const int oy = Platform::offsetY();

    Panel title(78);
    title.addRow("Tetris Console", Align::Center);
    title.addSeparator();
    title.addRow(subtitle, Align::Center);
    title.setPosition(1 + ox, 1 + oy);
    title.render();
}

void GameRenderer::drawMuteIndicator() {
    switch (SoundEngine::getMuteState()) {
        case MuteState::Unmuted: _muteIcon.setColor(Color::WHITE); break;
        case MuteState::MusicMuted: _muteIcon.setColor(Color::YELLOW); break;
        case MuteState::AllMuted: _muteIcon.setColor(Color::RED); break;
    }
    _muteIcon.draw();
}
