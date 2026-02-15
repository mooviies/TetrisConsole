#include "PiecePreview.h"

#include "Color.h"

using namespace std;

void PiecePreview::drawRow(int rowIndex, RowDrawContext& ctx) const {
    if (_hasPiece) {
        ctx.setColor(_color);
        ctx.print(rowIndex == 0 ? _line1 : _line2);
    } else {
        ctx.print(string(static_cast<size_t>(ctx.width()), ' '));
    }
}

void PiecePreview::setPiece(const PiecePreview* piecePreview) {
    if (piecePreview == nullptr) {
        clearPiece();
        return;
    }

    _line1 = piecePreview->_line1;
    _line2 = piecePreview->_line2;
    _color = piecePreview->_color;
    _hasPiece = true;
    markDirty();
}

void PiecePreview::setPiece(const Tetrimino* piece) {
    if (piece == nullptr) {
        clearPiece();
        return;
    }

    _line1 = piece->getPreviewLine1();
    _line2 = piece->getPreviewLine2();
    _color = piece->getColor();
    _hasPiece = true;
    markDirty();
}

void PiecePreview::clearPiece() {
    _hasPiece = false;
    markDirty();
}
