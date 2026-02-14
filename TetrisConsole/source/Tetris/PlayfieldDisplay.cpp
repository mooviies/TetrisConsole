#include "PlayfieldDisplay.h"

#include "Color.h"
#include "GameState.h"
#include "Constants.h"

using namespace std;

class PlayfieldElement : public PanelElement {
public:
    [[nodiscard]] int height() const override { return 20; }
    void drawRow(int rowIndex, RowDrawContext& ctx) const override;

    void update(const GameState& state, bool visible);

private:
    const GameState* _state = nullptr;
    bool _visible = true;
};

void PlayfieldElement::update(const GameState& state, bool visible) {
    _state = &state;
    _visible = visible;
    markDirty();
}

void PlayfieldElement::drawRow(int rowIndex, RowDrawContext& ctx) const {
    if (_state == nullptr)
        return;

    int line = MATRIX_START + rowIndex;
    for (int i = 0; i < TETRIS_WIDTH; i++) {
        bool currentTetriminoHere = false;
        if (_state->_currentTetrimino != nullptr)
            currentTetriminoHere = _state->_currentTetrimino->isMino(line, i);

        if (_visible && (_state->_matrix[line][i] || currentTetriminoHere)) {
            if (currentTetriminoHere) {
                ctx.setColor(_state->_currentTetrimino->getColor());
                ctx.print("██");
            } else {
                ctx.setColor(Color::BLACK);
                ctx.setBackgroundColor(_state->_matrix[line][i]);
                ctx.print("░░");
                ctx.setBackgroundColor(Color::BLACK);
            }

            ctx.setColor(Color::WHITE);
        } else {
            ctx.setColor(Color::DARKGREY);
            if ((line % 2 == 0 && i % 2 != 0) || (line % 2 != 0 && i % 2 == 0))
                ctx.print("░░");
            else
                ctx.print("▒▒");
        }
    }
}

PlayfieldDisplay::PlayfieldDisplay()
    : _panel(20), _element(std::make_shared<PlayfieldElement>())
{
    _panel.addElement(_element);
}

PlayfieldDisplay::~PlayfieldDisplay() = default;

void PlayfieldDisplay::update(const GameState& state, bool visible) {
    _element->update(state, visible);
}

void PlayfieldDisplay::setPosition(int x, int y) { _panel.setPosition(x, y); }
void PlayfieldDisplay::invalidate() { _panel.invalidate(); }
void PlayfieldDisplay::render() { _panel.render(); }
