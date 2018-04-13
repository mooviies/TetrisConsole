#include "stdafx.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>

#include "Tetris.h"
#include "OTetrimino.h"
#include "ITetrimino.h"
#include "TTetrimino.h"
#include "LTetrimino.h"
#include "JTetrimino.h"
#include "STetrimino.h"
#include "ZTetrimino.h"
#include "Random.h"
#include "Input.h"
#include "Utility.h"
#include "SoundEngine.h"

#define FALL "fall"
#define AUTOREPEAT_LEFT "autorepeatleft"
#define AUTOREPEAT_RIGHT "autorepeatright"
#define LOCK_DOWN "lockdown"
#define PAUSE "pause"

#define SCORE_FILE "score.bin"

#define AUTOREPEAT_DELAY 0.25
#define AUTOREPEAT_SPEED 0.01
#define LOCK_DOWN_DELAY 0.5
#define LOCK_DOWN_MOVE 15
#define PAUSE_DELAY 0.3

Tetris::Tetris()
	: _timer(Timer::instance())
{
	double speedNormal[] = { 0, 1.0, 0.793, 0.618, 0.473, 0.355, 0.262, 0.190, 0.135, 0.094, 0.064, 0.043, 0.028, 0.018, 0.011, 0.007 };
	double speedFast[] = { 0, 0.05, 0.03965, 0.0309, 0.02365, 0.01775, 0.0131, 0.0095, 0.00675, 0.0047, 0.0032, 0.00215, 0.0014, 0.0009, 0.00055, 0.00035 };
	_speedNormal = new double[16];
	_speedFast = new double[16];

	for (int i = 0; i < 16; i++)
	{
		_speedNormal[i] = speedNormal[i];
		_speedFast[i] = speedFast[i];
	}

	_bag.push_back(new OTetrimino(_matrix));
	_bag.push_back(new ITetrimino(_matrix));
	_bag.push_back(new TTetrimino(_matrix));
	_bag.push_back(new LTetrimino(_matrix));
	_bag.push_back(new JTetrimino(_matrix));
	_bag.push_back(new STetrimino(_matrix));
	_bag.push_back(new ZTetrimino(_matrix));
	_tspinTest = _bag[2];

	for (int i = 0; i < TETRIS_HEIGHT; i++)
	{
		_matrix.push_back(vector<int>(TETRIS_WIDTH));
	}

	ifstream highscoreFile(SCORE_FILE);

	if (highscoreFile.is_open())
	{
		highscoreFile >> _highscore;
		highscoreFile.close();
	}
	else
	{
		ofstream highscoreFile(SCORE_FILE);
		_highscore = 0;
		highscoreFile << _highscore;
		highscoreFile.close();
	}

	Utility::afficherTitre("A classic in console!");
	reset();
}

Tetris::~Tetris()
{
	delete[] _speedNormal;
	delete[] _speedFast;
	for (int i = 0; i < _bag.size(); i++)
		delete _bag[i];
}

void Tetris::display()
{
	rlutil::locate(1, 6);
	
	cout << "    ÉÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ»       ÉÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ»       ÉÍÍÍÍÍÍÍÍÍÍÍÍ»" << endl;
	cout << "    º     Score      º       º                    º       º    Next    º" << endl;
	cout << "    ÌÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¹       º                    º       ÌÍÍÍÍÍÍÍÍÍÍÍÍ¹" << endl;
	cout << "    º   0000000000   º       º                    º       º            º" << endl;
	cout << "    ÌÍÍÍÍÍÍÍËÍÍÍÍÍÍÍÍ¹       º                    º       º            º" << endl;
	cout << "    º Level º   01   º       º                    º       º            º" << endl;
	cout << "    ÌÍÍÍÍÍÍÍÎÍÍÍÍÍÍÍÍ¹       º                    º       º            º" << endl;
	cout << "    º Lines º 000001 º       º                    º       ÈÍÍÍÍÍÍÍÍÍÍÍÍ¼" << endl;
	cout << "    ÈÍÍÍÍÍÍÍÊÍÍÍÍÍÍÍÍ¼       º                    º                     " << endl;

	for (int i = 28; i <= MATRIX_END; i++)
	{
		cout << "                             º                    º                     " << endl;
	}
	cout << "                             ÈÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¼                     " << endl << endl;
}

void Tetris::refresh()
{
	printMatrix();
	printPreview();
	printScore();
}

void Tetris::step()
{
	if (Input::pause() && (_timer.getSeconds(PAUSE) >= PAUSE_DELAY))
	{
		_stepState = &Tetris::stepPause;
		_timer.resetTimer(PAUSE);
	}

	(*this.*_stepState)();

	if (!_didRotate)
	{
		if (Input::rotateClockwise())
			rotateClockwise();
		else if (Input::rotateCounterClockwise())
			rotateCounterClockwise();
	}
	else
	{
		if (!Input::rotateClockwise() && !Input::rotateCounterClockwise())
			_didRotate = false;
	}
}

void Tetris::fall()
{
	if (_currentTetrimino == NULL)
		return;

	int speedIndex = _level;
	if (speedIndex > 15)
		speedIndex = 15;

	double* speedArray = _speedNormal;
	bool isSoftDropping = false;
	isSoftDropping = Input::softDrop();
	if (isSoftDropping)
		speedArray = _speedFast;

	if (_timer.getSeconds(FALL) >= speedArray[speedIndex])
	{
		_timer.resetTimer(FALL);
		if (moveDown())
		{
			if (isSoftDropping)
				_score++;

			if (_timer.exist(LOCK_DOWN))
			{
				int currentLine = _currentTetrimino->getPosition().column;
				if (currentLine > _lowestLine)
				{
					_lowestLine = currentLine;
					_nbMoveAfterLockDown = 0;
					_timer.resetTimer(LOCK_DOWN);
				}
			}
		}
	}

	if (!_currentTetrimino->simulateMove(Vector2i(1, 0)) && !_timer.exist(LOCK_DOWN))
	{
		_timer.startTimer(LOCK_DOWN);
	}

	if (_timer.getSeconds(LOCK_DOWN) >= LOCK_DOWN_DELAY)
	{
		lock();
	}

	if (_nbMoveAfterLockDown >= LOCK_DOWN_MOVE)
		lock();

	if (Input::hardDrop() && !_ignoreHardDrop)
	{
		_stepState = &Tetris::stepHardDrop;
		_ignoreHardDrop = true;
	}
	else if (!Input::hardDrop() && _ignoreHardDrop)
	{
		_ignoreHardDrop = false;
	}
}

void Tetris::stepIdle()
{
	if (_currentTetrimino == NULL)
	{
       		popTetrimino();
		if (!_currentTetrimino->setPosition(_currentTetrimino->getStartingPosition()))
		{
			_stepState = &Tetris::stepGameOver;
			_gameOver = true;
			return;
		}
		_timer.startTimer(FALL);
		refresh();
	}
	
	fall();

	checkAutorepeat(Input::left(), AUTOREPEAT_LEFT, &Tetris::moveLeft, &Tetris::stepMoveLeft);
	checkAutorepeat(Input::right(), AUTOREPEAT_RIGHT, &Tetris::moveRight, &Tetris::stepMoveRight);
}

void Tetris::stepMoveLeft()
{
	if (_currentTetrimino == NULL)
	{
		_stepState = &Tetris::stepIdle;
		return;
	}

	fall();

	if (!Input::left())
	{
		_stepState = &Tetris::stepIdle;
		_timer.stopTimer(AUTOREPEAT_LEFT);
	}

	if (_timer.getSeconds(AUTOREPEAT_LEFT) >= AUTOREPEAT_SPEED)
	{
		moveLeft();
		_timer.resetTimer(AUTOREPEAT_LEFT);
	}
}

void Tetris::stepMoveRight()
{
	if (_currentTetrimino == NULL)
	{
		_stepState = &Tetris::stepIdle;
		return;
	}

	fall();

	if (!Input::right())
	{
		_stepState = &Tetris::stepIdle;
		_timer.stopTimer(AUTOREPEAT_RIGHT);
	}

	if (_timer.getSeconds(AUTOREPEAT_RIGHT) >= AUTOREPEAT_SPEED)
	{
		moveRight();
		_timer.resetTimer(AUTOREPEAT_RIGHT);
	}
}

void Tetris::stepHardDrop()
{
	if (_currentTetrimino == NULL)
	{
		_stepState = &Tetris::stepIdle;
		return;
	}

	while(moveDown())
	{
		_score += 2;
	}
	lock();
}

void Tetris::stepGameOver()
{
	if (_gameOver)
	{
		rlutil::setColor(rlutil::WHITE);
		rlutil::locate(29, 15);
		cout << "ÉÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ»";
		rlutil::locate(29, 16);
		cout << "º                      º";
		rlutil::locate(29, 17);
		cout << "º      GAME  OVER      º";
		rlutil::locate(29, 18);
		cout << "º                      º";

		if (_score > _highscore)
		{
			rlutil::locate(29, 19);
			cout << "º    New High Score!   º";
			rlutil::locate(29, 20);
			cout << "º                      º";
			rlutil::locate(29, 21);
			cout << "ÈÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¼";

			ofstream highscoreFile(SCORE_FILE);
			highscoreFile << _highscore;
			highscoreFile.close();
			_highscore = _score;
		}
		else
		{
			rlutil::locate(29, 19);
			cout << "ÈÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¼";
		}

		_gameOver = false;
	}
	
	if (Input::select())
	{
		reset();
		return;
	}

	if (Input::pause())
		_exit = true;
}

void Tetris::stepPause()
{
	rlutil::setColor(rlutil::WHITE);
	rlutil::locate(29, 15);
	cout << "ÉÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ»";
	rlutil::locate(29, 16);
	cout << "º                      º";
	rlutil::locate(29, 17);
	cout << "º         PAUSE        º";
	rlutil::locate(29, 18);
	cout << "º                      º";
	rlutil::locate(29, 19);
	cout << "ÈÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¼";
	SoundEngine::pauseMusic();

	float t = _timer.getSeconds(PAUSE);
	
	if (_timer.getSeconds(PAUSE) >= PAUSE_DELAY)
	{
		if (Input::pause())
		{
			display();
			refresh();
			_stepState = &Tetris::stepIdle;
			_timer.resetTimer(PAUSE);
			SoundEngine::unpauseMusic();
		}
	}
}

void Tetris::moveLeft()
{
	if (_currentTetrimino == NULL)
		return;

	if (_currentTetrimino->move(Vector2i(0, -1)))
	{
		_lastMoveIsTSpin = false;
		_lastMoveIsMiniTSpin = false;
		refresh();
	}
}

void Tetris::moveRight()
{
	if (_currentTetrimino == NULL)
		return;

	if (_currentTetrimino->move(Vector2i(0, 1)))
	{
		_lastMoveIsTSpin = false;
		_lastMoveIsMiniTSpin = false;
		refresh();
	}
}

bool Tetris::moveDown()
{
	if (_currentTetrimino == NULL)
		return false;

	if (_currentTetrimino->move(Vector2i(1, 0)))
	{
		_lastMoveIsTSpin = false;
		_lastMoveIsMiniTSpin = false;
		refresh();
		return true;
	}

	return false;
}

void Tetris::rotateClockwise()
{
	rotate(RIGHT);
}

void Tetris::rotateCounterClockwise()
{
	rotate(LEFT);
}

void Tetris::rotate(DIRECTION direction)
{
	if (_currentTetrimino == NULL)
		return;

	if (_currentTetrimino->rotate(direction))
	{
		_didRotate = true;
		_lastMoveIsTSpin = false;
		_lastMoveIsMiniTSpin = false;
		refresh();

		if (_currentTetrimino->canTSpin())
		{
			if (_currentTetrimino->checkTSpin())
			{
				_score += 400 * _level;
				_lastMoveIsTSpin = true;
			}
			else if (_currentTetrimino->checkMiniTSpin())
			{
				_score += 100 * _level;
				_lastMoveIsMiniTSpin = true;
			}
		}
	}
}

void Tetris::checkAutorepeat(bool input, string timer, void(Tetris::*move)(), void(Tetris::*state)())
{
	if (input)
	{
		if (!_timer.exist(timer))
		{
			(*this.*move)();
			_timer.startTimer(timer);
		}

		if (_timer.getSeconds(timer) >= AUTOREPEAT_DELAY)
		{
			_timer.startTimer(timer);
			(*this.*move)();
			_stepState = state;
		}
	}
	else
	{
		_timer.stopTimer(timer);
	}
}

bool Tetris::doExit()
{
	return _exit;
}

void Tetris::printMatrix()
{
	for (int i = MATRIX_START; i <= MATRIX_END; i++)
		printLine(i);
}

void Tetris::printPreview()
{
	peekTetrimino()->printPreview(0);
	peekTetrimino()->printPreview(1);
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

		bool currentTetriminoHere = false;
		if(_currentTetrimino != NULL)
			currentTetriminoHere = _currentTetrimino->isMino(line, i);
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

void Tetris::reset()
{
	_level = 1;
	_lines = 0;
	_goal = 0;
	_score = 0;
	_gameOver = false;
	_ignoreHardDrop = false;
	_lastMoveIsTSpin = false;
	_lastMoveIsMiniTSpin = false;
	_backToBackBonus = false;

	for (int i = 0; i < TETRIS_HEIGHT; i++)
	{
		for (int j = 0; j < TETRIS_WIDTH; j++)
		{
			_matrix[i][j] = 0;
		}
	}

	_currentTetrimino = NULL;
	_bagIndex = 0;
	shuffle();

	_stepState = &Tetris::stepIdle;
	_didRotate = false;
	_nbMoveAfterLockDown = 0;
	_lowestLine = 0;

	_timer.stopTimer(LOCK_DOWN);
	_timer.stopTimer(FALL);
	_timer.stopTimer(AUTOREPEAT_LEFT);
	_timer.stopTimer(AUTOREPEAT_RIGHT);

	display();
	refresh();

	_timer.startTimer(PAUSE);

	SoundEngine::playMusic("A");
}

void Tetris::lock()
{
	if (_currentTetrimino == NULL)
		return;

	if (_currentTetrimino->simulateMove(Vector2i(1, 0)))
	{
		_timer.stopTimer(LOCK_DOWN);
		_nbMoveAfterLockDown = 0;
		int row = _currentTetrimino->getPosition().row;
		if (row > _lowestLine)
			_lowestLine = row;
		return;
	}

	if (!_currentTetrimino->lock())
	{
 		_stepState = &Tetris::stepGameOver;
		_gameOver = true;
		return;
	}

	SoundEngine::playSound("LOCK");
	
	_nbMoveAfterLockDown = 0;
	_lowestLine = 0;
	_timer.stopTimer(LOCK_DOWN);
          	_currentTetrimino = NULL;

	int nbLine = 0;
	for (int i = MATRIX_END; i >= MATRIX_START; i--)
	{
		bool fullLine = true;
		for (int j = 0; j < TETRIS_WIDTH; j++)
		{
			if (_matrix[i][j] == 0)
			{
				fullLine = false;
				break;
			}
		}

		if (fullLine)
		{
			nbLine++;
			_matrix.erase(_matrix.begin() + i);
		}
	}

	for(int i = 0; i < nbLine; i++)
		_matrix.insert(_matrix.begin(), vector<int>(TETRIS_WIDTH));

	_lines += nbLine;
	_goal += nbLine;

	if (_lastMoveIsTSpin)
	{
		int value = 0;
		switch (nbLine)
		{
		case 1:
			value = 400;
			break;
		case 2:
			value = 800;
			break;
		case 3:
			value = 1200;
			break;
		}

		if (_backToBackBonus)
			value += value / 2;

		_score += value * _level;
	}
	else if (_lastMoveIsMiniTSpin)
	{
		if (nbLine == 1)
		{
			int value = 100;
			if (_backToBackBonus)
				value += value / 2;

			_score += value * _level;
		}
	}
	else
	{
		int value = 0;
		switch (nbLine)
		{
		case 1:
			value = 100;
			_backToBackBonus = false;
			break;
		case 2:
			value = 300;
			_backToBackBonus = false;
			break;
		case 3:
			value = 500;
			_backToBackBonus = false;
			break;
		case 4:
			value = 800;
			if (_backToBackBonus)
				value += value / 2;

			_backToBackBonus = true;
			break;
		}

		_score += value * _level;
	}

	_lastMoveIsTSpin = false;
	_lastMoveIsMiniTSpin = false;

	if (_goal >= _level * 5)
	{
		_level++;
		_goal = 0;
	}

	_stepState = &Tetris::stepIdle;
	refresh();
}

void Tetris::shuffle()
{
	for (int i = _bag.size() - 1; i >= 0; i--)
	{
		int j = Random::getInteger(0, i);
		if (i == j)
			continue;

		Tetrimino* buffer = _bag[i];
		_bag[i] = _bag[j];
		_bag[j] = buffer;
	}
	_bagIndex = 0;
}

void Tetris::popTetrimino()
{
	_currentTetrimino = _tspinTest; //TODO REMOVE
	return; //TODO REMOVE
	_currentTetrimino = _bag[_bagIndex++];
	if (_bagIndex >= _bag.size())
	{
		shuffle();
	}
}

Tetrimino* Tetris::peekTetrimino()
{
	return _bag[_bagIndex];
}