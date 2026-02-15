#pragma once

#include <string>
#include <vector>

enum class GameStep { Idle, MoveLeft, MoveRight, HardDrop };
enum class StepResult { Continue, PauseRequested, GameOver };
enum class GameSound { Click, Lock, HardDrop, LineClear, Tetris };
enum class GamePhase { Generation, Falling, Pattern, Iterate, Animate, Eliminate, Completion };

struct LineClearState {
	std::vector<int> rows;   // matrix-row indices of detected full rows
	bool flashOn{};
	std::string notificationText;   // e.g. "TETRIS!", "B2B T-SPIN DOUBLE"
	int notificationColor{};        // Color:: constant
	std::string comboText;          // e.g. "COMBO x3"
	int comboColor{};
};
