#pragma once

#include <string>
#include <vector>
#include <memory>
#include <cstddef>

#include "RowDrawContext.h"

enum class Align { LEFT, CENTER, RIGHT, FILL };

struct Cell {
    std::string text;
    Align align = Align::LEFT;
    int color = 15; // rlutil::WHITE
    int width = 0;  // 0 = auto

    Cell() = default;
    Cell(std::string t, const Align a = Align::LEFT, const int c = 15, const int w = 0)
        : text(std::move(t)), align(a), color(c), width(w) {}
};

// Subclass to embed domain-specific content in a Panel without coupling modules.
class PanelElement {
public:
    virtual ~PanelElement() = default;

    [[nodiscard]] virtual int height() const = 0;
    virtual void drawRow(int rowIndex, RowDrawContext& ctx) const = 0;

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

    size_t addRow(const std::string& text, Align align = Align::LEFT, int color = 15);
    size_t addRow(std::vector<Cell> cells);
    void addSeparator();
    size_t addElement(std::shared_ptr<PanelElement> element);

    void setPosition(int x, int y);
    void invalidate();
    void render();
    void clear() const;

    void setCell(size_t row, size_t col, const std::string& text);
    void setCellColor(size_t row, size_t col, int color);

    int width() const;
    int height() const;

private:
    struct RowData {
        enum class Type { TEXT, SEPARATOR, ELEMENT };
        Type type;
        std::vector<Cell> cells;
        std::shared_ptr<PanelElement> element;
        int elementRowIndex = 0;
    };

    void ensureWidth() const;
    std::vector<int> computeColumnWidths(const std::vector<Cell>& cells) const;
    std::string renderTextRow(const RowData& row) const;
    std::string renderSeparator(size_t rowIndex) const;
    void drawColoredRow(int x, int y, const RowData& row) const;
    std::vector<int> columnBoundaries(const std::vector<Cell>& cells) const;
    void drawSingleRow(size_t row) const;
    void drawFull();

    std::vector<RowData> _rows;
    std::vector<bool> _dirtyRows;
    bool _needsFullDraw = true;
    mutable int _interiorWidth;
    mutable bool _widthComputed;
    int _x = 0;
    int _y = 0;
};
