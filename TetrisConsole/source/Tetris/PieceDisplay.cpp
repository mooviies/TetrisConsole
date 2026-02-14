#include "PieceDisplay.h"

#include "PiecePreview.h"
#include "Tetrimino.h"

PieceDisplay::PieceDisplay(const std::string& title)
    : _panel(12), _piece(std::make_shared<PiecePreview>())
{
    _panel.addRow(title, Align::CENTER);
    _panel.addSeparator();
    _panel.addElement(_piece);
    for (int i = 0; i < 2; i++)
        _panel.addRow("");
}

PieceDisplay::~PieceDisplay() = default;

void PieceDisplay::update(const Tetrimino* piece) {
    if (piece != nullptr) {
        _piece->setPiece(piece->getPreviewLine1(), piece->getPreviewLine2(),
                         piece->getColor());
    } else {
        _piece->clearPiece();
    }
}

void PieceDisplay::setPosition(int x, int y) { _panel.setPosition(x, y); }
void PieceDisplay::invalidate() { _panel.invalidate(); }
void PieceDisplay::render() { _panel.render(); }
