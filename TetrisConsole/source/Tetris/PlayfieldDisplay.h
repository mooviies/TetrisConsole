#pragma once

#include <memory>

#include "Panel.h"

class GameState;
class PlayfieldElement;

class PlayfieldDisplay {
public:
	PlayfieldDisplay();
	~PlayfieldDisplay();

	void update(const GameState& state, bool visible = true) const;
	void setPosition(int x, int y);
	void invalidate();
	void render();

private:
	Panel _panel;
	std::shared_ptr<PlayfieldElement> _element;
};
