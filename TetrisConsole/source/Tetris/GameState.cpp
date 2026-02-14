#include "GameState.h"

#include <fstream>
#include <cassert>

#include "PieceData.h"
#include "Platform.h"

using namespace std;

#define SCORE_FILE (Platform::getDataDir() + "/score.bin")

GameState::GameState()
{
	_bag.push_back(std::make_unique<Tetrimino>(PieceType::O, _matrix));
	_bag.push_back(std::make_unique<Tetrimino>(PieceType::I, _matrix));
	_bag.push_back(std::make_unique<Tetrimino>(PieceType::T, _matrix));
	_bag.push_back(std::make_unique<Tetrimino>(PieceType::L, _matrix));
	_bag.push_back(std::make_unique<Tetrimino>(PieceType::J, _matrix));
	_bag.push_back(std::make_unique<Tetrimino>(PieceType::S, _matrix));
	_bag.push_back(std::make_unique<Tetrimino>(PieceType::Z, _matrix));

	_matrix.resize(TETRIS_HEIGHT);

	loadHighscore();
}

GameState::~GameState() = default;

void GameState::loadHighscore()
{
	if (ifstream highscoreFileRead(SCORE_FILE); highscoreFileRead.is_open()) {
		highscoreFileRead >> _highscore;
		highscoreFileRead.close();
	} else {
		ofstream highscoreFileWrite(SCORE_FILE);
		_highscore = 0;
		highscoreFileWrite << _highscore;
		highscoreFileWrite.close();
	}
}

void GameState::saveHighscore() const {
	ofstream highscoreFile(SCORE_FILE);
	highscoreFile << _highscore;
	highscoreFile.close();
}

Tetrimino* GameState::peekTetrimino() const
{
	assert(_bagIndex < _bag.size());
	return _bag[_bagIndex].get();
}
