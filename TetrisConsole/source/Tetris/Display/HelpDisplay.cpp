#include "HelpDisplay.h"

#include <iostream>

#include "Input.h"
#include "Menu.h"
#include "Platform.h"
#include "rlutil.h"

using namespace std;

static constexpr int kLabelWidth    = 14;
static constexpr int kKeyColWidth   = 6;
static constexpr int kSeparators    = HelpDisplay::kMaxKeyCols; // one between each of the 4 cells
static constexpr int kLeftInterior  = kLabelWidth + HelpDisplay::kMaxKeyCols * kKeyColWidth + kSeparators;
static constexpr int kRightInterior = 22;
static constexpr int kWindowWidth   = 80;
static constexpr int kWindowHeight  = 28;
static constexpr int kTitleHeight   = 5;
static constexpr int kGap           = 1;

HelpDisplay::HelpDisplay()
    : _leftPanel(kLeftInterior), _rightPanel(kRightInterior)
{
	// --- Left panel: Controls ---
	auto makeRow = [](const string& label) {
		vector<Cell> cells;
		cells.emplace_back(label, Align::LEFT, 15, kLabelWidth);
		for (int k = 0; k < kMaxKeyCols; k++)
			cells.emplace_back("", Align::LEFT, 15, kKeyColWidth);
		return cells;
	};

	_leftPanel.addRow("CONTROLS", Align::CENTER);
	_leftPanel.addSeparator();
	_controlRows[0] = _leftPanel.addRow(makeRow("Move Left"));
	_controlRows[1] = _leftPanel.addRow(makeRow("Move Right"));
	_controlRows[2] = _leftPanel.addRow(makeRow("Soft Drop"));
	_controlRows[3] = _leftPanel.addRow(makeRow("Hard Drop"));
	_controlRows[4] = _leftPanel.addRow(makeRow("Rotate CW"));
	_controlRows[5] = _leftPanel.addRow(makeRow("Rotate CCW"));
	_controlRows[6] = _leftPanel.addRow(makeRow("Hold Piece"));
	_controlRows[7] = _leftPanel.addRow(makeRow("Pause"));
	_controlRows[8] = _leftPanel.addRow(makeRow("Mute"));

	// --- Right panel: About / Credits ---
	_rightPanel.addRow("ABOUT", Align::CENTER);
	_rightPanel.addSeparator();
	_rightPanel.addRow("Console Tetris", Align::CENTER);
	_rightPanel.addRow("with official score", Align::CENTER);
	_rightPanel.addRow("system!", Align::CENTER);
	_rightPanel.addSeparator();
	_rightPanel.addRow("by mooviies", Align::CENTER);

	reposition();
}

// Column assignment: 0 = control keys, 1 = letters/digits, 2 = numpad
static int keyColumn(KeyCode key) {
	int k = static_cast<int>(key);
	if ((k >= 'A' && k <= 'Z') || (k >= '0' && k <= '9'))
		return 1;
	if (key >= KeyCode::Numpad0 && key <= KeyCode::NumpadDel)
		return 2;
	return 0;
}

void HelpDisplay::refreshBindings() {
	for (int i = 0; i < kControlCount; i++) {
		const auto& keys = Input::getBindings(kActions[i]);
		size_t row = _controlRows[static_cast<size_t>(i)];

		string cols[kMaxKeyCols];
		vector<KeyCode> overflow;

		// First pass: place each key in its preferred column
		for (auto key : keys) {
			int col = keyColumn(key);
			if (col < kMaxKeyCols && cols[col].empty())
				cols[col] = Input::keyName(key);
			else
				overflow.push_back(key);
		}

		// Second pass: place overflow keys in any empty column
		for (auto key : overflow) {
			for (int c = 0; c < kMaxKeyCols; c++) {
				if (cols[c].empty()) {
					cols[c] = Input::keyName(key);
					break;
				}
			}
		}

		for (int col = 0; col < kMaxKeyCols; col++)
			_leftPanel.setCell(row, static_cast<size_t>(col + 1), cols[col]);
	}
}

void HelpDisplay::reposition() {
	int lw = _leftPanel.width();
	int rw = _rightPanel.width();
	int totalW = lw + kGap + rw;

	int availableTop = kTitleHeight + 1;
	int availableHeight = kWindowHeight - availableTop;

	int lh = _leftPanel.height();
	int rh = _rightPanel.height();

	int lx = Platform::offsetX() + 1 + (kWindowWidth - totalW) / 2;
	int rx = lx + lw + kGap;

	int ly = Platform::offsetY() + availableTop + (availableHeight - lh) / 2;
	int ry = Platform::offsetY() + availableTop + (availableHeight - rh) / 2;

	_leftPanel.setPosition(lx, ly);
	_rightPanel.setPosition(rx, ry);
}

void HelpDisplay::open() {
	refreshBindings();
	reposition();
	_leftPanel.invalidate();
	_rightPanel.invalidate();
	Platform::flushInput();

	while (true) {
		if (!Platform::isTerminalTooSmall()) {
			_leftPanel.render();
			_rightPanel.render();
			cout << flush;
		}

		switch (Platform::getKey()) {
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
