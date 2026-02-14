#pragma once

#include <cstddef>
#include <map>
#include <string>
#include <vector>

#include "Panel.h"
#include "GameState.h"

class HighScoreDisplay {
public:
	HighScoreDisplay(const std::vector<std::string>& levels,
	                 const std::vector<std::string>& lockDownModes);

	void open(const std::map<HighScoreKey, HighScoreRecord>& highscoreMap);

private:
	void updateDisplay(const std::map<HighScoreKey, HighScoreRecord>& hsMap);
	void reposition();

	std::vector<std::string> _levels;
	std::vector<std::string> _lockDownModes;

	Panel _panel;
	size_t _levelRow{};
	size_t _modeRow{};
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

	int _choice{};
	int _levelIdx{};
	int _modeIdx{};
};
