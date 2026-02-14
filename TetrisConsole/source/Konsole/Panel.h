#pragma once

#include <string>
#include <vector>
#include <memory>
#include <cstddef>

enum class Align { LEFT, CENTER, RIGHT, FILL };

struct Cell {
    std::string text;
    Align align = Align::LEFT;
    int color = 15; // rlutil::WHITE
    int width = 0;  // 0 = auto (take remaining space equally)

    Cell() = default;
    Cell(std::string t, Align a = Align::LEFT, int c = 15, int w = 0)
        : text(std::move(t)), align(a), color(c), width(w) {}
};

// Base class for custom elements that can be embedded in a Panel.
// Subclass this outside the Konsole module to add domain-specific content.
class PanelElement {
public:
    virtual ~PanelElement() = default;

    // Number of rows this element occupies
    [[nodiscard]] virtual int height() const = 0;

    // Draw one row of this element.
    // rowIndex: 0..height()-1
    // x, y: screen position for this row (left border column, row line)
    // interiorWidth: number of columns between the ║ borders
    virtual void drawRow(int rowIndex, int x, int y, int interiorWidth) const = 0;

    // Dirty tracking — subclasses should call markDirty() when their data changes
    [[nodiscard]] bool isDirty() const { return _dirty; }
    void clearDirty() { _dirty = false; }

protected:
    void markDirty() { _dirty = true; }

private:
    bool _dirty = false;
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
    // Custom element (occupies element->height() rows); returns index of first row
    size_t addElement(std::shared_ptr<PanelElement> element);

    // Set screen position (1-based); can be called again on resize
    void setPosition(int x, int y);
    // Mark entire panel for full redraw on next render()
    void invalidate();
    // Smart render: full draw if invalidated, partial (dirty rows only) otherwise
    void render();
    // Clear the panel area
    void clear() const;

    // Dynamic content updates
    void setCell(size_t row, size_t col, const std::string& text);
    void setCellColor(size_t row, size_t col, int color);

    // Dimensions (including borders)
    int width() const;
    int height() const;

private:
    struct RowData {
        enum class Type { TEXT, SEPARATOR, ELEMENT };
        Type type;
        std::vector<Cell> cells;                  // TEXT rows
        std::shared_ptr<PanelElement> element;     // ELEMENT rows
        int elementRowIndex = 0;                   // sub-row within the element
    };

    void ensureWidth();
    std::vector<int> computeColumnWidths(const std::vector<Cell>& cells) const;
    std::string renderTextRow(const RowData& row) const;
    std::string renderSeparator(size_t rowIndex) const;
    void drawColoredRow(int x, int y, const RowData& row) const;

    // Find column boundaries (interior positions where column separators go)
    std::vector<int> columnBoundaries(const std::vector<Cell>& cells) const;

    void drawSingleRow(size_t row) const;

    void drawFull();

    std::vector<RowData> _rows;
    std::vector<bool> _dirtyRows;
    bool _needsFullDraw = true;
    int _interiorWidth;
    mutable bool _widthComputed;
    int _x = 0;
    int _y = 0;
};
