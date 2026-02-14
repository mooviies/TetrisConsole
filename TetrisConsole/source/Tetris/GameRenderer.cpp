#include "GameRenderer.h"

#include <iostream>
#include <memory>

#include "GameState.h"
#include "Constants.h"
#include "Platform.h"
#include "Utility.h"
#include "SoundEngine.h"
#include "PiecePreview.h"
#include "rlutil.h"

using namespace std;

GameRenderer::GameRenderer()
    : _scorePanel(16), _playfieldPanel(20), _nextPanel(12),
      _nextQueuePanel(12), _holdPanel(12), _highScorePanel(16),
      _scoreValueRow(0), _levelRow(0), _linesRow(0), _highScoreValueRow(0),
      _nextPiece(std::make_shared<PiecePreview>()),
      _holdPiece(std::make_shared<PiecePreview>()),
      _nextPieceRow(0), _holdPieceRow(0)
{
    // Score panel
    _scorePanel.addRow("Score", Align::CENTER);
    _scorePanel.addSeparator();
    _scoreValueRow = _scorePanel.addRow("0000000000", Align::CENTER);
    _scorePanel.addSeparator();
    _levelRow = _scorePanel.addRow({Cell("Level", Align::LEFT, 15, 7),
                                    Cell("01", Align::CENTER)});
    _scorePanel.addSeparator();
    _linesRow = _scorePanel.addRow({Cell("Lines", Align::LEFT, 15, 7),
                                    Cell("000001", Align::CENTER)});

    // Playfield panel (20 empty rows)
    for (int i = 0; i < 20; i++)
        _playfieldPanel.addRow("");

    // Next panel
    _nextPanel.addRow("Next", Align::CENTER);
    _nextPanel.addSeparator();
    _nextPieceRow = _nextPanel.addElement(_nextPiece);
    for (int i = 0; i < 2; i++)
        _nextPanel.addRow("");

    // Next queue pannel
    for (int i = 0; i < 12; i++)
        _nextQueuePanel.addRow("");

    // Hold panel
    _holdPanel.addRow("Hold", Align::CENTER);
    _holdPanel.addSeparator();
    _holdPieceRow = _holdPanel.addElement(_holdPiece);
    for (int i = 0; i < 2; i++)
        _holdPanel.addRow("");

    // High score panel
    _highScorePanel.addRow("High Score", Align::CENTER);
    _highScorePanel.addSeparator();
    _highScoreValueRow = _highScorePanel.addRow("0000000000", Align::CENTER);
}

GameRenderer::~GameRenderer() = default;

void GameRenderer::updatePositions() {
    const int ox = Platform::offsetX();
    const int oy = Platform::offsetY();

    _scorePanel.setPosition(5 + ox, 14 + oy);
    _playfieldPanel.setPosition(30 + ox, 6 + oy);
    _nextPanel.setPosition(59 + ox, 6 + oy);
    _nextQueuePanel.setPosition(59 + ox, 14 + oy);
    _highScorePanel.setPosition(5 + ox, 23 + oy);
    _holdPanel.setPosition(7 + ox, 6 + oy);
}

void GameRenderer::invalidate() {
    updatePositions();
    _scorePanel.invalidate();
    _playfieldPanel.invalidate();
    _nextPanel.invalidate();
    _nextQueuePanel.invalidate();
    _highScorePanel.invalidate();
    _holdPanel.invalidate();
}

void GameRenderer::refresh(GameState& state) {
    // Update the highscore before rendering (moved from printScore to avoid mutation in render)
    if (state._score > state._highscore) {
        state._hasBetterHighscore = true;
    }
    if (state._hasBetterHighscore)
        state._highscore = state._score;

    _scorePanel.render();
    _playfieldPanel.render();
    _nextPanel.render();
    _nextQueuePanel.render();
    _holdPanel.render();
    _highScorePanel.render();

    printMatrix(state);
    printPreview(state);
    printScore(state);

    _scorePanel.render();
    _highScorePanel.render();
    _nextPanel.render();
    _holdPanel.render();
    cout << flush;
}

void GameRenderer::printMatrix(const GameState& state, const bool visible) {
    for (int i = MATRIX_START; i <= MATRIX_END; i++)
        printLine(state, i, visible);
}

void GameRenderer::printPreview(const GameState& state) const {
    const Tetrimino* next = state.peekTetrimino();
    _nextPiece->setPiece(next->getPreviewLine1(), next->getPreviewLine2(),
                         next->getColor());

    if (state._holdTetrimino == nullptr) {
        _holdPiece->clearPiece();
    } else {
        _holdPiece->setPiece(state._holdTetrimino->getPreviewLine1(),
                             state._holdTetrimino->getPreviewLine2(),
                             state._holdTetrimino->getColor());
    }
}

void GameRenderer::printScore(const GameState& state) {
    const int ox = Platform::offsetX();
    const int oy = Platform::offsetY();

    // Update score value and color
    const int scoreColor = state._backToBackBonus ? rlutil::LIGHTGREEN : rlutil::WHITE;
    _scorePanel.setCell(_scoreValueRow, 0, Utility::valueToString(state._score, 10));
    _scorePanel.setCellColor(_scoreValueRow, 0, scoreColor);

    // Update level
    _scorePanel.setCell(_levelRow, 1, Utility::valueToString(state._level, 2));

    // Update lines
    _scorePanel.setCell(_linesRow, 1, Utility::valueToString(state._lines, 6));

    // Update high score
    _highScorePanel.setCell(_highScoreValueRow, 0, Utility::valueToString(state._highscore, 10));

    // Mute indicator
    rlutil::locate(78 + ox, 2 + oy);
    switch (SoundEngine::getMuteState()) {
        case MuteState::UNMUTED:      rlutil::setColor(rlutil::WHITE);  break;
        case MuteState::MUSIC_MUTED:  rlutil::setColor(rlutil::YELLOW); break;
        case MuteState::ALL_MUTED:    rlutil::setColor(rlutil::RED);    break;
    }
    cout << "♪";
    rlutil::setColor(rlutil::WHITE);
}

void GameRenderer::printLine(const GameState& state, const int line, const bool visible) {
    const int ox = Platform::offsetX();
    const int oy = Platform::offsetY();
    int x = 31 + ox;
    const int y = 7 + line - MATRIX_START + oy;

    for (int i = 0; i < TETRIS_WIDTH; i++) {
        rlutil::locate(x, y);
        x += 2;

        bool currentTetriminoHere = false;
        if (state._currentTetrimino != nullptr)
            currentTetriminoHere = state._currentTetrimino->isMino(line, i);
        if (visible && (state._matrix[line][i] || currentTetriminoHere)) {
            if (currentTetriminoHere)
                rlutil::setColor(state._currentTetrimino->getColor());
            else
                rlutil::setColor(state._matrix[line][i]);

            if (currentTetriminoHere)
                cout << "██";
            else {
                rlutil::setColor(rlutil::BLACK);
                rlutil::setBackgroundColor(state._matrix[line][i]);
                cout << "░░";
                rlutil::setBackgroundColor(rlutil::BLACK);
            }

            rlutil::setColor(rlutil::WHITE);
        } else {
            rlutil::setColor(rlutil::DARKGREY);
            if ((line % 2 == 0 && i % 2 != 0) || (line % 2 != 0 && i % 2 == 0))
                cout << "░░";
            else
                cout << "▒▒";
        }
    }
}
