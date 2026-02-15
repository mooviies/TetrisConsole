#include "RowDrawContext.h"

#include <iostream>

#include "rlutil.h"

RowDrawContext::RowDrawContext(const int x, const int y, const int width)
    : _width(width)
{
    rlutil::locate(x, y);
}

void RowDrawContext::setColor(const int color) {
    rlutil::setColor(color);
}

void RowDrawContext::setBackgroundColor(const int color) {
    rlutil::setBackgroundColor(color);
}

void RowDrawContext::print(const std::string& text) {
    std::cout << text;
}
