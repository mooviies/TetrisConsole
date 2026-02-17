#include "ScoreDisplay.h"

#include "GameState.h"
#include "Utility.h"
#include "Color.h"

ScoreDisplay::ScoreDisplay() : _panel(18) {
}

void ScoreDisplay::configure(bool showGoal) {
    _showGoal = showGoal;
    _panel.recreate();

    _scoreValueRow = _panel.addRow("0000000000", Align::Center);
    _panel.addSeparator();
    _timeValueRow = _panel.addRow("00:00.00", Align::Center);
    _panel.addSeparator();
    _levelRow = _panel.addRow({Cell("Level", Align::Left, Color::WHITE, 9), Cell("01", Align::Center)});
    _panel.addSeparator();

    _linesRow = _panel.addRow({Cell("Lines", Align::Left, Color::WHITE, 9), Cell("000000", Align::Center)});

    if (showGoal) {
        _goalRow = _panel.addRow({Cell("Goal", Align::Left, Color::WHITE, 9), Cell("000000", Align::Center)});
    }

    _panel.addSeparator();

    _tpmRow = _panel.addRow({Cell("TPM", Align::Left, Color::WHITE, 9), Cell("000000", Align::Center)});

    _lpmRow = _panel.addRow({Cell("LPM", Align::Left, Color::WHITE, 9), Cell("000000", Align::Center)});

    _tetrisRow = _panel.addRow({Cell("Tetris", Align::Left, Color::WHITE, 9), Cell("000000", Align::Center)});

    _combosRow = _panel.addRow({Cell("Combos", Align::Left, Color::WHITE, 9), Cell("000000", Align::Center)});

    _tSpinsRow = _panel.addRow({Cell("T-Spins", Align::Left, Color::WHITE, 9), Cell("000000", Align::Center)});
}

void ScoreDisplay::update(const GameState &state) {
    const int scoreColor = state.stats.backToBackBonus ? Color::LIGHTGREEN : Color::WHITE;
    _panel.setCell(_scoreValueRow, 0, Utility::valueToString(state.stats.score, 10));
    _panel.setCellColor(_scoreValueRow, 0, scoreColor);
    _panel.setCell(_levelRow, 1, Utility::valueToString(state.stats.level, 2));

    if (_showGoal) _panel.setCell(_goalRow, 1, Utility::valueToString(state.stats.goal, 6));

    _panel.setCell(_linesRow, 1, Utility::valueToString(state.stats.lines, 6));
    _panel.setCell(_tetrisRow, 1, Utility::valueToString(state.stats.tetris, 6));
    _panel.setCell(_combosRow, 1, Utility::valueToString(state.stats.combos, 6));
    _panel.setCell(_tSpinsRow, 1, Utility::valueToString(state.stats.tSpins, 6));
}

void ScoreDisplay::updateTimer(const GameState &state) {
    _panel.setCell(_timeValueRow, 0, Utility::timeToString(state.displayTime()));
    _panel.setCell(_tpmRow, 1, Utility::valueToString(state.tpm(), 6));
    _panel.setCell(_lpmRow, 1, Utility::valueToString(state.lpm(), 6));
}

void ScoreDisplay::setPosition(const int x, const int y) {
    _panel.setPosition(x, y);
}
void ScoreDisplay::invalidate() {
    _panel.invalidate();
}
void ScoreDisplay::render() {
    _panel.render();
}
