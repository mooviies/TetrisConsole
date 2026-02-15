#include "RowDrawContext.h"

#include <iostream>

#include "rlutil.h"

RowDrawContext::RowDrawContext(int x, int y, int width)
    : _width(width)
{
    rlutil::locate(x, y);
}

void RowDrawContext::setColor(int color) {
    rlutil::setColor(color);
}

void RowDrawContext::setBackgroundColor(int color) {
    rlutil::setBackgroundColor(color);
}

void RowDrawContext::print(const std::string& text) {
    std::cout << text;
}
