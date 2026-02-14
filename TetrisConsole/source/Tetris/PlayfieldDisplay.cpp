#include "PlayfieldDisplay.h"

#include <iostream>

#include "GameState.h"
#include "Constants.h"
#include "rlutil.h"

using namespace std;

class PlayfieldElement : public PanelElement {
public:
    [[nodiscard]] int height() const override { return 20; }
    void drawRow(int rowIndex, int x, int y, int interiorWidth) const override;

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

void PlayfieldElement::drawRow(int rowIndex, int x, int y, int /*interiorWidth*/) const {
    if (_state == nullptr)
        return;

    rlutil::locate(x, y);
    rlutil::setColor(rlutil::WHITE);
    cout << "║";

    int line = MATRIX_START + rowIndex;
    for (int i = 0; i < TETRIS_WIDTH; i++) {
        bool currentTetriminoHere = false;
        if (_state->_currentTetrimino != nullptr)
            currentTetriminoHere = _state->_currentTetrimino->isMino(line, i);

        if (_visible && (_state->_matrix[line][i] || currentTetriminoHere)) {
            if (currentTetriminoHere)
                rlutil::setColor(_state->_currentTetrimino->getColor());
            else
                rlutil::setColor(_state->_matrix[line][i]);

            if (currentTetriminoHere)
                cout << "██";
            else {
                rlutil::setColor(rlutil::BLACK);
                rlutil::setBackgroundColor(_state->_matrix[line][i]);
                cout << "░░";
                rlutil::setBackgroundColor(rlutil::BLACK);
            }

            rlutil::setColor(rlutil::WHITE);
        } else {
            rlutil::setColor(rlutil::DARKGREY);
            if ((line % 2 == 0 && i % 2 != 0) || (line % 2 != 0 && i % 2 == 0))
                cout << "░░";
            else
                cout << "▒▒";
        }
    }

    rlutil::setColor(rlutil::WHITE);
    cout << "║";
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
