#pragma once

#include <cstddef>
#include <cstdint>

#include "Panel.h"

class HighScoreDisplay {
public:
	HighScoreDisplay();

	void update(int64_t highscore);
	void setPosition(int x, int y);
	void invalidate();
	void render();

private:
	Panel _panel;
	size_t _valueRow;
};
