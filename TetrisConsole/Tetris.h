#pragma once

#include <string>
#include <queue>

#include "Tetrimino.h"
#include "Vector2i.h"
#include "Timer.h"
#include "Menu.h"

using namespace std;

class Tetris
{
public:
	Tetris(Menu& pauseMenu);
	~Tetris();

	void display();
	void refresh();
	void step();
	bool doExit();

protected:
	void printLine(int line);
	string valueToString(int value, int length);

	void printMatrix();
	void printPreview();
	void printScore();

	void reset();
	void lock();
	void shuffle();
	void popTetrimino();
	Tetrimino* peekTetrimino();

	void fall();

	void stepIdle();
	void stepMoveLeft();
	void stepMoveRight();
	void stepHardDrop();
	void stepGameOver();
	void stepPause();

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

	int _level;
	int _lines;
	int _goal;
	int _score;
	int _highscore;
	int _nbMoveAfterLockDown;
	int _lowestLine;

	unsigned int _bagIndex;
	Tetrimino* _currentTetrimino;
	vector<Tetrimino*> _bag;

	double* _speedNormal;
	double* _speedFast;

	bool _exit;
	bool _didRotate;
	bool _gameOver;
	bool _ignoreHardDrop;
	bool _lastMoveIsTSpin;
	bool _lastMoveIsMiniTSpin;
	bool _backToBackBonus;

	void(Tetris::*_stepState)();

	Tetrimino* _tspinTest;
};

