#include "HighScoreDisplay.h"

#include <iostream>

#include "Menu.h"
#include "Platform.h"
#include "Utility.h"
#include "rlutil.h"

using namespace std;

static constexpr int kInterior   = 24;
static constexpr int kLabelWidth = 12;
static constexpr int kWindowWidth  = 80;
static constexpr int kWindowHeight = 28;
static constexpr int kTitleHeight  = 5; // top border + title + sep + subtitle + bottom border

HighScoreDisplay::HighScoreDisplay(const vector<string>& levels,
                                   const vector<string>& lockDownModes)
    : _levels(levels), _lockDownModes(lockDownModes), _panel(kInterior)
{
    _panel.addRow("HIGH SCORES", Align::CENTER);
    _panel.addSeparator();
    _levelRow = _panel.addRow("", Align::LEFT);
    _modeRow  = _panel.addRow("", Align::LEFT);
    _panel.addSeparator();
    _scoreRow = _panel.addRow("", Align::CENTER);
    _timeRow  = _panel.addRow("", Align::CENTER);
    _nameRow  = _panel.addRow("", Align::CENTER);
    _panel.addSeparator();
    _lvlStatRow    = _panel.addRow({Cell("Level",   Align::LEFT, 15, kLabelWidth),
                                    Cell("", Align::CENTER)});
    _tpmStatRow    = _panel.addRow({Cell("TPM",     Align::LEFT, 15, kLabelWidth),
                                    Cell("", Align::CENTER)});
    _lpmStatRow    = _panel.addRow({Cell("LPM",     Align::LEFT, 15, kLabelWidth),
                                    Cell("", Align::CENTER)});
    _linesStatRow  = _panel.addRow({Cell("Lines",   Align::LEFT, 15, kLabelWidth),
                                    Cell("", Align::CENTER)});
    _tetrisStatRow = _panel.addRow({Cell("Tetris",  Align::LEFT, 15, kLabelWidth),
                                    Cell("", Align::CENTER)});
    _combosStatRow = _panel.addRow({Cell("Combos",  Align::LEFT, 15, kLabelWidth),
                                    Cell("", Align::CENTER)});
    _tSpinsStatRow = _panel.addRow({Cell("T-Spins", Align::LEFT, 15, kLabelWidth),
                                    Cell("", Align::CENTER)});

    reposition();
}

void HighScoreDisplay::reposition() {
    int w = _panel.width();
    int h = _panel.height();
    int availableTop = kTitleHeight + 1;
    int availableHeight = kWindowHeight - availableTop;
    int px = Platform::offsetX() + (kWindowWidth / 2) - (w / 2);
    int py = Platform::offsetY() + availableTop + (availableHeight - h) / 2;
    _panel.setPosition(px, py);
}

void HighScoreDisplay::updateDisplay(const map<HighScoreKey, HighScoreRecord>& hsMap) {
    string lp = (_choice == 0) ? "> " : "  ";
    string mp = (_choice == 1) ? "> " : "  ";
    _panel.setCell(_levelRow, 0,
                   lp + "Level     : " + _levels[static_cast<size_t>(_levelIdx)]);
    _panel.setCell(_modeRow, 0,
                   mp + "Lock Down : " + _lockDownModes[static_cast<size_t>(_modeIdx)]);

    int lvl = _levelIdx + 1;
    MODE mode = EXTENDED;
    if (_modeIdx == 1) mode = EXTENDED_INFINITY;
    else if (_modeIdx == 2) mode = CLASSIC;

    HighScoreKey key{lvl, mode};
    auto it = hsMap.find(key);

    if (it != hsMap.end()) {
        const auto& rec = it->second;
        _panel.setCell(_scoreRow, 0, Utility::valueToString(rec.score, 10));
        _panel.setCell(_timeRow, 0, Utility::timeToString(rec.gameElapsed));
        _panel.setCell(_nameRow, 0, rec.name);
        _panel.setCell(_lvlStatRow, 1, Utility::valueToString(rec.level, 2));
        _panel.setCell(_tpmStatRow, 1, Utility::valueToString(rec.tpm, 6));
        _panel.setCell(_lpmStatRow, 1, Utility::valueToString(rec.lpm, 6));
        _panel.setCell(_linesStatRow, 1, Utility::valueToString(rec.lines, 6));
        _panel.setCell(_tetrisStatRow, 1, Utility::valueToString(rec.tetris, 6));
        _panel.setCell(_combosStatRow, 1, Utility::valueToString(rec.combos, 6));
        _panel.setCell(_tSpinsStatRow, 1, Utility::valueToString(rec.tSpins, 6));
    } else {
        _panel.setCell(_scoreRow, 0, "----------");
        _panel.setCell(_timeRow, 0, "--:--.--");
        _panel.setCell(_nameRow, 0, "");
        _panel.setCell(_lvlStatRow, 1, "--");
        _panel.setCell(_tpmStatRow, 1, "------");
        _panel.setCell(_lpmStatRow, 1, "------");
        _panel.setCell(_linesStatRow, 1, "------");
        _panel.setCell(_tetrisStatRow, 1, "------");
        _panel.setCell(_combosStatRow, 1, "------");
        _panel.setCell(_tSpinsStatRow, 1, "------");
    }
}

void HighScoreDisplay::open(const map<HighScoreKey, HighScoreRecord>& highscoreMap) {
    _choice = 0;
    _levelIdx = 0;
    _modeIdx = 0;

    reposition();
    _panel.invalidate();
    updateDisplay(highscoreMap);
    Platform::flushInput();

    while (true) {
        if (!Platform::isTerminalTooSmall()) {
            _panel.render();
            cout << flush;
        }

        switch (Platform::getKey()) {
            case rlutil::KEY_UP:
            case rlutil::KEY_DOWN:
                _choice = (_choice == 0) ? 1 : 0;
                updateDisplay(highscoreMap);
                break;
            case rlutil::KEY_LEFT:
                if (_choice == 0) {
                    _levelIdx--;
                    if (_levelIdx < 0)
                        _levelIdx = static_cast<int>(_levels.size()) - 1;
                } else {
                    _modeIdx--;
                    if (_modeIdx < 0)
                        _modeIdx = static_cast<int>(_lockDownModes.size()) - 1;
                }
                updateDisplay(highscoreMap);
                break;
            case rlutil::KEY_RIGHT:
                if (_choice == 0) {
                    _levelIdx++;
                    if (_levelIdx >= static_cast<int>(_levels.size()))
                        _levelIdx = 0;
                } else {
                    _modeIdx++;
                    if (_modeIdx >= static_cast<int>(_lockDownModes.size()))
                        _modeIdx = 0;
                }
                updateDisplay(highscoreMap);
                break;
            case rlutil::KEY_ESCAPE:
            case rlutil::KEY_ENTER:
                _panel.clear();
                return;
            default:
                break;
        }

        if (Platform::wasResized()) {
            if (!Platform::isTerminalTooSmall()) {
                if (Menu::onResize)
                    Menu::onResize();
                reposition();
                _panel.invalidate();
            }
            continue;
        }

        if (Menu::shouldExitGame && Menu::shouldExitGame())
            break;
    }

    _panel.clear();
}
