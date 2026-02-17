#include "PlayfieldDisplay.h"

#include <algorithm>
#include <iostream>

#include "Color.h"
#include "GameState.h"
#include "Constants.h"
#include "Vector2i.h"
#include "rlutil.h"

using namespace std;

class PlayfieldElement : public PanelElement {
public:
    [[nodiscard]] int height() const override { return 20; }
    void drawRow(int rowIndex, RowDrawContext &ctx) const override;

    void update(const GameState &state, bool visible);

private:
    const GameState *_state = nullptr; // non-owning; lifetime guaranteed by game loop
    bool _visible = true;
    int _ghostDropDistance = 0;
};

void PlayfieldElement::update(const GameState &state, const bool visible) {
    _state = &state;
    _visible = visible;

    _ghostDropDistance = 0;
    if (state.config.ghostEnabled && state.pieces.current != nullptr) {
        while (state.pieces.current->simulateMove(Vector2i(_ghostDropDistance + 1, 0)))
            _ghostDropDistance++;
    }

    markDirty();
}

void PlayfieldElement::drawRow(const int rowIndex, RowDrawContext &ctx) const {
    if (_state == nullptr) return;

    const int line = MATRIX_START + rowIndex;

    // Notification overlay (levels 1-10): steady text centered on playfield
    static constexpr int kNotificationRow = VISIBLE_ROWS / 2 - 1; // 9
    static constexpr int kComboRow = VISIBLE_ROWS / 2;            // 10
    if (_state->phase == GamePhase::Animate && _state->stats.level <= OVERLAY_LEVEL_THRESHOLD) {
        const std::string *text = nullptr;
        int color = 0;
        if (rowIndex == kNotificationRow && !_state->lineClear.notificationText.empty()) {
            text = &_state->lineClear.notificationText;
            color = _state->lineClear.notificationColor;
        } else if (rowIndex == kComboRow && !_state->lineClear.comboText.empty()) {
            text = &_state->lineClear.comboText;
            color = _state->lineClear.comboColor;
        }
        if (text != nullptr) {
            constexpr int totalWidth = BOARD_WIDTH * 2;
            const auto textLen = static_cast<int>(text->length());
            const int leftPad = (totalWidth - textLen) / 2;
            const int rightPad = totalWidth - textLen - leftPad;
            ctx.setBackgroundColor(Color::BLACK);
            ctx.setColor(Color::BLACK);
            ctx.print(std::string(static_cast<size_t>(leftPad), ' '));
            ctx.setColor(color);
            ctx.print(*text);
            ctx.setColor(Color::BLACK);
            ctx.print(std::string(static_cast<size_t>(rightPad), ' '));
            ctx.setBackgroundColor(Color::BLACK);
            return;
        }
    }

    // Line-clear flash: draw entire row as white blocks when flashing on
    if (_state->phase == GamePhase::Animate && _state->lineClear.flashOn) {
        const auto &rows = _state->lineClear.rows;
        if (std::find(rows.begin(), rows.end(), line) != rows.end()) {
            for (int i = 0; i < BOARD_WIDTH; i++) {
                ctx.setColor(Color::WHITE);
                ctx.print("██");
            }
            return;
        }
    }

    const auto *tetrimino = _state->pieces.current;
    for (int i = 0; i < BOARD_WIDTH; i++) {
        bool currentTetriminoHere = false;
        bool ghostHere = false;
        if (tetrimino != nullptr) {
            currentTetriminoHere = tetrimino->isMino(line, i);
            ghostHere = _ghostDropDistance > 0 && tetrimino->isMino(line - _ghostDropDistance, i);
        }

        if (_visible && (_state->matrix[line][i] || currentTetriminoHere)) {
            if (currentTetriminoHere) {
                ctx.setColor(tetrimino->getColor());
                ctx.print("██");
            } else {
                ctx.setColor(Color::BLACK);
                ctx.setBackgroundColor(_state->matrix[line][i]);
                ctx.print("░░");
                ctx.setBackgroundColor(Color::BLACK);
            }

            ctx.setColor(Color::WHITE);
        } else if (_visible && ghostHere) {
            ctx.setColor(Color::DARKGREY);
            ctx.print("██");
            ctx.setColor(Color::WHITE);
        } else if (_visible && _state->hardDropTrail.active
                   && _state->hardDropTrail.columns[i]
                   && line >= _state->hardDropTrail.visibleStartRow
                   && line < _state->hardDropTrail.endRow) {
            ctx.setColor(_state->hardDropTrail.color);
            ctx.print("░░");
        } else {
            ctx.setColor(Color::DARKGREY);
            if ((line % 2 == 0 && i % 2 != 0) || (line % 2 != 0 && i % 2 == 0))
                ctx.print("░░");
            else
                ctx.print("▒▒");
        }
    }
}

PlayfieldDisplay::PlayfieldDisplay() : _panel(20), _element(std::make_shared<PlayfieldElement>()) {
    _panel.addElement(_element);
}

PlayfieldDisplay::~PlayfieldDisplay() = default;

void PlayfieldDisplay::update(const GameState &state, const bool visible) {
    _element->update(state, visible);

    _skylineColors.fill(0);
    if (visible) {
        const auto *tetrimino = state.pieces.current;
        for (int i = 0; i < BOARD_WIDTH; i++) {
            if (tetrimino != nullptr && tetrimino->isMino(BUFFER_END, i))
                _skylineColors[i] = tetrimino->getColor();
            else if (state.matrix[BUFFER_END][i])
                _skylineColors[i] = state.matrix[BUFFER_END][i];
        }
    }
}

void PlayfieldDisplay::setPosition(const int x, const int y) {
    _x = x;
    _y = y;
    _panel.setPosition(x, y);
}
void PlayfieldDisplay::invalidate() {
    _panel.invalidate();
}

void PlayfieldDisplay::render() {
    _panel.render();
    drawSkylineBorder();
}

void PlayfieldDisplay::drawSkylineBorder() const {
    rlutil::locate(_x, _y);
    rlutil::setColor(Color::WHITE);
    std::cout << "╔";

    for (int i = 0; i < BOARD_WIDTH; i++) {
        if (_skylineColors[i]) rlutil::setColor(_skylineColors[i]);
        std::cout << "══";
        if (_skylineColors[i]) rlutil::setColor(Color::WHITE);
    }

    std::cout << "╗";
}
