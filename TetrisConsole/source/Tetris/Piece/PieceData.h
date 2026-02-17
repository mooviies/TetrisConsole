#pragma once

#include <string>
#include <array>

#include "Facing.h"

enum class PieceType { I, J, L, O, S, T, Z };

struct TSpinPositions {
    TSpinPositions() = default;

    TSpinPositions(const Vector2i &a, const Vector2i &b, const Vector2i &c, const Vector2i &d)
        : A(a), B(b), C(c), D(d) {}

    Vector2i A;
    Vector2i B;
    Vector2i C;
    Vector2i D;
};

struct PieceData {
    int color;
    std::string previewLine1;
    std::string previewLine2;
    std::array<Facing, 4> facings;

    bool hasTSpin = false;
    std::array<TSpinPositions, 4> tSpinPositions;

    Vector2i startingPosition = {19, 4};
};

const PieceData &getPieceData(PieceType type);
