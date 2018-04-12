﻿#include "stdafx.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "Tetris.h"
#include "OTetrimino.h"
#include "ITetrimino.h"
#include "TTetrimino.h"
#include "LTetrimino.h"
#include "JTetrimino.h"
#include "STetrimino.h"
#include "ZTetrimino.h"

Tetris::Tetris()
{
	_level = 1;

	_matrix = new int*[TETRIS_HEIGHT];
	for (int i = 0; i < TETRIS_HEIGHT; i++)
	{
		_matrix[i] = new int[TETRIS_WIDTH];
		for (int j = 0; j < TETRIS_WIDTH; j++)
		{
			if (i > 35 && j > 1 && j < TETRIS_WIDTH - 2)
				_matrix[i][j] = rlutil::GREY;
			else
				_matrix[i][j] = 0;
		}
	}

	_currentTetrimino = new ZTetrimino(_matrix);
	_currentTetrimino->setPosition(Vector2i(30, 5));
}

Tetris::~Tetris()
{
	for (int i = 0; i < TETRIS_HEIGHT; i++)
	{
		delete _matrix[i];
	}
	delete _matrix;
}

void Tetris::display()
{
	cout << "    ÉÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ»       ÉÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ»       ÉÍÍÍÍÍÍÍÍÍÍÍÍ»" << endl;
	cout << "    º     Score      º       º                    º       º    Next    º" << endl;
	cout << "    ÌÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¹       º                    º       ÌÍÍÍÍÍÍÍÍÍÍÍÍ¹" << endl;
	cout << "    º   0000000000   º       º                    º       º            º" << endl;
	cout << "    ÌÍÍÍÍÍÍÍËÍÍÍÍÍÍÍÍ¹       º                    º       º            º" << endl;
	cout << "    º Level º   01   º       º                    º       º            º" << endl;
	cout << "    ÌÍÍÍÍÍÍÍÎÍÍÍÍÍÍÍÍ¹       º                    º       º            º" << endl;
	cout << "    º Lines º 000001 º       º                    º       ÈÍÍÍÍÍÍÍÍÍÍÍÍ¼" << endl;
	cout << "    ÈÍÍÍÍÍÍÍÊÍÍÍÍÍÍÍÍ¼       º                    º"					   << endl;

	for (int i = 28; i <= MATRIX_END; i++)
	{
		cout << "                             º                    º" << endl;
	}
	cout << "                             ÈÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¼" << endl << endl;
}

void Tetris::refresh()
{
	printMatrix();
	printPreview();
	printScore();
}

void Tetris::printMatrix()
{
	for (int i = MATRIX_START; i <= MATRIX_END; i++)
		printLine(i);
}

void Tetris::printPreview()
{
	_currentTetrimino->printPreview(0);
	_currentTetrimino->printPreview(1);
}

void Tetris::printScore()
{
	rlutil::locate(9, 9);
	cout << valueToString(_score, 10);
	rlutil::locate(17, 11);
	cout << valueToString(_level, 2);
	rlutil::locate(15, 13);
	cout << valueToString(_lines, 6);
}

void Tetris::printLine(int line)
{
	int x = 31, y = 7 + line - MATRIX_START;
	
	for (int i = 0; i < TETRIS_WIDTH; i++)
	{
		rlutil::locate(x, y);
		x += 2;
		bool currentTetriminoHere = _currentTetrimino->isMino(line, i);
		if (_matrix[line][i] > 0 || currentTetriminoHere)
		{
			if (currentTetriminoHere)
				rlutil::setColor(_currentTetrimino->getColor());
			else
				rlutil::setColor(_matrix[line][i]);
			cout << "ÛÛ";
			rlutil::setColor(rlutil::WHITE);
		}
		else
			cout << "  ";
	}
}

string Tetris::valueToString(int value, int length)
{
	string result;
	int decimal = 1;
	for (int i = 0; i < length; i++)
	{
		if (value < decimal)
			result += "0";

		decimal *= 10;
	}

	char buffer[10];
	if (value > 0)
	{
		_itoa_s(value, buffer, 10, 10);
		result += buffer;
	}

	return result;
}

COORD Tetris::getCursorPosition()
{
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD GetConsoleCursorPosition(HANDLE out);
	CONSOLE_SCREEN_BUFFER_INFO cbsi;
	if (GetConsoleScreenBufferInfo(out, &cbsi))
	{
		return cbsi.dwCursorPosition;
	}
	else
	{
		COORD invalid = { 0, 0 };
		return invalid;
	}
}

void Tetris::moveLeft()
{
	_currentTetrimino->move(Vector2i(0, -1));
}

void Tetris::moveRight()
{
	_currentTetrimino->move(Vector2i(0, 1));
}

void Tetris::moveDown()
{
	_currentTetrimino->move(Vector2i(1, 0));
}

void Tetris::moveUp()
{
	_currentTetrimino->move(Vector2i(-1, 0));
}

void Tetris::rotateClockwise()
{
	_currentTetrimino->rotate(RIGHT);
}

void Tetris::rotateCounterClockwise()
{
	_currentTetrimino->rotate(LEFT);
}
