#pragma once

#include <string>
#include <array>
#include <memory>
#include <cstdint>

#include "Tetrimino.h"
#include "Timer.h"
#include "Menu.h"

enum MODE {
	EXTENDED,
	EXTENDED_INFINITY,
	CLASSIC
};

class Tetris
{
public:
	Tetris(Menu& pauseMenu, Menu& gameOverMenu);
	~Tetris();

	void start();
	static void display();
	void refresh();
	void step();
	void exit();
	[[nodiscard]] bool doExit() const;
	void setStartingLevel(int level);
	void setMode(MODE mode);

protected:
	void printLine(int line, bool visible) const;

	void printMatrix(bool visible = true);
	void printPreview() const;
	void printScore();

	void reset();
	void lock();
	void shuffle();
	void popTetrimino();
	[[nodiscard]] Tetrimino* peekTetrimino() const;

	void gameOver();

	void fall();

	void stepIdle();
	void stepMoveLeft();
	void stepMoveRight();
	void stepHardDrop();
	void incrementMove();

	void smallResetLockDown() const;
	void moveLeft();
	void moveRight();
	[[nodiscard]] bool moveDown();
	void rotate(DIRECTION direction);
	void rotateClockwise();
	void rotateCounterClockwise();
	void checkAutorepeat(bool input, const std::string& timer, void(Tetris::*move)(), void(Tetris::*state)());

private:
	std::vector<std::vector<int>> _matrix;
	Timer& _timer;
	Menu& _pauseMenu;
	Menu& _gameOverMenu;

	MODE _mode;

	int _startingLevel;
	int _level{};
	int _lines{};
	int _goal{};
	int64_t _score{};
	int64_t _highscore;
	int _nbMoveAfterLockDown{};
	int _lowestLine{};

	unsigned int _bagIndex{};
	Tetrimino* _currentTetrimino{};
	Tetrimino* _holdTetrimino;
	std::vector<std::unique_ptr<Tetrimino>> _bag;

	// Tetris Guideline gravity values for levels 1-15 (index 0 unused).
	// Units: seconds per cell drop at 60fps equivalent.
	static constexpr std::array<double, 16> kSpeedNormal = {
		0, 1.0, 0.793, 0.618, 0.473, 0.355, 0.262, 0.190, 0.135, 0.094, 0.064, 0.043, 0.028, 0.018, 0.011, 0.007
	};
	static constexpr std::array<double, 16> kSpeedFast = {
		0, 0.05, 0.03965, 0.0309, 0.02365, 0.01775, 0.0131, 0.0095, 0.00675, 0.0047, 0.0032, 0.00215, 0.0014,
		0.0009, 0.00055, 0.00035
	};

	bool _isStarted;
	bool _shouldExit{};
	bool _didRotate{};
	bool _isGameOver{};
	bool _shouldIgnoreHardDrop{};
	bool _lastMoveIsTSpin{};
	bool _lastMoveIsMiniTSpin{};
	bool _backToBackBonus{};
	bool _isInLockDown{};
	bool _isNewHold;
	bool _hasBetterHighscore{};

	// Function-pointer state machine. Points to stepIdle/stepMoveLeft/stepMoveRight/stepHardDrop.
	void(Tetris::*_stepState)(){};

	Tetrimino* _tspinTest;
};
