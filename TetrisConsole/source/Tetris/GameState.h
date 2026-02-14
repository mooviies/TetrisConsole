#pragma once

#include <string>
#include <array>
#include <chrono>
#include <memory>
#include <vector>
#include <cstdint>
#include <map>

#include "Constants.h"
#include "Tetrimino.h"

enum class GameStep { Idle, MoveLeft, MoveRight, HardDrop };
enum class StepResult { Continue, PauseRequested, GameOver };
enum class GameSound { Click, Lock, HardDrop, LineClear, Tetris };

enum MODE {
	EXTENDED,
	EXTENDED_INFINITY,
	CLASSIC
};

struct HighScoreKey {
	int startingLevel;
	MODE mode;
	bool operator<(const HighScoreKey& o) const {
		if (mode != o.mode) return mode < o.mode;
		return startingLevel < o.startingLevel;
	}
};

struct HighScoreRecord {
	int64_t score{};
	int level{};          // level reached (not starting level)
	int lines{};
	int tpm{};
	int lpm{};
	int tetris{};
	int combos{};
	int tSpins{};
	double gameElapsed{}; // seconds
};

class GameState
{
public:
	GameState();
	~GameState();

	void loadHighscore();
	void saveHighscore();
	[[nodiscard]] Tetrimino* peekTetrimino() const;
	[[nodiscard]] std::vector<const Tetrimino*> peekTetriminos(size_t count) const;

	void setShouldExit(bool v) { _shouldExit = v; }
	void setMode(MODE m) { _mode = m; }
	void setStartingLevel(int level);

	[[nodiscard]] const GameMatrix& matrix() const { return _matrix; }
	[[nodiscard]] const Tetrimino* currentTetrimino() const { return _currentTetrimino; }
	[[nodiscard]] const Tetrimino* holdTetrimino() const { return _holdTetrimino; }
	[[nodiscard]] int64_t score() const { return _score; }
	[[nodiscard]] int64_t highscore() const { return _highscore; }
	[[nodiscard]] int level() const { return _level; }
	[[nodiscard]] int tpm() const { return _tpm; }
	[[nodiscard]] int lpm() const { return _lpm; }
	[[nodiscard]] int lines() const { return _lines; }
	[[nodiscard]] int tetris() const { return _tetris; }
	[[nodiscard]] int combos() const { return _combos; }
	[[nodiscard]] int tSpins() const { return _tSpins; }
	[[nodiscard]] bool backToBackBonus() const { return _backToBackBonus; }
	[[nodiscard]] bool hasBetterHighscore() const { return _hasBetterHighscore; }
	[[nodiscard]] const std::map<HighScoreKey, HighScoreRecord>& highscoreMap() const { return _highscoreMap; }
	[[nodiscard]] bool shouldExit() const { return _shouldExit; }
	[[nodiscard]] int startingLevel() const { return _startingLevel; }
	[[nodiscard]] MODE mode() const { return _mode; }

	void markDirty() { _isDirty = true; }
	[[nodiscard]] bool isDirty() const { return _isDirty; }
	void clearDirty() { _isDirty = false; }

	[[nodiscard]] const std::vector<GameSound>& pendingSounds() const { return _pendingSounds; }
	void clearPendingSounds() { _pendingSounds.clear(); }
	[[nodiscard]] bool muteRequested() const { return _muteRequested; }
	void clearMuteRequested() { _muteRequested = false; }

	void startGameTimer();
	void pauseGameTimer();
	void resumeGameTimer();
	[[nodiscard]] double gameElapsed() const;
	[[nodiscard]] int minutesElapsed() const { return static_cast<int>(_gameElapsedAccum / 60); }

	// Tetris Guideline gravity values for levels 1-15 (index 0 unused).
	static constexpr std::array<double, 16> kSpeedNormal = {
		0, 1.0, 0.793, 0.618, 0.473, 0.355, 0.262, 0.190, 0.135, 0.094, 0.064, 0.043, 0.028, 0.018, 0.011, 0.007
	};
	static constexpr std::array<double, 16> kSpeedFast = {
		0, 0.05, 0.03965, 0.0309, 0.02365, 0.01775, 0.0131, 0.0095, 0.00675, 0.0047, 0.0032, 0.00215, 0.0014,
		0.0009, 0.00055, 0.00035
	};

private:
	friend class GameController;

	void updateHighscore();
	void activateHighscore();
	void queueSound(GameSound s) { _pendingSounds.push_back(s); }

	GameMatrix _matrix;

	MODE _mode = EXTENDED;
	size_t _previewSize = 1;

	int _startingLevel = 1;
	int _level{};
	int _tpm{};
	int _lpm{};
	int _lines{};
	int _tetris{};
	int _combos{};
	int _tSpins{};
	int _nbMinos{};
	int _goal{};
	int64_t _score{};
	int64_t _highscore{};
	std::map<HighScoreKey, HighScoreRecord> _highscoreMap;
	int _nbMoveAfterLockDown{};
	int _lowestLine{};

	unsigned int _bagIndex{};
	Tetrimino* _currentTetrimino{};  // non-owning; points into _bag
	Tetrimino* _holdTetrimino{};     // non-owning; points into _bag
	std::vector<std::unique_ptr<Tetrimino>> _bag;

	bool _isStarted{};
	bool _shouldExit{};
	bool _didRotate{};
	bool _isGameOver{};
	bool _shouldIgnoreHardDrop{};
	bool _lastMoveIsTSpin{};
	bool _lastMoveIsMiniTSpin{};
	bool _backToBackBonus{};
	bool _isInLockDown{};
	bool _isNewHold{};
	bool _hasBetterHighscore{};
	bool _isDirty{};
	bool _muteRequested{};

	std::vector<GameSound> _pendingSounds;
	GameStep _stepState = GameStep::Idle;

	std::chrono::steady_clock::time_point _gameTimerStart{};
	double _gameElapsedAccum{};
	bool _gameTimerRunning{};
};
