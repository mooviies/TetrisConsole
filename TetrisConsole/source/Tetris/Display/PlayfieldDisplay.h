#pragma once

#include <memory>
#include <array>

#include "Panel.h"
#include "Constants.h"

class GameState;
class PlayfieldElement;

class PlayfieldDisplay {
public:
	PlayfieldDisplay();
	~PlayfieldDisplay();

	void update(const GameState& state, bool visible = true);
	void setPosition(int x, int y);
	void invalidate();
	void render();

private:
	void drawSkylineBorder() const;

	Panel _panel;
	std::shared_ptr<PlayfieldElement> _element;
	int _x = 0;
	int _y = 0;
	std::array<int, TETRIS_WIDTH> _skylineColors{};
};
