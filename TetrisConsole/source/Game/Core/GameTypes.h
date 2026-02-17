#pragma once

#include <string>
#include <vector>

#include "Constants.h"

enum class GameStep { Idle, MoveLeft, MoveRight, HardDrop };
enum class StepResult { Continue, PauseRequested, GameOver };
enum class GameSound { Click, Lock, HardDrop, LineClear, Quad };
enum class GamePhase { Generation, Falling, Pattern, Iterate, Animate, Eliminate, Completion };

struct LineClearState {
    std::vector<int> rows; // matrix-row indices of detected full rows
    bool flashOn{};
    std::string notificationText; // e.g. "QUAD!", "B2B T-SPIN DOUBLE"
    int notificationColor{};      // Color:: constant
    std::string comboText;        // e.g. "COMBO x3"
    int comboColor{};
};

struct HardDropTrail {
    int startRow{};               // piece top-left row before drop (fixed)
    int endRow{};                 // piece top-left row after drop (fixed)
    int visibleStartRow{};        // shrinks toward endRow for top-to-bottom fade
    int color{};
    bool active{};
    bool columns[BOARD_WIDTH]{};
};
