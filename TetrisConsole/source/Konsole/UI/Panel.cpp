#include "Panel.h"

#include <iostream>
#include <algorithm>
#include <set>

#include "Color.h"
#include "rlutil.h"

using namespace std;

Panel::Panel(const int interiorWidth) : _interiorWidth(interiorWidth), _widthComputed(interiorWidth > 0) {
}

size_t Panel::addRow(const string &text, const Align align, const int color) {
    RowData row;
    row.type = RowData::Type::TEXT;
    row.cells.emplace_back(text, align, color, 0);
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

size_t Panel::addElement(const shared_ptr<PanelElement> &element) {
    const size_t firstRow = _rows.size();
    const int h = element->height();
    for (int i = 0; i < h; i++) {
        RowData row;
        row.type = RowData::Type::ELEMENT;
        row.element = element;
        row.elementRowIndex = i;
        _rows.push_back(std::move(row));
    }
    return firstRow;
}

void Panel::ensureWidth() const {
    if (_widthComputed) return;

    int maxW = 0;
    for (const auto &row : _rows) {
        if (row.type != RowData::Type::TEXT) continue;

        if (row.cells.size() == 1) {
            const int w = static_cast<int>(row.cells[0].text.length()) + 2;
            if (w > maxW) maxW = w;
        } else {
            int w = 0;
            for (const auto &cell : row.cells) {
                const int cellMin = max(cell.width, static_cast<int>(cell.text.length()) + 2);
                w += cellMin;
            }
            w += static_cast<int>(row.cells.size()) - 1; // column separators
            if (w > maxW) maxW = w;
        }
    }

    _interiorWidth = maxW;
    _widthComputed = true;
}

vector<int> Panel::computeColumnWidths(const vector<Cell> &cells) const {
    const int n = static_cast<int>(cells.size());
    if (n == 0) return {};

    if (n == 1) return {_interiorWidth};

    const int available = _interiorWidth - (n - 1); // subtract column separators
    int fixedSum = 0;
    int autoCount = 0;

    for (const auto &cell : cells) {
        if (cell.width > 0) {
            fixedSum += cell.width;
        } else {
            autoCount++;
        }
    }

    const int remaining = available - fixedSum;
    const int autoWidth = autoCount > 0 ? remaining / autoCount : 0;
    int autoExtra = autoCount > 0 ? remaining % autoCount : 0;

    vector<int> widths;
    widths.reserve(static_cast<size_t>(n));
    for (const auto &cell : cells) {
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

vector<int> Panel::columnBoundaries(const vector<Cell> &cells) const {
    vector<int> boundaries;
    if (cells.size() <= 1) return boundaries;

    const vector<int> widths = computeColumnWidths(cells);
    int pos = 0;
    for (size_t i = 0; i < widths.size() - 1; i++) {
        pos += widths[i];
        boundaries.push_back(pos + static_cast<int>(i)); // account for separators before this one
    }

    return boundaries;
}

string Panel::renderSeparator(const size_t rowIndex) const {
    // Junction characters adapt to column boundaries of neighboring TEXT rows
    set<int> aboveBounds, belowBounds;

    for (size_t i = rowIndex; i > 0;) {
        i--;
        if (_rows[i].type == RowData::Type::TEXT) {
            for (int b : columnBoundaries(_rows[i].cells))
                aboveBounds.insert(b);
            break;
        }
    }

    for (size_t i = rowIndex + 1; i < _rows.size(); i++) {
        // cppcheck-suppress useStlAlgorithm
        if (_rows[i].type == RowData::Type::TEXT) {
            for (int b : columnBoundaries(_rows[i].cells))
                belowBounds.insert(b);
            break;
        }
    }

    string result = "╠";
    for (int pos = 0; pos < _interiorWidth; pos++) {
        const bool above = aboveBounds.count(pos) > 0;
        const bool below = belowBounds.count(pos) > 0;

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

void Panel::drawColoredRow(const int x, const int y, const RowData &row) const {
    const vector<int> widths = computeColumnWidths(row.cells);
    rlutil::locate(x, y);
    rlutil::setBackgroundColor(Color::BLACK);
    rlutil::setColor(rlutil::WHITE);
    cout << "║";

    for (size_t i = 0; i < row.cells.size(); i++) {
        if (i > 0) {
            rlutil::setColor(rlutil::WHITE);
            cout << "║";
        }

        const Cell &cell = row.cells[i];
        const int w = widths[i];
        const int textLen = static_cast<int>(cell.text.length());
        const int padding = w - textLen;

        rlutil::setColor(cell.color);

        if (padding < 0) {
            cout << cell.text.substr(0, static_cast<size_t>(w));
        } else {
            switch (cell.align) {
                case Align::Left: {
                    cout << " " << cell.text;
                    for (int p = 0; p < padding - 1; p++)
                        cout << " ";
                    break;
                }
                case Align::Center: {
                    const int left = padding / 2;
                    const int right = padding - left;
                    for (int p = 0; p < left; p++)
                        cout << " ";
                    cout << cell.text;
                    for (int p = 0; p < right; p++)
                        cout << " ";
                    break;
                }
                case Align::Right: {
                    for (int p = 0; p < padding - 1; p++)
                        cout << " ";
                    cout << cell.text;
                    cout << " ";
                    break;
                }
                case Align::Fill: {
                    cout << cell.text;
                    break;
                }
            }
        }
    }

    rlutil::setColor(rlutil::WHITE);
    cout << "║";
}

void Panel::setPosition(const int x, const int y) {
    _x = x;
    _y = y;
}

void Panel::invalidate() {
    _needsFullDraw = true;
}

void Panel::render() {
    if (_needsFullDraw) {
        drawFull();
        return;
    }

    for (size_t i = 0; i < _rows.size(); i++) {
        if (_dirtyRows.size() > i && _dirtyRows[i]) {
            drawSingleRow(i);
            _dirtyRows[i] = false;
        }
    }

    for (size_t i = 0; i < _rows.size(); i++) {
        if (_rows[i].type == RowData::Type::ELEMENT && _rows[i].element && _rows[i].element->isDirty()) {
            drawSingleRow(i);
            // Only clear after drawing all sub-rows of a multi-row element
            if (_rows[i].elementRowIndex == _rows[i].element->height() - 1) _rows[i].element->clearDirty();
        }
    }
}

void Panel::drawFull() {
    ensureWidth();
    _dirtyRows.assign(_rows.size(), false);
    _needsFullDraw = false;

    // Top border
    rlutil::locate(_x, _y);
    rlutil::setBackgroundColor(Color::BLACK);
    rlutil::setColor(rlutil::WHITE);
    {
        string top = "╔";
        set<int> firstBounds;
        for (const auto &row : _rows) {
            // cppcheck-suppress useStlAlgorithm
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
        drawSingleRow(i);
        if (_rows[i].type == RowData::Type::ELEMENT && _rows[i].element) _rows[i].element->clearDirty();
    }

    // Bottom border
    rlutil::locate(_x, _y + static_cast<int>(_rows.size()) + 1);
    rlutil::setBackgroundColor(Color::BLACK);
    rlutil::setColor(rlutil::WHITE);
    {
        string bottom = "╚";
        set<int> lastBounds;
        for (auto it = _rows.rbegin(); it != _rows.rend(); ++it) {
            // cppcheck-suppress useStlAlgorithm
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

void Panel::drawSingleRow(const size_t row) const {
    if (row >= _rows.size()) return;

    const int rowY = _y + static_cast<int>(row) + 1;
    if (_rows[row].type == RowData::Type::SEPARATOR) {
        rlutil::locate(_x, rowY);
        rlutil::setBackgroundColor(Color::BLACK);
        rlutil::setColor(rlutil::WHITE);
        cout << renderSeparator(row);
    } else if (_rows[row].type == RowData::Type::ELEMENT) {
        rlutil::locate(_x, rowY);
        rlutil::setBackgroundColor(Color::BLACK);
        rlutil::setColor(Color::WHITE);
        cout << "║";
        RowDrawContext ctx(_x + 1, rowY, _interiorWidth);
        _rows[row].element->drawRow(_rows[row].elementRowIndex, ctx);
        rlutil::setColor(Color::WHITE);
        rlutil::setBackgroundColor(Color::BLACK);
        cout << "║";
    } else {
        drawColoredRow(_x, rowY, _rows[row]);
    }
}

void Panel::clear() const {
    const int w = width();
    const int h = height();
    const string blank(static_cast<size_t>(w), ' ');
    rlutil::setBackgroundColor(Color::BLACK);
    rlutil::setColor(rlutil::WHITE);
    for (int i = 0; i < h; i++) {
        rlutil::locate(_x, _y + i);
        cout << blank;
    }
}

void Panel::recreate() {
    _rows.clear();
    _dirtyRows.clear();
    _needsFullDraw = true;
}

void Panel::setCell(const size_t row, const size_t col, const string &text) {
    if (row < _rows.size() && _rows[row].type == RowData::Type::TEXT && col < _rows[row].cells.size()) {
        if (_rows[row].cells[col].text == text) return;
        _rows[row].cells[col].text = text;
        if (row < _dirtyRows.size()) _dirtyRows[row] = true;
    }
}

void Panel::setCellColor(const size_t row, const size_t col, const int color) {
    if (row < _rows.size() && _rows[row].type == RowData::Type::TEXT && col < _rows[row].cells.size()) {
        if (_rows[row].cells[col].color == color) return;
        _rows[row].cells[col].color = color;
        if (row < _dirtyRows.size()) _dirtyRows[row] = true;
    }
}

int Panel::width() const {
    ensureWidth();
    return _interiorWidth + 2;
}

int Panel::height() const {
    return static_cast<int>(_rows.size()) + 2;
}
