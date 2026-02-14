#include "PieceDisplay.h"

#include "PiecePreview.h"
#include "Tetrimino.h"

PieceDisplay::PieceDisplay(const std::string& title, const size_t size)
    : _size(size), _panel(12)
{
    if (_size == 0) _size = 1;

    _panel.addRow(title, Align::CENTER);
    _panel.addSeparator();

    _panel.addRow("");
    _pieces.push_back(std::make_shared<PiecePreview>());
    _panel.addElement(_pieces.back());
    _panel.addRow("");

    if (size > 1) {
        _panel.addSeparator();
        _panel.addRow("");
        for (size_t i = 0; i < _size; i++) {
            _pieces.push_back(std::make_shared<PiecePreview>());
            _panel.addElement(_pieces.back());
            _panel.addRow("");
        }
    }
}

PieceDisplay::~PieceDisplay() = default;

void PieceDisplay::update(const Tetrimino* piece) const {
    for (size_t i = 0; i < _size - 1; i++) {
        _pieces[i]->setPiece(_pieces[i + 1].get());
    }
    _pieces[_size - 1]->setPiece(piece);
}

void PieceDisplay::setPosition(int x, int y) { _panel.setPosition(x, y); }
void PieceDisplay::invalidate() { _panel.invalidate(); }
void PieceDisplay::render() { _panel.render(); }
