#pragma once

#include <vector>

enum class GameStep { Idle, MoveLeft, MoveRight, HardDrop };
enum class StepResult { Continue, PauseRequested, GameOver };
enum class GameSound { Click, Lock, HardDrop, LineClear, Tetris };
enum class GamePhase { Generation, Falling, Pattern, Iterate, Animate, Eliminate, Completion };

struct LineClearState {
	std::vector<int> rows;   // matrix-row indices of detected full rows
	bool flashOn{};
};
