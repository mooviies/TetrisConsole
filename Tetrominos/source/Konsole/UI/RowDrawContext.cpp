#include "RowDrawContext.h"

#include <iostream>

#include "rlutil.h"

RowDrawContext::RowDrawContext(const int x, const int y, const int width) : _width(width) {
    rlutil::locate(x, y);
}

// NOLINTBEGIN(readability-convert-member-functions-to-static) - context object pattern
void RowDrawContext::setColor(const int color) {
    rlutil::setColor(color);
}

void RowDrawContext::setBackgroundColor(const int color) {
    rlutil::setBackgroundColor(color);
}

// cppcheck-suppress functionStatic
void RowDrawContext::print(const std::string &text) {
    std::cout << text;
}
// NOLINTEND(readability-convert-member-functions-to-static)
