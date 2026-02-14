#include "ScoreDisplay.h"

#include "GameState.h"
#include "Utility.h"
#include "rlutil.h"

ScoreDisplay::ScoreDisplay()
    : _panel(16), _scoreValueRow(0), _levelRow(0), _linesRow(0)
{
    _panel.addRow("Score", Align::CENTER);
    _panel.addSeparator();
    _scoreValueRow = _panel.addRow("0000000000", Align::CENTER);
    _panel.addSeparator();
    _levelRow = _panel.addRow({Cell("Level", Align::LEFT, 15, 7),
                                Cell("01", Align::CENTER)});
    _panel.addSeparator();
    _linesRow = _panel.addRow({Cell("Lines", Align::LEFT, 15, 7),
                                Cell("000001", Align::CENTER)});
}

void ScoreDisplay::update(const GameState& state) {
    const int scoreColor = state._backToBackBonus ? rlutil::LIGHTGREEN : rlutil::WHITE;
    _panel.setCell(_scoreValueRow, 0, Utility::valueToString(state._score, 10));
    _panel.setCellColor(_scoreValueRow, 0, scoreColor);
    _panel.setCell(_levelRow, 1, Utility::valueToString(state._level, 2));
    _panel.setCell(_linesRow, 1, Utility::valueToString(state._lines, 6));
}

void ScoreDisplay::setPosition(int x, int y) { _panel.setPosition(x, y); }
void ScoreDisplay::invalidate() { _panel.invalidate(); }
void ScoreDisplay::render() { _panel.render(); }
