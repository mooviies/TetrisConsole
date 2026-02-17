#pragma once

#include <string>
#include <vector>

#include "Constants.h"

enum class GameStep { Idle, MoveLeft, MoveRight, HardDrop };
enum class StepResult { Continue, PauseRequested, GameOver };
enum class GameSound { Click, Lock, HardDrop, LineClear, Tetris };
enum class GamePhase { Generation, Falling, Pattern, Iterate, Animate, Eliminate, Completion };

struct LineClearState {
    std::vector<int> rows; // matrix-row indices of detected full rows
    bool flashOn{};
    std::string notificationText; // e.g. "TETRIS!", "B2B T-SPIN DOUBLE"
    int notificationColor{};      // Color:: constant
    std::string comboText;        // e.g. "COMBO x3"
    int comboColor{};
};

struct HardDropTrail {
    int startRow{};               // first row of the trail (piece top before drop)
    int endRow{};                 // last row of the trail (piece top after drop)
    int visibleStartRow{};        // current top of visible trail (advances downward)
    int color{};                  // piece color
    bool active{};
    bool columns[TETRIS_WIDTH]{}; // which columns the trail covers
};
