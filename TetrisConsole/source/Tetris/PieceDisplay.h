#pragma once

#include <string>
#include <memory>

#include "Panel.h"

class Tetrimino;
class PiecePreview;

class PieceDisplay {
public:
	explicit PieceDisplay(const std::string& title);
	~PieceDisplay();

	void update(const Tetrimino* piece);
	void setPosition(int x, int y);
	void invalidate();
	void render();

private:
	Panel _panel;
	std::shared_ptr<PiecePreview> _piece;
};
