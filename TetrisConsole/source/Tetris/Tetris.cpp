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

#define SCORE_FILE "score.bin"

#define AUTOREPEAT_DELAY 0.25
#define AUTOREPEAT_SPEED 0.01
#define LOCK_DOWN_DELAY 0.5
#define LOCK_DOWN_SMALL_DELAY 0.2
#define LOCK_DOWN_MOVE 15

Tetris::Tetris(Menu& pauseMenu, Menu& gameOverMenu)
	: _timer(Timer::instance()), _pauseMenu(pauseMenu), _gameOverMenu(gameOverMenu), _startingLevel(1), _mode(EXTENDED), _start(false), _newHold(false), _holdTetrimino(NULL)
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
}

Tetris::~Tetris()
{
	delete[] _speedNormal;
	delete[] _speedFast;
	for (int i = 0; i < _bag.size(); i++)
		delete _bag[i];
}

void Tetris::start()
{
	reset();
	_start = true;
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

	rlutil::locate(5, 23);
	cout << "ÉÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ»";
	rlutil::locate(5, 24);
	cout << "º   High Score   º";
	rlutil::locate(5, 25);
	cout << "ÌÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¹";
	rlutil::locate(5, 26);
	cout << "º   0000000000   º";
	rlutil::locate(5, 27);
	cout << "ÈÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¼";

	rlutil::locate(59, 20);
	cout << "ÉÍÍÍÍÍÍÍÍÍÍÍÍ»";
	rlutil::locate(59, 21);
	cout << "º    Hold    º";
	rlutil::locate(59, 22);
	cout << "ÌÍÍÍÍÍÍÍÍÍÍÍÍ¹";
	rlutil::locate(59, 23);
	cout << "º            º";
	rlutil::locate(59, 24);
	cout << "º            º";
	rlutil::locate(59, 25);
	cout << "º            º";
	rlutil::locate(59, 26);
	cout << "º            º";
	rlutil::locate(59, 27);
	cout << "ÈÍÍÍÍÍÍÍÍÍÍÍÍ¼";
}

void Tetris::refresh()
{
	printMatrix();
	printPreview();
	printScore();
}

void Tetris::step()
{
	if (!_start)
		return;

	(*this.*_stepState)();

	if (Input::pause())
	{
		SoundEngine::pauseMusic();
		printMatrix(false);
		OptionChoice choices = _pauseMenu.open();
		if (choices.options[choices.selected] == "Restart")
		{
			SoundEngine::stopMusic();
			reset();
			return;
		}
		display();
		refresh();
		SoundEngine::unpauseMusic();
	}

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

			if (_lockDownMode)
			{
				int currentLine = _currentTetrimino->getPosition().row;
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
		_lockDownMode = true;
	}

	if (_timer.getSeconds(LOCK_DOWN) >= LOCK_DOWN_DELAY)
	{
		lock();
	}

	if ((_mode == EXTENDED) && (_nbMoveAfterLockDown >= LOCK_DOWN_MOVE))
		lock();

	if (Input::hardDrop() && !_ignoreHardDrop)
	{
		SoundEngine::playSound("HARD_DROP");
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
			gameOver();
			return;
		}
		_timer.startTimer(FALL);
		refresh();
	}
	
	fall();

	checkAutorepeat(Input::left(), AUTOREPEAT_LEFT, &Tetris::moveLeft, &Tetris::stepMoveLeft);
	checkAutorepeat(Input::right(), AUTOREPEAT_RIGHT, &Tetris::moveRight, &Tetris::stepMoveRight);

	if (!_newHold && Input::hold())
	{
		Tetrimino* buffer = _holdTetrimino;;
		_holdTetrimino = _currentTetrimino;
		_currentTetrimino = buffer;
		if (_currentTetrimino != NULL)
		{
			_currentTetrimino->resetRotation();
			if (!_currentTetrimino->setPosition(_currentTetrimino->getStartingPosition()))
			{
				gameOver();
				return;
			}
			_timer.startTimer(FALL);
			refresh();
		}
		_newHold = true;
	}
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

void Tetris::incrementMove()
{
	SoundEngine::playSound("CLICK");
	if (_lockDownMode)
		_nbMoveAfterLockDown++;
}

void Tetris::smallResetLockDown()
{
	if (_mode == CLASSIC)
		return;

	if (_timer.exist(LOCK_DOWN))
	{
		if (_timer.getSeconds(LOCK_DOWN) >= LOCK_DOWN_DELAY - LOCK_DOWN_SMALL_DELAY)
		{
			_timer.resetTimer(LOCK_DOWN, LOCK_DOWN_DELAY - LOCK_DOWN_SMALL_DELAY);
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
		incrementMove();
		smallResetLockDown();
		refresh();
	}}

void Tetris::moveRight()
{
	if (_currentTetrimino == NULL)
		return;

	if (_currentTetrimino->move(Vector2i(0, 1)))
	{
		_lastMoveIsTSpin = false;
		_lastMoveIsMiniTSpin = false;
		incrementMove();
		smallResetLockDown();
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
		incrementMove();
		smallResetLockDown();
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

void Tetris::setStartingLevel(int level)
{
	_startingLevel = level;
}

void Tetris::setMode(MODE mode)
{
	_mode = mode;
}

void Tetris::printMatrix(bool visible)
{
	for (int i = MATRIX_START; i <= MATRIX_END; i++)
		printLine(i, visible);
}

void Tetris::printPreview()
{
	peekTetrimino()->printPreview(0);
	peekTetrimino()->printPreview(1);

	if (_holdTetrimino == NULL)
	{
		rlutil::locate(60, 24);
		cout << "            ";
		rlutil::locate(60, 25);
		cout << "            ";
	}
	else
	{
		_holdTetrimino->printPreview(0, true);
		_holdTetrimino->printPreview(1, true);
	}
}

void Tetris::printScore()
{
	if (_score > _highscore)
	{
		_betterHighscore = true;
	}

	if (_betterHighscore)
		_highscore = _score;

	rlutil::locate(9, 9);
	if (_backToBackBonus)
		rlutil::setColor(rlutil::LIGHTGREEN);
	cout << Utility::valueToString(_score, 10);
	rlutil::setColor(rlutil::WHITE);

	rlutil::locate(17, 11);
	cout << Utility::valueToString(_level, 2);
	rlutil::locate(15, 13);
	cout << Utility::valueToString(_lines, 6);
	rlutil::locate(9, 26);
	cout << Utility::valueToString(_highscore, 10);
}

void Tetris::printLine(int line, bool visible)
{
	int x = 31, y = 7 + line - MATRIX_START;
	
	for (int i = 0; i < TETRIS_WIDTH; i++)
	{
		rlutil::locate(x, y);
		x += 2;

		bool currentTetriminoHere = false;
		if(_currentTetrimino != NULL)
			currentTetriminoHere = _currentTetrimino->isMino(line, i);
		if (visible && (_matrix[line][i] || currentTetriminoHere))
		{
			if (currentTetriminoHere)
				rlutil::setColor(_currentTetrimino->getColor());
			else
				rlutil::setColor(_matrix[line][i]);

			if (currentTetriminoHere)
				cout << "ÛÛ";
			else
			{
				rlutil::setColor(rlutil::BLACK);
				rlutil::setBackgroundColor(_matrix[line][i]);
				cout << "°°";
				rlutil::setBackgroundColor(rlutil::BLACK);
			}
				
			rlutil::setColor(rlutil::WHITE);
		}
		else
		{
			rlutil::setColor(rlutil::DARKGREY);
			if ((line % 2 == 0 && i % 2 != 0) || (line % 2 != 0 && i % 2 == 0))
				cout << "°°";
			else
				cout << "±±";
		}
	}
}

void Tetris::reset()
{
	_level = _startingLevel;
	_lines = 0;
	_goal = 0;
	_score = 0;
	_ignoreHardDrop = false;
	_lastMoveIsTSpin = false;
	_lastMoveIsMiniTSpin = false;
	_backToBackBonus = false;
	_lockDownMode = false;
	_newHold = false;
	_betterHighscore = false;

	for (int i = 0; i < TETRIS_HEIGHT; i++)
	{
		for (int j = 0; j < TETRIS_WIDTH; j++)
		{
			_matrix[i][j] = 0;
		}
	}

	_holdTetrimino = NULL;
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
		gameOver();
		return;
	}

	SoundEngine::playSound("LOCK");
	
	_newHold = false;
	_lockDownMode = false;
	_nbMoveAfterLockDown = 0;
	_lowestLine = 0;
	_timer.stopTimer(LOCK_DOWN);
    _currentTetrimino = NULL;

	int linesCleared = 0;
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
			linesCleared++;
			_matrix.erase(_matrix.begin() + i);
		}
	}

	for(int i = 0; i < linesCleared; i++)
		_matrix.insert(_matrix.begin(), vector<int>(TETRIS_WIDTH));

	int awardedLines = linesCleared;

	if (_lastMoveIsTSpin)
	{
		if (linesCleared >= 1)
		{
			int value = 0;
			switch (linesCleared)
			{
			case 1:
				value = 400;
				awardedLines = 8;
				break;
			case 2:
				value = 800;
				awardedLines = 12;
				break;
			case 3:
				value = 1200;
				awardedLines = 16;
				break;
			}

			if (_backToBackBonus)
			{
				value += value / 2;
				awardedLines += 0.5 * linesCleared;
			}

			_score += value * _level;
		}
		else
			awardedLines = 4;
	}
	else if (_lastMoveIsMiniTSpin)
	{
		if (linesCleared == 1)
		{
			int value = 100;
			awardedLines = 2;
			if (_backToBackBonus)
			{
				value += value / 2;
				awardedLines += 0.5 * linesCleared;
			}

			_score += value * _level;
		}
		else
			awardedLines = 1;
	}
	else
	{
		int value = 0;
		switch (linesCleared)
		{
		case 1:
			value = 100;
			_backToBackBonus = false;
			break;
		case 2:
			value = 300;
			awardedLines = 3;
			_backToBackBonus = false;
			break;
		case 3:
			value = 500;
			awardedLines = 5;
			_backToBackBonus = false;
			break;
		case 4:
			value = 800;
			awardedLines = 8;
			if (_backToBackBonus)
			{
				value += value / 2;
				awardedLines += 0.5 * linesCleared;
			}

			_backToBackBonus = true;
			break;
		}

		_score += value * _level;
	}

	_lastMoveIsTSpin = false;
	_lastMoveIsMiniTSpin = false;

	_lines += awardedLines;
	_goal += awardedLines;

	if (_goal >= _level * 5)
	{
		_level++;
		_goal = 0;
	}

	if(linesCleared == 4)
		SoundEngine::playSound("TETRIS");
	else if(linesCleared >= 1)
		SoundEngine::playSound("LINE_CLEAR");

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

void Tetris::gameOver()
{
	SoundEngine::stopMusic();

	ofstream highscoreFile(SCORE_FILE);
	highscoreFile << _highscore;
	highscoreFile.close();
	
	OptionChoice choice = _gameOverMenu.open(_betterHighscore);
	reset();
}
