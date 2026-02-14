#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "Panel.h"
#include "GameState.h"

class HighScoreDisplay {
public:
	HighScoreDisplay();

	void open(const std::vector<HighScoreRecord>& highscores);

private:
	void updateDisplay(const std::vector<HighScoreRecord>& hs);
	void reposition();

	Panel _leftPanel;
	Panel _rightPanel;

	// Left panel row indices (10 list entries)
	std::array<size_t, 10> _listRows{};

	// Right panel row indices
	size_t _scoreRow{};
	size_t _timeRow{};
	size_t _nameRow{};
	size_t _lvlStatRow{};
	size_t _tpmStatRow{};
	size_t _lpmStatRow{};
	size_t _linesStatRow{};
	size_t _tetrisStatRow{};
	size_t _combosStatRow{};
	size_t _tSpinsStatRow{};
	size_t _startRow{};
	size_t _modeRow{};
	size_t _ghostRow{};
	size_t _holdRow{};
	size_t _previewRow{};

	int _selected{};
};
