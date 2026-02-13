#include "GameRenderer.h"

#include <iostream>

#include "GameState.h"
#include "Constants.h"
#include "Utility.h"
#include "rlutil.h"

using namespace std;

GameRenderer::GameRenderer() = default;
GameRenderer::~GameRenderer() = default;

void GameRenderer::display() {
    rlutil::locate(1, 6);

    cout << "    ╔════════════════╗       ╔════════════════════╗       ╔════════════╗" << endl;
    cout << "    ║     Score      ║       ║                    ║       ║    Next    ║" << endl;
    cout << "    ╠════════════════╣       ║                    ║       ╠════════════╣" << endl;
    cout << "    ║   0000000000   ║       ║                    ║       ║            ║" << endl;
    cout << "    ╠═══════╦════════╣       ║                    ║       ║            ║" << endl;
    cout << "    ║ Level ║   01   ║       ║                    ║       ║            ║" << endl;
    cout << "    ╠═══════╬════════╣       ║                    ║       ║            ║" << endl;
    cout << "    ║ Lines ║ 000001 ║       ║                    ║       ╚════════════╝" << endl;
    cout << "    ╚═══════╩════════╝       ║                    ║                     " << endl;

    for (int i = 28; i <= MATRIX_END; i++) {
        cout << "                             ║                    ║                     " << endl;
    }
    cout << "                             ╚════════════════════╝                     " << endl << endl;

    rlutil::locate(5, 23);
    cout << "╔════════════════╗";
    rlutil::locate(5, 24);
    cout << "║   High Score   ║";
    rlutil::locate(5, 25);
    cout << "╠════════════════╣";
    rlutil::locate(5, 26);
    cout << "║   0000000000   ║";
    rlutil::locate(5, 27);
    cout << "╚════════════════╝";

    rlutil::locate(59, 20);
    cout << "╔════════════╗";
    rlutil::locate(59, 21);
    cout << "║    Hold    ║";
    rlutil::locate(59, 22);
    cout << "╠════════════╣";
    rlutil::locate(59, 23);
    cout << "║            ║";
    rlutil::locate(59, 24);
    cout << "║            ║";
    rlutil::locate(59, 25);
    cout << "║            ║";
    rlutil::locate(59, 26);
    cout << "║            ║";
    rlutil::locate(59, 27);
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
        rlutil::locate(60, 24);
        cout << "            ";
        rlutil::locate(60, 25);
        cout << "            ";
    } else {
        state._holdTetrimino->printPreview(0, true);
        state._holdTetrimino->printPreview(1, true);
    }
}

void GameRenderer::printScore(const GameState& state) {
    rlutil::locate(9, 9);
    if (state._backToBackBonus)
        rlutil::setColor(rlutil::LIGHTGREEN);
    cout << Utility::valueToString(state._score, 10);
    rlutil::setColor(rlutil::WHITE);

    rlutil::locate(17, 11);
    cout << Utility::valueToString(state._level, 2);
    rlutil::locate(15, 13);
    cout << Utility::valueToString(state._lines, 6);
    rlutil::locate(9, 26);
    cout << Utility::valueToString(state._highscore, 10);
}

void GameRenderer::printLine(const GameState& state, const int line, const bool visible) {
    int x = 31;
    const int y = 7 + line - MATRIX_START;

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
