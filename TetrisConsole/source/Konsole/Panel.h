#pragma once

#include <string>
#include <vector>
#include <cstddef>

enum class Align { LEFT, CENTER, RIGHT };

struct Cell {
    std::string text;
    Align align = Align::LEFT;
    int color = 15; // rlutil::WHITE
    int width = 0;  // 0 = auto (take remaining space equally)

    Cell() = default;
    Cell(std::string t, Align a = Align::LEFT, int c = 15, int w = 0)
        : text(std::move(t)), align(a), color(c), width(w) {}
};

class Panel {
public:
    explicit Panel(int interiorWidth = 0);

    // Full-width row (single cell spanning entire interior)
    size_t addRow(const std::string& text, Align align = Align::LEFT, int color = 15);
    // Multi-column row
    size_t addRow(std::vector<Cell> cells);
    // Horizontal separator
    void addSeparator();

    // Draw full panel at screen position (1-based)
    void draw(int x, int y) const;
    // Redraw a single row
    void drawRow(int x, int y, size_t row) const;
    // Clear the panel area with spaces
    void clear(int x, int y) const;

    // Dynamic content updates
    void setCell(size_t row, size_t col, const std::string& text);
    void setCellColor(size_t row, size_t col, int color);

    // Dimensions (including borders)
    int width() const;
    int height() const;

private:
    struct RowData {
        enum class Type { TEXT, SEPARATOR };
        Type type;
        std::vector<Cell> cells;
    };

    void ensureWidth();
    std::vector<int> computeColumnWidths(const std::vector<Cell>& cells) const;
    std::string renderTextRow(const RowData& row) const;
    std::string renderSeparator(size_t rowIndex) const;
    void drawColoredRow(int x, int y, const RowData& row) const;

    // Find column boundaries (interior positions where column separators go)
    std::vector<int> columnBoundaries(const std::vector<Cell>& cells) const;

    std::vector<RowData> _rows;
    int _interiorWidth;
    mutable bool _widthComputed;
};
