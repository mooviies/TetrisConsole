#include "HelpDisplay.h"

#include <iostream>

#include "Input.h"
#include "Menu.h"
#include "Platform.h"
#include "rlutil.h"

using namespace std;

static constexpr int kLeftInterior  = 28;
static constexpr int kRightInterior = 22;
static constexpr int kLabelWidth    = 14;
static constexpr int kKeyWidth      = kLeftInterior - kLabelWidth;
static constexpr int kWindowWidth   = 80;
static constexpr int kWindowHeight  = 28;
static constexpr int kTitleHeight   = 5;
static constexpr int kGap           = 1;

HelpDisplay::HelpDisplay()
    : _leftPanel(kLeftInterior), _rightPanel(kRightInterior)
{
	// --- Left panel: Controls ---
	_leftPanel.addRow("CONTROLS", Align::CENTER);
	_leftPanel.addSeparator();
	_controlRows[0] = _leftPanel.addRow({Cell("Move Left",   Align::LEFT, 15, kLabelWidth), Cell("", Align::CENTER)});
	_controlRows[1] = _leftPanel.addRow({Cell("Move Right",  Align::LEFT, 15, kLabelWidth), Cell("", Align::CENTER)});
	_controlRows[2] = _leftPanel.addRow({Cell("Soft Drop",   Align::LEFT, 15, kLabelWidth), Cell("", Align::CENTER)});
	_controlRows[3] = _leftPanel.addRow({Cell("Hard Drop",   Align::LEFT, 15, kLabelWidth), Cell("", Align::CENTER)});
	_controlRows[4] = _leftPanel.addRow({Cell("Rotate CW",   Align::LEFT, 15, kLabelWidth), Cell("", Align::CENTER)});
	_controlRows[5] = _leftPanel.addRow({Cell("Rotate CCW",  Align::LEFT, 15, kLabelWidth), Cell("", Align::CENTER)});
	_controlRows[6] = _leftPanel.addRow({Cell("Hold Piece",  Align::LEFT, 15, kLabelWidth), Cell("", Align::CENTER)});
	_controlRows[7] = _leftPanel.addRow({Cell("Pause",       Align::LEFT, 15, kLabelWidth), Cell("", Align::CENTER)});
	_controlRows[8] = _leftPanel.addRow({Cell("Mute",        Align::LEFT, 15, kLabelWidth), Cell("", Align::CENTER)});

	// --- Right panel: About / Credits ---
	_rightPanel.addRow("ABOUT", Align::CENTER);
	_rightPanel.addSeparator();
	_rightPanel.addRow("Console Tetris", Align::CENTER);
	_rightPanel.addRow("with SRS, T-Spins,", Align::CENTER);
	_rightPanel.addRow("combos and more!", Align::CENTER);
	_rightPanel.addSeparator();
	_rightPanel.addRow("CREDITS", Align::CENTER);
	_rightPanel.addSeparator();
	_rightPanel.addRow("mooviies", Align::CENTER);

	reposition();
}

string HelpDisplay::formatKeys(int action, int maxWidth) {
	const auto& keys = Input::getBindings(action);
	string result;
	for (const auto& key : keys) {
		string name = Input::keyName(key);
		if (result.empty()) {
			result = name;
		} else {
			string candidate = result + " / " + name;
			if (static_cast<int>(candidate.size()) > maxWidth)
				break;
			result = candidate;
		}
	}
	return result;
}

void HelpDisplay::refreshBindings() {
	for (int i = 0; i < kControlCount; i++)
		_leftPanel.setCell(_controlRows[static_cast<size_t>(i)], 1, formatKeys(kActions[i], kKeyWidth));
}

void HelpDisplay::reposition() {
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
