#pragma once

#include <string>
#include <array>
#include <memory>
#include <vector>
#include <cstdint>

#include "Constants.h"
#include "Tetrimino.h"

enum class GameStep { Idle, MoveLeft, MoveRight, HardDrop };

enum MODE {
	EXTENDED,
	EXTENDED_INFINITY,
	CLASSIC
};

class GameState
{
public:
	GameState();
	~GameState();

	void loadHighscore();
	void saveHighscore();
	[[nodiscard]] Tetrimino* peekTetrimino() const;

	GameMatrix _matrix;

	MODE _mode = EXTENDED;
	int _startingLevel = 1;
	int _level{};
	int _lines{};
	int _goal{};
	int64_t _score{};
	int64_t _highscore{};
	int _nbMoveAfterLockDown{};
	int _lowestLine{};

	unsigned int _bagIndex{};
	Tetrimino* _currentTetrimino{};
	Tetrimino* _holdTetrimino{};
	std::vector<std::unique_ptr<Tetrimino>> _bag;

	// Tetris Guideline gravity values for levels 1-15 (index 0 unused).
	static constexpr std::array<double, 16> kSpeedNormal = {
		0, 1.0, 0.793, 0.618, 0.473, 0.355, 0.262, 0.190, 0.135, 0.094, 0.064, 0.043, 0.028, 0.018, 0.011, 0.007
	};
	static constexpr std::array<double, 16> kSpeedFast = {
		0, 0.05, 0.03965, 0.0309, 0.02365, 0.01775, 0.0131, 0.0095, 0.00675, 0.0047, 0.0032, 0.00215, 0.0014,
		0.0009, 0.00055, 0.00035
	};

	bool _isStarted{};
	bool _shouldExit{};
	bool _didRotate{};
	bool _isGameOver{};
	bool _shouldIgnoreHardDrop{};
	bool _lastMoveIsTSpin{};
	bool _lastMoveIsMiniTSpin{};
	bool _backToBackBonus{};
	bool _isInLockDown{};
	bool _isNewHold{};
	bool _hasBetterHighscore{};

	GameStep _stepState = GameStep::Idle;
};
