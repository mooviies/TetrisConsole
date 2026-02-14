#include "PiecePreview.h"

#include <iostream>

#include "rlutil.h"

using namespace std;

void PiecePreview::drawRow(int rowIndex, int x, int y, int interiorWidth) const {
    rlutil::locate(x, y);
    rlutil::setColor(rlutil::WHITE);
    cout << "║";

    if (_hasPiece) {
        rlutil::setColor(_color);
        cout << (rowIndex == 0 ? _line1 : _line2);
    } else {
        cout << string(static_cast<size_t>(interiorWidth), ' ');
    }

    rlutil::setColor(rlutil::WHITE);
    cout << "║";
}

void PiecePreview::setPiece(const string& line1, const string& line2, int color) {
    _line1 = line1;
    _line2 = line2;
    _color = color;
    _hasPiece = true;
}

void PiecePreview::clearPiece() {
    _hasPiece = false;
}
