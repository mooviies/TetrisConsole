#pragma once

#include <string>

class RowDrawContext {
public:
    RowDrawContext(int x, int y, int width);

    void setColor(int color);
    void setBackgroundColor(int color);
    void print(const std::string& text);
    [[nodiscard]] int width() const { return _width; }

private:
    int _width;
};
