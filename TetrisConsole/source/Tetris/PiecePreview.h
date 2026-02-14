#pragma once

#include <string>

#include "Panel.h"

class PiecePreview : public PanelElement {
public:
    [[nodiscard]] int height() const override { return 2; }
    void drawRow(int rowIndex, int x, int y, int interiorWidth) const override;

    void setPiece(const std::string& line1, const std::string& line2, int color);
    void clearPiece();

private:
    std::string _line1;
    std::string _line2;
    int _color = 15;
    bool _hasPiece = false;
};
