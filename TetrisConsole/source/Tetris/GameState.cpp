#include "GameState.h"

#include <algorithm>
#include <fstream>
#include <cassert>
#include <cstring>

#include "PieceData.h"
#include "Platform.h"

using namespace std;

static constexpr uint32_t kMagic   = 0x53484354; // "TCHS" little-endian
static constexpr uint32_t kVersion = 2;
static constexpr size_t   kRecordSize = 68;
static constexpr size_t   kRecordSizeV1 = 52;

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
	_highscoreMap.clear();

	ifstream in(SCORE_FILE, ios::binary);
	if (!in.is_open()) {
		activateHighscore();
		return;
	}

	uint32_t magic = 0;
	in.read(reinterpret_cast<char*>(&magic), 4);

	if (magic == kMagic) {
		// Binary format
		uint32_t version = 0;
		uint32_t count   = 0;
		in.read(reinterpret_cast<char*>(&version), 4);
		in.read(reinterpret_cast<char*>(&count), 4);

		const size_t recSize = (version >= 2) ? kRecordSize : kRecordSizeV1;

		for (uint32_t i = 0; i < count; i++) {
			char buf[kRecordSize]{};
			in.read(buf, static_cast<streamsize>(recSize));
			if (!in) break;

			HighScoreKey key{};
			HighScoreRecord rec{};

			int32_t lvl = 0, md = 0;
			memcpy(&lvl,           buf + 0,  4);
			memcpy(&md,            buf + 4,  4);
			memcpy(&rec.score,     buf + 8,  8);
			int32_t tmp = 0;
			memcpy(&tmp,           buf + 16, 4); rec.level   = tmp;
			memcpy(&tmp,           buf + 20, 4); rec.lines   = tmp;
			memcpy(&tmp,           buf + 24, 4); rec.tpm     = tmp;
			memcpy(&tmp,           buf + 28, 4); rec.lpm     = tmp;
			memcpy(&tmp,           buf + 32, 4); rec.tetris  = tmp;
			memcpy(&tmp,           buf + 36, 4); rec.combos  = tmp;
			memcpy(&tmp,           buf + 40, 4); rec.tSpins  = tmp;
			memcpy(&rec.gameElapsed, buf + 44, 8);

			if (version >= 2) {
				char nameBuf[17]{};
				memcpy(nameBuf, buf + 52, 16);
				rec.name = nameBuf;
			}

			key.startingLevel = lvl;
			key.mode          = static_cast<MODE>(md);
			_highscoreMap[key] = rec;
		}
	} else {
		// Migration: old text format (plain int64_t)
		in.close();
		ifstream text(SCORE_FILE);
		int64_t oldScore = 0;
		if (text >> oldScore && oldScore > 0) {
			HighScoreKey key{1, EXTENDED};
			HighScoreRecord rec{};
			rec.score = oldScore;
			_highscoreMap[key] = rec;
		}
		text.close();
		saveHighscore(); // rewrite in new format
		activateHighscore();
		return;
	}

	in.close();
	activateHighscore();
}

void GameState::saveHighscore() {
	if (_hasBetterHighscore) {
		HighScoreKey key{_startingLevel, _mode};
		HighScoreRecord& rec = _highscoreMap[key];
		rec.score       = _score;
		rec.level       = _level;
		rec.lines       = _lines;
		rec.tpm         = _tpm;
		rec.lpm         = _lpm;
		rec.tetris      = _tetris;
		rec.combos      = _combos;
		rec.tSpins      = _tSpins;
		rec.gameElapsed = gameElapsed();
		rec.name        = _playerName;
	}

	ofstream out(SCORE_FILE, ios::binary);
	if (!out.is_open()) return;

	uint32_t magic   = kMagic;
	uint32_t version = kVersion;
	auto     count   = static_cast<uint32_t>(_highscoreMap.size());

	out.write(reinterpret_cast<const char*>(&magic),   4);
	out.write(reinterpret_cast<const char*>(&version), 4);
	out.write(reinterpret_cast<const char*>(&count),   4);

	for (const auto& [key, rec] : _highscoreMap) {
		char buf[kRecordSize]{};
		auto lvl = static_cast<int32_t>(key.startingLevel);
		auto md  = static_cast<int32_t>(key.mode);

		memcpy(buf + 0,  &lvl,             4);
		memcpy(buf + 4,  &md,              4);
		memcpy(buf + 8,  &rec.score,       8);
		int32_t tmp;
		tmp = static_cast<int32_t>(rec.level);   memcpy(buf + 16, &tmp, 4);
		tmp = static_cast<int32_t>(rec.lines);   memcpy(buf + 20, &tmp, 4);
		tmp = static_cast<int32_t>(rec.tpm);     memcpy(buf + 24, &tmp, 4);
		tmp = static_cast<int32_t>(rec.lpm);     memcpy(buf + 28, &tmp, 4);
		tmp = static_cast<int32_t>(rec.tetris);  memcpy(buf + 32, &tmp, 4);
		tmp = static_cast<int32_t>(rec.combos);  memcpy(buf + 36, &tmp, 4);
		tmp = static_cast<int32_t>(rec.tSpins);  memcpy(buf + 40, &tmp, 4);
		memcpy(buf + 44, &rec.gameElapsed, 8);

		// Name field (16 bytes, null-padded; buf is zero-initialized)
		memcpy(buf + 52, rec.name.c_str(),
		       min(rec.name.size(), static_cast<size_t>(16)));

		out.write(buf, static_cast<streamsize>(kRecordSize));
	}

	out.close();
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

void GameState::activateHighscore() {
	HighScoreKey key{_startingLevel, _mode};
	auto it = _highscoreMap.find(key);
	_highscore = (it != _highscoreMap.end()) ? it->second.score : 0;
}

void GameState::setStartingLevel(int level) {
	_startingLevel = std::clamp(level, 1, 15);
}

void GameState::startGameTimer() {
	_gameElapsedAccum = 0;
	_gameTimerStart = chrono::steady_clock::now();
	_gameTimerRunning = true;
}

void GameState::pauseGameTimer() {
	if (_gameTimerRunning) {
		_gameElapsedAccum += chrono::duration<double>(
			chrono::steady_clock::now() - _gameTimerStart).count();
		_gameTimerRunning = false;
	}
}

void GameState::resumeGameTimer() {
	if (!_gameTimerRunning) {
		_gameTimerStart = chrono::steady_clock::now();
		_gameTimerRunning = true;
	}
}

double GameState::gameElapsed() const {
	double total = _gameElapsedAccum;
	if (_gameTimerRunning)
		total += chrono::duration<double>(
			chrono::steady_clock::now() - _gameTimerStart).count();
	return total;
}
