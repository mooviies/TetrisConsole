#include "Icon.h"

#include <iostream>

#include "rlutil.h"

Icon::Icon(std::string symbol, int color)
    : _symbol(std::move(symbol)), _color(color) {}

void Icon::setPosition(int x, int y) {
    _x = x;
    _y = y;
}

void Icon::setColor(int color) {
    _color = color;
}

void Icon::draw() const {
    rlutil::locate(_x, _y);
    rlutil::setColor(_color);
    std::cout << _symbol;
    rlutil::setColor(rlutil::WHITE);
}
