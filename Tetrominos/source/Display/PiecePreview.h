#pragma once

#include <string>

#include "Panel.h"
#include "Tetrimino.h"

class PiecePreview : public PanelElement {
public:
    [[nodiscard]] int height() const override { return 2; }
    void drawRow(int rowIndex, RowDrawContext &ctx) const override;

    void setPiece(const PiecePreview *piecePreview);
    void setPiece(const Tetrimino *piece);
    void clearPiece();

private:
    std::string _line1;
    std::string _line2;
    int _color = 15;
    bool _hasPiece = false;
};
