#pragma once

#include <string>
#include <array>
#include <chrono>
#include <memory>
#include <vector>
#include <cstdint>

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
	std::string name;
	// Options used during this game
	int startingLevel{1};
	MODE mode{EXTENDED};
	bool ghostEnabled{true};
	bool holdEnabled{true};
	int previewCount{NEXT_PIECE_QUEUE_SIZE};
};

class GameState
{
public:
	GameState();
	~GameState();

	void loadHighscore();
	void saveHighscore();
	void loadOptions();
	void saveOptions() const;
	[[nodiscard]] Tetrimino* peekTetrimino() const;
	[[nodiscard]] std::vector<const Tetrimino*> peekTetriminos(size_t count) const;

	void setShouldExit(bool v) { _shouldExit = v; }
	void setMode(MODE m) { _mode = m; }
	void setStartingLevel(int level);
	void setPlayerName(const std::string& n) { _playerName = n; }
	void setGhostEnabled(bool v) { _ghostEnabled = v; }
	void setHoldEnabled(bool v) { _holdEnabled = v; }
	void setPreviewCount(int n) { _previewCount = n; }

	[[nodiscard]] const GameMatrix& matrix() const { return _matrix; }
	[[nodiscard]] const Tetrimino* currentTetrimino() const { return _currentTetrimino; }
	[[nodiscard]] const Tetrimino* holdTetrimino() const { return _holdTetrimino; }
	[[nodiscard]] int64_t score() const { return _score; }
	[[nodiscard]] int64_t highscore() const { return _highscore; }
	[[nodiscard]] int level() const { return _level; }
	[[nodiscard]] int tpm() const;
	[[nodiscard]] int lpm() const;
	[[nodiscard]] int lines() const { return _lines; }
	[[nodiscard]] int tetris() const { return _tetris; }
	[[nodiscard]] int combos() const { return _combos; }
	[[nodiscard]] int tSpins() const { return _tSpins; }
	[[nodiscard]] bool backToBackBonus() const { return _backToBackBonus; }
	[[nodiscard]] bool hasBetterHighscore() const { return _hasBetterHighscore; }
	[[nodiscard]] const std::vector<HighScoreRecord>& highscores() const { return _highscores; }
	[[nodiscard]] bool shouldExit() const { return _shouldExit; }
	[[nodiscard]] int startingLevel() const { return _startingLevel; }
	[[nodiscard]] MODE mode() const { return _mode; }
	[[nodiscard]] bool ghostEnabled() const { return _ghostEnabled; }
	[[nodiscard]] bool holdEnabled() const { return _holdEnabled; }
	[[nodiscard]] int previewCount() const { return _previewCount; }

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
	[[nodiscard]] int minutesElapsed() const { return static_cast<int>(gameElapsed() / 60); }

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
	bool _ghostEnabled = true;
	bool _holdEnabled = true;
	int _previewCount = NEXT_PIECE_QUEUE_SIZE;

	int _startingLevel = 1;
	int _level{};
	int _lines{};
	int _tetris{};
	int _combos{};       // best combo (Ren) achieved
	int _currentCombo{}; // running consecutive-clear count (-1 = no chain)
	int _tSpins{};
	int _nbMinos{};
	int _goal{};
	int64_t _score{};
	int64_t _highscore{};
	std::vector<HighScoreRecord> _highscores; // sorted by score desc, max 10
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
	std::string _playerName;

	std::vector<GameSound> _pendingSounds;
	GameStep _stepState = GameStep::Idle;

	std::chrono::steady_clock::time_point _gameTimerStart{};
	double _gameElapsedAccum{};
	bool _gameTimerRunning{};
};
