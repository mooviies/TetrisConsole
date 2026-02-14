#pragma once

#include <string>
#include <memory>
#include <vector>

#include "Panel.h"

class Tetrimino;
class PiecePreview;

class PieceDisplay {
public:
	explicit PieceDisplay(size_t size = 1);
	~PieceDisplay();

	void update(const std::vector<const Tetrimino*>& pieces) const;
	void setPosition(int x, int y);
	void invalidate();
	void render();

private:
	size_t _size;
	Panel _panel;
	std::vector<std::shared_ptr<PiecePreview>> _pieces;
};
