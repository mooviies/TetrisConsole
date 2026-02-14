#include "HighScoreDisplay.h"

#include "Utility.h"

HighScoreDisplay::HighScoreDisplay() : _panel(16), _valueRow(0) {
    _panel.addRow("High Score", Align::CENTER);
    _panel.addSeparator();
    _valueRow = _panel.addRow("0000000000", Align::CENTER);
}

void HighScoreDisplay::update(int64_t highscore) {
    _panel.setCell(_valueRow, 0, Utility::valueToString(highscore, 10));
}

void HighScoreDisplay::setPosition(int x, int y) { _panel.setPosition(x, y); }
void HighScoreDisplay::invalidate() { _panel.invalidate(); }
void HighScoreDisplay::render() { _panel.render(); }
