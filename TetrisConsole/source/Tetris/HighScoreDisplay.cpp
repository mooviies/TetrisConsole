#include "HighScoreDisplay.h"

#include <algorithm>
#include <iostream>

#include "Menu.h"
#include "Platform.h"
#include "Utility.h"
#include "rlutil.h"

using namespace std;

static constexpr int kLeftInterior  = 28;
static constexpr int kRightInterior = 22;
static constexpr int kLabelWidth    = 9;
static constexpr int kWindowWidth   = 80;
static constexpr int kWindowHeight  = 28;
static constexpr int kTitleHeight   = 5;
static constexpr int kGap           = 1;

HighScoreDisplay::HighScoreDisplay()
    : _leftPanel(kLeftInterior), _rightPanel(kRightInterior)
{
	// --- Left panel: title + 10 list rows ---
	_leftPanel.addRow("HIGH SCORES", Align::CENTER);
	_leftPanel.addSeparator();
	for (int i = 0; i < 10; i++)
		_listRows[static_cast<size_t>(i)] = _leftPanel.addRow("", Align::LEFT);

	// --- Right panel: score/time/name header + stats + options ---
	_scoreRow = _rightPanel.addRow("", Align::CENTER);
	_timeRow  = _rightPanel.addRow("", Align::CENTER);
	_nameRow  = _rightPanel.addRow("", Align::CENTER);
	_rightPanel.addSeparator();
	_lvlStatRow    = _rightPanel.addRow({Cell("Level",   Align::LEFT, 15, kLabelWidth),
	                                     Cell("", Align::CENTER)});
	_tpmStatRow    = _rightPanel.addRow({Cell("TPM",     Align::LEFT, 15, kLabelWidth),
	                                     Cell("", Align::CENTER)});
	_lpmStatRow    = _rightPanel.addRow({Cell("LPM",     Align::LEFT, 15, kLabelWidth),
	                                     Cell("", Align::CENTER)});
	_linesStatRow  = _rightPanel.addRow({Cell("Lines",   Align::LEFT, 15, kLabelWidth),
	                                     Cell("", Align::CENTER)});
	_tetrisStatRow = _rightPanel.addRow({Cell("Tetris",  Align::LEFT, 15, kLabelWidth),
	                                     Cell("", Align::CENTER)});
	_combosStatRow = _rightPanel.addRow({Cell("Combos",  Align::LEFT, 15, kLabelWidth),
	                                     Cell("", Align::CENTER)});
	_tSpinsStatRow = _rightPanel.addRow({Cell("T-Spins", Align::LEFT, 15, kLabelWidth),
	                                     Cell("", Align::CENTER)});
	_rightPanel.addSeparator();
	_startRow   = _rightPanel.addRow({Cell("Start",   Align::LEFT, 15, kLabelWidth),
	                                  Cell("", Align::CENTER)});
	_modeRow    = _rightPanel.addRow({Cell("Mode",    Align::LEFT, 15, kLabelWidth),
	                                  Cell("", Align::CENTER)});
	_ghostRow   = _rightPanel.addRow({Cell("Ghost",   Align::LEFT, 15, kLabelWidth),
	                                  Cell("", Align::CENTER)});
	_holdRow    = _rightPanel.addRow({Cell("Hold",    Align::LEFT, 15, kLabelWidth),
	                                  Cell("", Align::CENTER)});
	_previewRow = _rightPanel.addRow({Cell("Preview", Align::LEFT, 15, kLabelWidth),
	                                  Cell("", Align::CENTER)});

	reposition();
}

void HighScoreDisplay::reposition() {
	int lw = _leftPanel.width();
	int rw = _rightPanel.width();
	int totalW = lw + kGap + rw;

	int availableTop = kTitleHeight + 1;
	int availableHeight = kWindowHeight - availableTop;

	int lh = _leftPanel.height();
	int rh = _rightPanel.height();

	int cx = Platform::offsetX() + (kWindowWidth / 2);
	int lx = cx - (totalW / 2);
	int rx = lx + lw + kGap;

	int ly = Platform::offsetY() + availableTop + (availableHeight - lh) / 2;
	int ry = Platform::offsetY() + availableTop + (availableHeight - rh) / 2;

	_leftPanel.setPosition(lx, ly);
	_rightPanel.setPosition(rx, ry);
}

void HighScoreDisplay::updateDisplay(const vector<HighScoreRecord>& hs) {
	// --- Left panel: list entries ---
	for (int i = 0; i < 10; i++) {
		string prefix = (_selected == i) ? "> " : "  ";
		string rank = Utility::valueToString(i + 1, 2) + ". ";

		if (i < static_cast<int>(hs.size())) {
			const auto& rec = hs[static_cast<size_t>(i)];
			// Pad or truncate name to 10 chars
			string name = rec.name;
			if (name.empty()) name = "----------";
			if (name.size() > 10) name.resize(10);
			while (name.size() < 10) name += ' ';
			string score = Utility::valueToString(rec.score, 10);
			_leftPanel.setCell(_listRows[static_cast<size_t>(i)], 0,
			                   prefix + rank + name + " " + score);
		} else {
			_leftPanel.setCell(_listRows[static_cast<size_t>(i)], 0,
			                   prefix + rank + "----------" + " " + "----------");
		}
	}

	// --- Right panel: details for selected entry ---
	if (_selected < static_cast<int>(hs.size())) {
		const auto& rec = hs[static_cast<size_t>(_selected)];
		_rightPanel.setCell(_scoreRow, 0, Utility::valueToString(rec.score, 10));
		_rightPanel.setCell(_timeRow,  0, Utility::timeToString(rec.gameElapsed));
		_rightPanel.setCell(_nameRow,  0, rec.name.empty() ? "----------" : rec.name);

		_rightPanel.setCell(_lvlStatRow,    1, Utility::valueToString(rec.level, 6));
		_rightPanel.setCell(_tpmStatRow,    1, Utility::valueToString(rec.tpm, 6));
		_rightPanel.setCell(_lpmStatRow,    1, Utility::valueToString(rec.lpm, 6));
		_rightPanel.setCell(_linesStatRow,  1, Utility::valueToString(rec.lines, 6));
		_rightPanel.setCell(_tetrisStatRow, 1, Utility::valueToString(rec.tetris, 6));
		_rightPanel.setCell(_combosStatRow, 1, Utility::valueToString(rec.combos, 6));
		_rightPanel.setCell(_tSpinsStatRow, 1, Utility::valueToString(rec.tSpins, 6));

		_rightPanel.setCell(_startRow, 1, Utility::valueToString(rec.startingLevel, 2));
		string modeStr = "Extended";
		if (rec.mode == MODE::CLASSIC) modeStr = "Classic";
		else if (rec.mode == MODE::EXTENDED_INFINITY) modeStr = "Infinite";
		_rightPanel.setCell(_modeRow,    1, modeStr);
		_rightPanel.setCell(_ghostRow,   1, rec.ghostEnabled ? "On" : "Off");
		_rightPanel.setCell(_holdRow,    1, rec.holdEnabled  ? "On" : "Off");
		_rightPanel.setCell(_previewRow, 1, Utility::valueToString(rec.previewCount, 2));
	} else {
		_rightPanel.setCell(_scoreRow, 0, "----------");
		_rightPanel.setCell(_timeRow,  0, "--:--.--");
		_rightPanel.setCell(_nameRow,  0, "");

		_rightPanel.setCell(_lvlStatRow,    1, "------");
		_rightPanel.setCell(_tpmStatRow,    1, "------");
		_rightPanel.setCell(_lpmStatRow,    1, "------");
		_rightPanel.setCell(_linesStatRow,  1, "------");
		_rightPanel.setCell(_tetrisStatRow, 1, "------");
		_rightPanel.setCell(_combosStatRow, 1, "------");
		_rightPanel.setCell(_tSpinsStatRow, 1, "------");

		_rightPanel.setCell(_startRow,   1, "--");
		_rightPanel.setCell(_modeRow,    1, "------");
		_rightPanel.setCell(_ghostRow,   1, "---");
		_rightPanel.setCell(_holdRow,    1, "---");
		_rightPanel.setCell(_previewRow, 1, "--");
	}
}

void HighScoreDisplay::open(const vector<HighScoreRecord>& highscores) {
	_selected = 0;

	reposition();
	_leftPanel.invalidate();
	_rightPanel.invalidate();
	updateDisplay(highscores);
	Platform::flushInput();

	while (true) {
		if (!Platform::isTerminalTooSmall()) {
			_leftPanel.render();
			_rightPanel.render();
			cout << flush;
		}

		switch (Platform::getKey()) {
			case rlutil::KEY_UP:
				if (_selected > 0) {
					_selected--;
					updateDisplay(highscores);
				}
				break;
			case rlutil::KEY_DOWN:
				if (_selected < 9) {
					_selected++;
					updateDisplay(highscores);
				}
				break;
			case rlutil::KEY_ESCAPE:
			case rlutil::KEY_ENTER:
				_leftPanel.clear();
				_rightPanel.clear();
				return;
			default:
				break;
		}

		if (Platform::wasResized()) {
			if (!Platform::isTerminalTooSmall()) {
				if (Menu::onResize)
					Menu::onResize();
				reposition();
				_leftPanel.invalidate();
				_rightPanel.invalidate();
			}
			continue;
		}

		if (Menu::shouldExitGame && Menu::shouldExitGame())
			break;
	}

	_leftPanel.clear();
	_rightPanel.clear();
}

string HighScoreDisplay::openForNewEntry(const vector<HighScoreRecord>& highscores,
                                         const HighScoreRecord& newRecord) {
	constexpr int kMaxName = 10;

	// Build merged list: insert new record at correct sorted position
	auto merged = highscores;
	auto it = lower_bound(merged.begin(), merged.end(), newRecord,
	                       [](const HighScoreRecord& a, const HighScoreRecord& b) {
		                       return a.score > b.score;
	                       });
	int rank = static_cast<int>(it - merged.begin());
	merged.insert(it, newRecord);
	if (merged.size() > 10)
		merged.resize(10);

	_selected = rank;
	reposition();
	_leftPanel.invalidate();
	_rightPanel.invalidate();
	updateDisplay(merged);

	// Highlight the new entry row in yellow
	auto rankIdx = static_cast<size_t>(rank);
	_leftPanel.setCellColor(_listRows[rankIdx], 0, rlutil::YELLOW);

	Platform::flushInput();

	string name;

	while (true) {
		// Update name display in both panels
		string display = name;
		for (int i = static_cast<int>(name.size()); i < kMaxName; i++)
			display += '_';

		string prefix = "> ";
		string rankStr = Utility::valueToString(rank + 1, 2) + ". ";
		string scoreStr = Utility::valueToString(newRecord.score, 10);
		_leftPanel.setCell(_listRows[rankIdx], 0,
		                   prefix + rankStr + display + " " + scoreStr);
		_rightPanel.setCell(_nameRow, 0, name.empty() ? "__________" : name);

		if (!Platform::isTerminalTooSmall()) {
			_leftPanel.render();
			_rightPanel.render();
			cout << flush;
		}

		int key = Platform::getKey();

		if (key == rlutil::KEY_ENTER) {
			break;
		} else if (key == rlutil::KEY_ESCAPE) {
			name.clear();
			break;
		} else if (key == 8 || key == 127) {
			if (!name.empty())
				name.pop_back();
		} else if (key >= 32 && key <= 126 && static_cast<int>(name.size()) < kMaxName) {
			name += static_cast<char>(key);
		}

		if (Platform::wasResized()) {
			if (!Platform::isTerminalTooSmall()) {
				if (Menu::onResize)
					Menu::onResize();
				reposition();
				_leftPanel.invalidate();
				_rightPanel.invalidate();
			}
			continue;
		}

		if (Menu::shouldExitGame && Menu::shouldExitGame())
			break;
	}

	_leftPanel.clear();
	_rightPanel.clear();
	return name;
}
