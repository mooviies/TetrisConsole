#include "PieceDisplay.h"

#include "PiecePreview.h"
#include "Tetrimino.h"

PieceDisplay::PieceDisplay(const size_t size) : _size(size), _panel(12) {
    if (_size == 0) _size = 1;

    _panel.addRow("");
    _pieces.push_back(std::make_shared<PiecePreview>());
    _panel.addElement(_pieces.back());
    _panel.addRow("");

    if (size > 1) {
        for (size_t i = 1; i < _size; i++) {
            _pieces.push_back(std::make_shared<PiecePreview>());
            _panel.addElement(_pieces.back());
            _panel.addRow("");
        }
        _panel.addRow("");
    }
}

PieceDisplay::~PieceDisplay() = default;

void PieceDisplay::update(const std::vector<const Tetrimino *> &pieces) const {
    for (size_t i = 0; i < _pieces.size(); i++) {
        if (i < pieces.size())
            _pieces[i]->setPiece(pieces[i]);
        else
            _pieces[i]->clearPiece();
    }
}

void PieceDisplay::setPosition(const int x, const int y) {
    _panel.setPosition(x, y);
}
void PieceDisplay::invalidate() {
    _panel.invalidate();
}
void PieceDisplay::render() {
    _panel.render();
}
void PieceDisplay::clear() const {
    _panel.clear();
}

void PieceDisplay::rebuild(const size_t size) {
    _size = size;
    _pieces.clear();
    _panel = Panel(12);

    if (_size == 0) _size = 1;

    _panel.addRow("");
    _pieces.push_back(std::make_shared<PiecePreview>());
    _panel.addElement(_pieces.back());
    _panel.addRow("");

    if (_size > 1) {
        for (size_t i = 1; i < _size; i++) {
            _pieces.push_back(std::make_shared<PiecePreview>());
            _panel.addElement(_pieces.back());
            _panel.addRow("");
        }
        _panel.addRow("");
    }
}
