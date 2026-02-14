#include "Panel.h"

#include <iostream>
#include <algorithm>
#include <set>

#include "rlutil.h"

using namespace std;

Panel::Panel(int interiorWidth)
    : _interiorWidth(interiorWidth), _widthComputed(interiorWidth > 0) {}

size_t Panel::addRow(const string& text, Align align, int color) {
    RowData row;
    row.type = RowData::Type::TEXT;
    row.cells.push_back(Cell(text, align, color, 0));
    _rows.push_back(std::move(row));
    _widthComputed = (_interiorWidth > 0);
    return _rows.size() - 1;
}

size_t Panel::addRow(vector<Cell> cells) {
    RowData row;
    row.type = RowData::Type::TEXT;
    row.cells = std::move(cells);
    _rows.push_back(std::move(row));
    _widthComputed = (_interiorWidth > 0);
    return _rows.size() - 1;
}

void Panel::addSeparator() {
    RowData row;
    row.type = RowData::Type::SEPARATOR;
    _rows.push_back(std::move(row));
}

void Panel::ensureWidth() {
    if (_widthComputed)
        return;

    int maxW = 0;
    for (const auto& row : _rows) {
        if (row.type == RowData::Type::SEPARATOR)
            continue;

        if (row.cells.size() == 1) {
            int w = static_cast<int>(row.cells[0].text.length()) + 2;
            if (w > maxW)
                maxW = w;
        } else {
            int w = 0;
            for (const auto& cell : row.cells) {
                int cellMin = max(cell.width, static_cast<int>(cell.text.length()) + 2);
                w += cellMin;
            }
            w += static_cast<int>(row.cells.size()) - 1; // column separators
            if (w > maxW)
                maxW = w;
        }
    }

    _interiorWidth = maxW;
    _widthComputed = true;
}

vector<int> Panel::computeColumnWidths(const vector<Cell>& cells) const {
    int n = static_cast<int>(cells.size());
    if (n == 0)
        return {};

    if (n == 1)
        return {_interiorWidth};

    int available = _interiorWidth - (n - 1); // subtract column separators
    int fixedSum = 0;
    int autoCount = 0;

    for (const auto& cell : cells) {
        if (cell.width > 0) {
            fixedSum += cell.width;
        } else {
            autoCount++;
        }
    }

    int remaining = available - fixedSum;
    int autoWidth = autoCount > 0 ? remaining / autoCount : 0;
    int autoExtra = autoCount > 0 ? remaining % autoCount : 0;

    vector<int> widths;
    widths.reserve(static_cast<size_t>(n));
    for (const auto& cell : cells) {
        if (cell.width > 0) {
            widths.push_back(cell.width);
        } else {
            int w = autoWidth;
            if (autoExtra > 0) {
                w++;
                autoExtra--;
            }
            widths.push_back(w);
        }
    }

    return widths;
}

vector<int> Panel::columnBoundaries(const vector<Cell>& cells) const {
    vector<int> boundaries;
    if (cells.size() <= 1)
        return boundaries;

    vector<int> widths = computeColumnWidths(cells);
    int pos = 0;
    for (size_t i = 0; i < widths.size() - 1; i++) {
        pos += widths[i];
        boundaries.push_back(pos + static_cast<int>(i)); // account for separators before this one
    }

    return boundaries;
}

string Panel::renderTextRow(const RowData& row) const {
    vector<int> widths = computeColumnWidths(row.cells);
    string result = "║";

    for (size_t i = 0; i < row.cells.size(); i++) {
        if (i > 0)
            result += "║";

        const Cell& cell = row.cells[i];
        int w = widths[i];
        int textLen = static_cast<int>(cell.text.length());
        int padding = w - textLen;

        if (padding < 0) {
            result += cell.text.substr(0, static_cast<size_t>(w));
        } else {
            switch (cell.align) {
                case Align::LEFT: {
                    result += " ";
                    result += cell.text;
                    for (int p = 0; p < padding - 1; p++)
                        result += " ";
                    break;
                }
                case Align::CENTER: {
                    int left = padding / 2;
                    int right = padding - left;
                    for (int p = 0; p < left; p++)
                        result += " ";
                    result += cell.text;
                    for (int p = 0; p < right; p++)
                        result += " ";
                    break;
                }
                case Align::RIGHT: {
                    for (int p = 0; p < padding - 1; p++)
                        result += " ";
                    result += cell.text;
                    result += " ";
                    break;
                }
            }
        }
    }

    result += "║";
    return result;
}

string Panel::renderSeparator(size_t rowIndex) const {
    // Collect column boundaries from neighboring TEXT rows
    set<int> aboveBounds, belowBounds;

    // Search upward for nearest TEXT row
    for (size_t i = rowIndex; i > 0; ) {
        i--;
        if (_rows[i].type == RowData::Type::TEXT) {
            for (int b : columnBoundaries(_rows[i].cells))
                aboveBounds.insert(b);
            break;
        }
    }

    // Search downward for nearest TEXT row
    for (size_t i = rowIndex + 1; i < _rows.size(); i++) {
        if (_rows[i].type == RowData::Type::TEXT) {
            for (int b : columnBoundaries(_rows[i].cells))
                belowBounds.insert(b);
            break;
        }
    }

    string result = "╠";
    for (int pos = 0; pos < _interiorWidth; pos++) {
        bool above = aboveBounds.count(pos) > 0;
        bool below = belowBounds.count(pos) > 0;

        if (above && below)
            result += "╬";
        else if (above)
            result += "╩";
        else if (below)
            result += "╦";
        else
            result += "═";
    }
    result += "╣";
    return result;
}

void Panel::drawColoredRow(int x, int y, const RowData& row) const {
    // Check if all cells have the same color (common case)
    bool uniformColor = true;
    int firstColor = row.cells.empty() ? 15 : row.cells[0].color;
    for (const auto& cell : row.cells) {
        if (cell.color != firstColor) {
            uniformColor = false;
            break;
        }
    }

    if (uniformColor) {
        rlutil::locate(x, y);
        rlutil::setColor(firstColor);
        cout << renderTextRow(row);
        return;
    }

    // Complex case: different colors per cell
    vector<int> widths = computeColumnWidths(row.cells);
    rlutil::locate(x, y);
    rlutil::setColor(15); // WHITE for border
    cout << "║";

    for (size_t i = 0; i < row.cells.size(); i++) {
        if (i > 0) {
            rlutil::setColor(15);
            cout << "║";
        }

        const Cell& cell = row.cells[i];
        int w = widths[i];
        int textLen = static_cast<int>(cell.text.length());
        int padding = w - textLen;

        rlutil::setColor(cell.color);

        if (padding < 0) {
            cout << cell.text.substr(0, static_cast<size_t>(w));
        } else {
            switch (cell.align) {
                case Align::LEFT: {
                    cout << " " << cell.text;
                    for (int p = 0; p < padding - 1; p++)
                        cout << " ";
                    break;
                }
                case Align::CENTER: {
                    int left = padding / 2;
                    int right = padding - left;
                    for (int p = 0; p < left; p++)
                        cout << " ";
                    cout << cell.text;
                    for (int p = 0; p < right; p++)
                        cout << " ";
                    break;
                }
                case Align::RIGHT: {
                    for (int p = 0; p < padding - 1; p++)
                        cout << " ";
                    cout << cell.text;
                    cout << " ";
                    break;
                }
            }
        }
    }

    rlutil::setColor(15);
    cout << "║";
}

void Panel::draw(int x, int y) const {
    const_cast<Panel*>(this)->ensureWidth();

    // Top border
    rlutil::locate(x, y);
    rlutil::setColor(rlutil::WHITE);
    {
        string top = "╔";
        // Check for column boundaries in first TEXT row
        set<int> firstBounds;
        for (const auto& row : _rows) {
            if (row.type == RowData::Type::TEXT) {
                for (int b : columnBoundaries(row.cells))
                    firstBounds.insert(b);
                break;
            }
        }

        for (int pos = 0; pos < _interiorWidth; pos++) {
            if (firstBounds.count(pos) > 0)
                top += "╦";
            else
                top += "═";
        }
        top += "╗";
        cout << top;
    }

    // Rows
    for (size_t i = 0; i < _rows.size(); i++) {
        if (_rows[i].type == RowData::Type::SEPARATOR) {
            rlutil::locate(x, y + static_cast<int>(i) + 1);
            rlutil::setColor(rlutil::WHITE);
            cout << renderSeparator(i);
        } else {
            drawColoredRow(x, y + static_cast<int>(i) + 1, _rows[i]);
        }
    }

    // Bottom border
    rlutil::locate(x, y + static_cast<int>(_rows.size()) + 1);
    rlutil::setColor(rlutil::WHITE);
    {
        string bottom = "╚";
        set<int> lastBounds;
        for (auto it = _rows.rbegin(); it != _rows.rend(); ++it) {
            if (it->type == RowData::Type::TEXT) {
                for (int b : columnBoundaries(it->cells))
                    lastBounds.insert(b);
                break;
            }
        }

        for (int pos = 0; pos < _interiorWidth; pos++) {
            if (lastBounds.count(pos) > 0)
                bottom += "╩";
            else
                bottom += "═";
        }
        bottom += "╝";
        cout << bottom;
    }
}

void Panel::drawRow(int x, int y, size_t row) const {
    if (row >= _rows.size())
        return;

    if (_rows[row].type == RowData::Type::SEPARATOR) {
        rlutil::locate(x, y + static_cast<int>(row) + 1);
        rlutil::setColor(rlutil::WHITE);
        cout << renderSeparator(row);
    } else {
        drawColoredRow(x, y + static_cast<int>(row) + 1, _rows[row]);
    }
}

void Panel::clear(int x, int y) const {
    int w = width();
    int h = height();
    string blank(static_cast<size_t>(w), ' ');
    rlutil::setColor(rlutil::WHITE);
    for (int i = 0; i < h; i++) {
        rlutil::locate(x, y + i);
        cout << blank;
    }
}

void Panel::setCell(size_t row, size_t col, const string& text) {
    if (row < _rows.size() && _rows[row].type == RowData::Type::TEXT && col < _rows[row].cells.size()) {
        _rows[row].cells[col].text = text;
    }
}

void Panel::setCellColor(size_t row, size_t col, int color) {
    if (row < _rows.size() && _rows[row].type == RowData::Type::TEXT && col < _rows[row].cells.size()) {
        _rows[row].cells[col].color = color;
    }
}

int Panel::width() const {
    const_cast<Panel*>(this)->ensureWidth();
    return _interiorWidth + 2;
}

int Panel::height() const {
    return static_cast<int>(_rows.size()) + 2;
}
