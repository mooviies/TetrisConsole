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

void PiecePreview::setPiece(const string& line1, const string& line2, int color) {
    _line1 = line1;
    _line2 = line2;
    _color = color;
    _hasPiece = true;
    markDirty();
}

void PiecePreview::clearPiece() {
    _hasPiece = false;
    markDirty();
}
