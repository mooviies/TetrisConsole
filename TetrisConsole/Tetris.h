#pragma once

#include <string>

#include "Tetrimino.h"
#include "Vector2i.h"

using namespace std;

class Tetris
{
public:
	Tetris();
	~Tetris();

	void display();
	void moveLeft();
	void moveRight();
	void moveDown();
	void moveUp();
	void rotateClockwise();
	void rotateCounterClockwise();
	void refresh();

protected:
	void printLine(int line);
	string valueToString(int value, int length);
	COORD getCursorPosition();
	void printMatrix();
	void printPreview();
	void printScore();
	void writeString(int& currentX, int currentY, string text);

private:
	int** _matrix;

	int _level;
	int _lines;
	int _score;

	Tetrimino* _currentTetrimino;
	Tetrimino* _nextPiece;
	Tetrimino* _bag[7] = { NULL };
	Tetrimino* _allPieces[7] = { NULL };
};

