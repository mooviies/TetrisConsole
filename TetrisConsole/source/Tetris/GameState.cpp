#include "GameState.h"

#include <algorithm>
#include <fstream>
#include <cassert>
#include <cstring>

#include "PieceData.h"
#include "Platform.h"

using namespace std;

static constexpr uint32_t kMagic   = 0x53484354; // "TCHS" little-endian
static constexpr uint32_t kVersion = 3;
static constexpr size_t   kRecordSize = 80;
static constexpr size_t   kMaxHighscores = 10;

#define SCORE_FILE (Platform::getDataDir() + "/score.bin")

static constexpr uint32_t kOptMagic   = 0x54434F50; // "PCOT" little-endian
static constexpr uint32_t kOptVersion = 1;

#define OPTIONS_FILE (Platform::getDataDir() + "/options.bin")

GameState::GameState()
{
	for (int batch = 0; batch < 2; batch++) {
		pieces.bag.push_back(std::make_unique<Tetrimino>(PieceType::O, matrix));
		pieces.bag.push_back(std::make_unique<Tetrimino>(PieceType::I, matrix));
		pieces.bag.push_back(std::make_unique<Tetrimino>(PieceType::T, matrix));
		pieces.bag.push_back(std::make_unique<Tetrimino>(PieceType::L, matrix));
		pieces.bag.push_back(std::make_unique<Tetrimino>(PieceType::J, matrix));
		pieces.bag.push_back(std::make_unique<Tetrimino>(PieceType::S, matrix));
		pieces.bag.push_back(std::make_unique<Tetrimino>(PieceType::Z, matrix));
	}

	matrix.resize(TETRIS_HEIGHT);
}

GameState::~GameState() = default;

void GameState::loadHighscore()
{
	_highscores.clear();

	ifstream in(SCORE_FILE, ios::binary);
	if (!in.is_open()) {
		activateHighscore();
		return;
	}

	uint32_t magic = 0, version = 0, count = 0;
	in.read(reinterpret_cast<char*>(&magic), 4);
	in.read(reinterpret_cast<char*>(&version), 4);
	in.read(reinterpret_cast<char*>(&count), 4);

	if (!in || magic != kMagic || version != kVersion) {
		in.close();
		activateHighscore();
		return;
	}

	for (uint32_t i = 0; i < count; i++) {
		char buf[kRecordSize]{};
		in.read(buf, static_cast<streamsize>(kRecordSize));
		if (!in) break;

		HighScoreRecord rec{};
		int32_t tmp = 0;
		memcpy(&rec.score,       buf + 0,  8);
		memcpy(&tmp, buf + 8,  4); rec.level  = tmp;
		memcpy(&tmp, buf + 12, 4); rec.lines  = tmp;
		memcpy(&tmp, buf + 16, 4); rec.tpm    = tmp;
		memcpy(&tmp, buf + 20, 4); rec.lpm    = tmp;
		memcpy(&tmp, buf + 24, 4); rec.tetris = tmp;
		memcpy(&tmp, buf + 28, 4); rec.combos = tmp;
		memcpy(&tmp, buf + 32, 4); rec.tSpins = tmp;
		memcpy(&rec.gameElapsed, buf + 36, 8);

		char nameBuf[17]{};
		memcpy(nameBuf, buf + 44, 16);
		rec.name = nameBuf;

		memcpy(&tmp, buf + 60, 4); rec.startingLevel = tmp;
		memcpy(&tmp, buf + 64, 4); rec.mode = static_cast<MODE>(tmp);
		memcpy(&tmp, buf + 68, 4); rec.ghostEnabled = (tmp != 0);
		memcpy(&tmp, buf + 72, 4); rec.holdEnabled  = (tmp != 0);
		memcpy(&tmp, buf + 76, 4); rec.previewCount = tmp;

		_highscores.push_back(rec);
	}

	in.close();

	// Ensure sorted descending by score, cap at 10
	sort(_highscores.begin(), _highscores.end(),
	     [](const HighScoreRecord& a, const HighScoreRecord& b) {
		     return a.score > b.score;
	     });
	if (_highscores.size() > kMaxHighscores)
		_highscores.resize(kMaxHighscores);

	activateHighscore();
}

void GameState::saveHighscore() {
	if (stats.hasBetterHighscore) {
		HighScoreRecord rec{};
		rec.score         = stats.score;
		rec.level         = stats.level;
		rec.lines         = stats.lines;
		rec.tpm           = tpm();
		rec.lpm           = lpm();
		rec.tetris        = stats.tetris;
		rec.combos        = stats.combos;
		rec.tSpins        = stats.tSpins;
		rec.gameElapsed   = gameElapsed();
		rec.name          = _playerName;
		rec.startingLevel = config.startingLevel;
		rec.mode          = config.mode;
		rec.ghostEnabled  = config.ghostEnabled;
		rec.holdEnabled   = config.holdEnabled;
		rec.previewCount  = config.previewCount;

		// Insert maintaining sorted order (descending by score)
		auto it = lower_bound(_highscores.begin(), _highscores.end(), rec,
		                      [](const HighScoreRecord& a, const HighScoreRecord& b) {
			                      return a.score > b.score;
		                      });
		_highscores.insert(it, rec);
		if (_highscores.size() > kMaxHighscores)
			_highscores.resize(kMaxHighscores);
	}

	ofstream out(SCORE_FILE, ios::binary);
	if (!out.is_open()) return;

	uint32_t magic   = kMagic;
	uint32_t version = kVersion;
	auto     count   = static_cast<uint32_t>(_highscores.size());

	out.write(reinterpret_cast<const char*>(&magic),   4);
	out.write(reinterpret_cast<const char*>(&version), 4);
	out.write(reinterpret_cast<const char*>(&count),   4);

	for (const auto& rec : _highscores) {
		char buf[kRecordSize]{};
		int32_t tmp;

		memcpy(buf + 0,  &rec.score, 8);
		tmp = static_cast<int32_t>(rec.level);  memcpy(buf + 8,  &tmp, 4);
		tmp = static_cast<int32_t>(rec.lines);  memcpy(buf + 12, &tmp, 4);
		tmp = static_cast<int32_t>(rec.tpm);    memcpy(buf + 16, &tmp, 4);
		tmp = static_cast<int32_t>(rec.lpm);    memcpy(buf + 20, &tmp, 4);
		tmp = static_cast<int32_t>(rec.tetris); memcpy(buf + 24, &tmp, 4);
		tmp = static_cast<int32_t>(rec.combos); memcpy(buf + 28, &tmp, 4);
		tmp = static_cast<int32_t>(rec.tSpins); memcpy(buf + 32, &tmp, 4);
		memcpy(buf + 36, &rec.gameElapsed, 8);

		// Name field (16 bytes, null-padded; buf is zero-initialized)
		memcpy(buf + 44, rec.name.c_str(),
		       min(rec.name.size(), static_cast<size_t>(16)));

		tmp = static_cast<int32_t>(rec.startingLevel); memcpy(buf + 60, &tmp, 4);
		tmp = static_cast<int32_t>(rec.mode);          memcpy(buf + 64, &tmp, 4);
		tmp = rec.ghostEnabled ? 1 : 0;               memcpy(buf + 68, &tmp, 4);
		tmp = rec.holdEnabled  ? 1 : 0;               memcpy(buf + 72, &tmp, 4);
		tmp = static_cast<int32_t>(rec.previewCount);  memcpy(buf + 76, &tmp, 4);

		out.write(buf, static_cast<streamsize>(kRecordSize));
	}

	out.close();
}

Tetrimino* GameState::peekTetrimino() const
{
	assert(pieces.bagIndex < pieces.bag.size());
	return pieces.bag[pieces.bagIndex].get();
}

vector<const Tetrimino*> GameState::peekTetriminos(size_t count) const
{
	vector<const Tetrimino*> result;
	result.reserve(count);
	for (size_t i = 0; i < count && pieces.bagIndex + i < pieces.bag.size(); i++)
		result.push_back(pieces.bag[pieces.bagIndex + i].get());
	return result;
}

int GameState::tpm() const {
	const double minutes = gameElapsed() / 60.0;
	return (minutes > 0.0) ? static_cast<int>(stats.nbMinos / minutes) : 0;
}

int GameState::lpm() const {
	const double minutes = gameElapsed() / 60.0;
	return (minutes > 0.0) ? static_cast<int>(stats.lines / minutes) : 0;
}

void GameState::updateHighscore() {
	if (stats.score > stats.highscore)
		stats.hasBetterHighscore = true;
	if (stats.hasBetterHighscore)
		stats.highscore = stats.score;
}

void GameState::activateHighscore() {
	stats.highscore = (_highscores.size() >= kMaxHighscores)
		? _highscores[kMaxHighscores - 1].score : 0;
}

void GameState::loadOptions() {
	ifstream in(OPTIONS_FILE, ios::binary);
	if (!in.is_open()) return;

	uint32_t magic = 0, version = 0;
	in.read(reinterpret_cast<char*>(&magic), 4);
	in.read(reinterpret_cast<char*>(&version), 4);
	if (!in || magic != kOptMagic || version < 1) return;

	int32_t val = 0;
	in.read(reinterpret_cast<char*>(&val), 4);
	if (in) config.startingLevel = clamp(static_cast<int>(val), 1, 15);

	in.read(reinterpret_cast<char*>(&val), 4);
	if (in) config.mode = static_cast<MODE>(clamp(static_cast<int>(val), 0, 2));

	in.read(reinterpret_cast<char*>(&val), 4);
	if (in) config.ghostEnabled = (val != 0);

	in.read(reinterpret_cast<char*>(&val), 4);
	if (in) config.holdEnabled = (val != 0);

	in.read(reinterpret_cast<char*>(&val), 4);
	if (in) config.previewCount = clamp(static_cast<int>(val), 0, NEXT_PIECE_QUEUE_SIZE);
}

void GameState::saveOptions() const {
	ofstream out(OPTIONS_FILE, ios::binary);
	if (!out.is_open()) return;

	uint32_t magic   = kOptMagic;
	uint32_t version = kOptVersion;
	out.write(reinterpret_cast<const char*>(&magic), 4);
	out.write(reinterpret_cast<const char*>(&version), 4);

	auto write32 = [&](int32_t v) {
		out.write(reinterpret_cast<const char*>(&v), 4);
	};
	write32(static_cast<int32_t>(config.startingLevel));
	write32(static_cast<int32_t>(config.mode));
	write32(config.ghostEnabled ? 1 : 0);
	write32(config.holdEnabled ? 1 : 0);
	write32(static_cast<int32_t>(config.previewCount));
}

void GameState::setStartingLevel(int level) {
	config.startingLevel = std::clamp(level, 1, 15);
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
