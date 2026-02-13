#include "GameRenderer.h"

#include <iostream>

#include "GameState.h"
#include "Constants.h"
#include "Platform.h"
#include "Utility.h"
#include "rlutil.h"

using namespace std;

GameRenderer::GameRenderer() = default;
GameRenderer::~GameRenderer() = default;

void GameRenderer::display() {
    int ox = Platform::offsetX();
    int oy = Platform::offsetY();

    rlutil::locate(1 + ox, 6 + oy);
    cout << "    ╔════════════════╗       ╔════════════════════╗       ╔════════════╗";
    rlutil::locate(1 + ox, 7 + oy);
    cout << "    ║     Score      ║       ║                    ║       ║    Next    ║";
    rlutil::locate(1 + ox, 8 + oy);
    cout << "    ╠════════════════╣       ║                    ║       ╠════════════╣";
    rlutil::locate(1 + ox, 9 + oy);
    cout << "    ║   0000000000   ║       ║                    ║       ║            ║";
    rlutil::locate(1 + ox, 10 + oy);
    cout << "    ╠═══════╦════════╣       ║                    ║       ║            ║";
    rlutil::locate(1 + ox, 11 + oy);
    cout << "    ║ Level ║   01   ║       ║                    ║       ║            ║";
    rlutil::locate(1 + ox, 12 + oy);
    cout << "    ╠═══════╬════════╣       ║                    ║       ║            ║";
    rlutil::locate(1 + ox, 13 + oy);
    cout << "    ║ Lines ║ 000001 ║       ║                    ║       ╚════════════╝";
    rlutil::locate(1 + ox, 14 + oy);
    cout << "    ╚═══════╩════════╝       ║                    ║                     ";

    for (int i = 28; i <= MATRIX_END; i++) {
        rlutil::locate(1 + ox, 15 + (i - 28) + oy);
        cout << "                             ║                    ║                     ";
    }
    rlutil::locate(1 + ox, 15 + (MATRIX_END - 28) + 1 + oy);
    cout << "                             ╚════════════════════╝                     ";

    rlutil::locate(5 + ox, 23 + oy);
    cout << "╔════════════════╗";
    rlutil::locate(5 + ox, 24 + oy);
    cout << "║   High Score   ║";
    rlutil::locate(5 + ox, 25 + oy);
    cout << "╠════════════════╣";
    rlutil::locate(5 + ox, 26 + oy);
    cout << "║   0000000000   ║";
    rlutil::locate(5 + ox, 27 + oy);
    cout << "╚════════════════╝";

    rlutil::locate(59 + ox, 20 + oy);
    cout << "╔════════════╗";
    rlutil::locate(59 + ox, 21 + oy);
    cout << "║    Hold    ║";
    rlutil::locate(59 + ox, 22 + oy);
    cout << "╠════════════╣";
    rlutil::locate(59 + ox, 23 + oy);
    cout << "║            ║";
    rlutil::locate(59 + ox, 24 + oy);
    cout << "║            ║";
    rlutil::locate(59 + ox, 25 + oy);
    cout << "║            ║";
    rlutil::locate(59 + ox, 26 + oy);
    cout << "║            ║";
    rlutil::locate(59 + ox, 27 + oy);
    cout << "╚════════════╝";
}

void GameRenderer::refresh(GameState& state) {
    // Update highscore before rendering (moved from printScore to avoid mutation in render)
    if (state._score > state._highscore) {
        state._hasBetterHighscore = true;
    }
    if (state._hasBetterHighscore)
        state._highscore = state._score;

    printMatrix(state);
    printPreview(state);
    printScore(state);
    cout << flush;
}

void GameRenderer::printMatrix(const GameState& state, const bool visible) {
    for (int i = MATRIX_START; i <= MATRIX_END; i++)
        printLine(state, i, visible);
}

void GameRenderer::printPreview(const GameState& state) {
    state.peekTetrimino()->printPreview(0);
    state.peekTetrimino()->printPreview(1);

    if (state._holdTetrimino == nullptr) {
        int ox = Platform::offsetX();
        int oy = Platform::offsetY();
        rlutil::locate(60 + ox, 24 + oy);
        cout << "            ";
        rlutil::locate(60 + ox, 25 + oy);
        cout << "            ";
    } else {
        state._holdTetrimino->printPreview(0, true);
        state._holdTetrimino->printPreview(1, true);
    }
}

void GameRenderer::printScore(const GameState& state) {
    int ox = Platform::offsetX();
    int oy = Platform::offsetY();

    rlutil::locate(9 + ox, 9 + oy);
    if (state._backToBackBonus)
        rlutil::setColor(rlutil::LIGHTGREEN);
    cout << Utility::valueToString(state._score, 10);
    rlutil::setColor(rlutil::WHITE);

    rlutil::locate(17 + ox, 11 + oy);
    cout << Utility::valueToString(state._level, 2);
    rlutil::locate(15 + ox, 13 + oy);
    cout << Utility::valueToString(state._lines, 6);
    rlutil::locate(9 + ox, 26 + oy);
    cout << Utility::valueToString(state._highscore, 10);
}

void GameRenderer::printLine(const GameState& state, const int line, const bool visible) {
    int ox = Platform::offsetX();
    int oy = Platform::offsetY();
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
