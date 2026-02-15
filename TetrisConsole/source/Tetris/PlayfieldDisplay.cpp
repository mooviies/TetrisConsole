#include "PlayfieldDisplay.h"

#include "Color.h"
#include "GameState.h"
#include "Constants.h"
#include "Vector2i.h"

using namespace std;

class PlayfieldElement : public PanelElement {
public:
    [[nodiscard]] int height() const override { return 20; }
    void drawRow(int rowIndex, RowDrawContext& ctx) const override;

    void update(const GameState& state, bool visible);

private:
    const GameState* _state = nullptr;  // non-owning; lifetime guaranteed by game loop
    bool _visible = true;
    int _ghostDropDistance = 0;
};

void PlayfieldElement::update(const GameState& state, bool visible) {
    _state = &state;
    _visible = visible;

    _ghostDropDistance = 0;
    if (state.config.ghostEnabled && state.pieces.current != nullptr) {
        while (state.pieces.current->simulateMove(Vector2i(_ghostDropDistance + 1, 0)))
            _ghostDropDistance++;
    }

    markDirty();
}

void PlayfieldElement::drawRow(int rowIndex, RowDrawContext& ctx) const {
    if (_state == nullptr)
        return;

    const int line = MATRIX_START + rowIndex;
    const auto* tetrimino = _state->pieces.current;
    for (int i = 0; i < TETRIS_WIDTH; i++) {
        bool currentTetriminoHere = false;
        bool ghostHere = false;
        if (tetrimino != nullptr) {
            currentTetriminoHere = tetrimino->isMino(line, i);
            ghostHere = _ghostDropDistance > 0
                        && tetrimino->isMino(line - _ghostDropDistance, i);
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

void PlayfieldDisplay::update(const GameState& state, const bool visible) const {
    _element->update(state, visible);
}

void PlayfieldDisplay::setPosition(int x, int y) { _panel.setPosition(x, y); }
void PlayfieldDisplay::invalidate() { _panel.invalidate(); }
void PlayfieldDisplay::render() { _panel.render(); }
