#pragma once

#include <string>
#include <queue>

#include "Tetrimino.h"
#include "Vector2i.h"
#include "Timer.h"
#include "Menu.h"

using namespace std;

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
	void display();
	void refresh();
	void step();
	bool doExit();
	void setStartingLevel(int level);
	void setMode(MODE mode);

protected:
	void printLine(int line, bool visible);

	void printMatrix(bool visible = true);
	void printPreview();
	void printScore();

	void reset();
	void lock();
	void shuffle();
	void popTetrimino();
	Tetrimino* peekTetrimino();

	void gameOver();

	void fall();

	void stepIdle();
	void stepMoveLeft();
	void stepMoveRight();
	void stepHardDrop();
	void incrementMove();

	void smallResetLockDown();
	void moveLeft();
	void moveRight();
	bool moveDown();
	void rotate(DIRECTION direction);
	void rotateClockwise();
	void rotateCounterClockwise();
	void checkAutorepeat(bool input, string timer, void(Tetris::*move)(), void(Tetris::*state)());

private:
	vector<vector<int>> _matrix;
	Timer& _timer;
	Menu& _pauseMenu;
	Menu& _gameOverMenu;

	MODE _mode;

	int _startingLevel;
	int _level;
	int _lines;
	int _goal;
	int _score;
	int _highscore;
	int _nbMoveAfterLockDown;
	int _lowestLine;

	unsigned int _bagIndex;
	Tetrimino* _currentTetrimino;
	Tetrimino* _holdTetrimino;
	vector<Tetrimino*> _bag;

	double* _speedNormal;
	double* _speedFast;

	bool _start;
	bool _exit;
	bool _didRotate;
	bool _gameOver;
	bool _ignoreHardDrop;
	bool _lastMoveIsTSpin;
	bool _lastMoveIsMiniTSpin;
	bool _backToBackBonus;
	bool _lockDownMode;
	bool _newHold;
	bool _betterHighscore;

	void(Tetris::*_stepState)();

	Tetrimino* _tspinTest;
};

