#include "GameState.h"

#include <algorithm>
#include <fstream>
#include <cassert>

#include "PieceData.h"
#include "Platform.h"

using namespace std;

#define SCORE_FILE (Platform::getDataDir() + "/score.bin")

GameState::GameState()
{
	for (int batch = 0; batch < 2; batch++) {
		_bag.push_back(std::make_unique<Tetrimino>(PieceType::O, _matrix));
		_bag.push_back(std::make_unique<Tetrimino>(PieceType::I, _matrix));
		_bag.push_back(std::make_unique<Tetrimino>(PieceType::T, _matrix));
		_bag.push_back(std::make_unique<Tetrimino>(PieceType::L, _matrix));
		_bag.push_back(std::make_unique<Tetrimino>(PieceType::J, _matrix));
		_bag.push_back(std::make_unique<Tetrimino>(PieceType::S, _matrix));
		_bag.push_back(std::make_unique<Tetrimino>(PieceType::Z, _matrix));
	}

	_matrix.resize(TETRIS_HEIGHT);
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

vector<const Tetrimino*> GameState::peekTetriminos(size_t count) const
{
	vector<const Tetrimino*> result;
	result.reserve(count);
	for (size_t i = 0; i < count && _bagIndex + i < _bag.size(); i++)
		result.push_back(_bag[_bagIndex + i].get());
	return result;
}

void GameState::updateHighscore() {
	if (_score > _highscore)
		_hasBetterHighscore = true;
	if (_hasBetterHighscore)
		_highscore = _score;
}

void GameState::setStartingLevel(int level) {
	_startingLevel = std::clamp(level, 1, 15);
}
