#pragma once

#include <string>
#include <queue>

#include "Tetrimino.h"
#include "Vector2i.h"
#include "Timer.h"

using namespace std;

class Tetris
{
public:
	Tetris();
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
	void rotateClockwise();
	void rotateCounterClockwise();
	void checkAutorepeat(bool input, string timer, void(Tetris::*move)(), void(Tetris::*state)());

private:
	vector<vector<int>> _matrix;
	Timer& _timer;

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

	void(Tetris::*_stepState)();
};

