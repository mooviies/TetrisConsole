#pragma once

#include <string>
#include <chrono>
#include <memory>
#include <vector>
#include <cstdint>

#include "Constants.h"
#include "GameTypes.h"
#include "Tetrimino.h"

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
	MODE mode{MODE::EXTENDED};
	bool ghostEnabled{true};
	bool holdEnabled{true};
	int previewCount{NEXT_PIECE_QUEUE_SIZE};
};

struct GameConfig {
	MODE mode = MODE::EXTENDED;
	bool ghostEnabled = true;
	bool holdEnabled = true;
	int previewCount = NEXT_PIECE_QUEUE_SIZE;
	int startingLevel = 1;
};

struct Stats {
	int64_t score{};
	int level{};
	int lines{};
	int goal{};
	int tetris{};
	int combos{};
	int currentCombo{-1};
	int tSpins{};
	int nbMinos{};
	bool backToBackBonus{};
	int64_t highscore{};
	bool hasBetterHighscore{};
};

struct LockDownState {
	bool active{};
	int moveCount{};
	int lowestLine{};
};

struct PieceState {
	std::vector<std::unique_ptr<Tetrimino>> bag;
	unsigned int bagIndex{};
	Tetrimino* current{};
	Tetrimino* hold{};
	bool isNewHold{};
};

struct FrameFlags {
	GameStep stepState = GameStep::Idle;
	bool didRotate{};
	bool lastMoveIsTSpin{};
	bool lastMoveIsMiniTSpin{};
	bool isGameOver{};
	bool isStarted{};
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

	void setShouldExit(const bool v) { _shouldExit = v; }
	void setStartingLevel(int level);
	void setPlayerName(const std::string& n) { _playerName = n; }

	[[nodiscard]] int tpm() const;
	[[nodiscard]] int lpm() const;
	[[nodiscard]] const std::vector<HighScoreRecord>& highscores() const { return _highscores; }
	[[nodiscard]] bool shouldExit() const { return _shouldExit; }

	void markDirty() { _isDirty = true; }
	[[nodiscard]] bool isDirty() const { return _isDirty; }
	void clearDirty() { _isDirty = false; }

	[[nodiscard]] const std::vector<GameSound>& pendingSounds() const { return _pendingSounds; }
	void clearPendingSounds() { _pendingSounds.clear(); }

	void startGameTimer();
	void pauseGameTimer();
	void resumeGameTimer();
	[[nodiscard]] double gameElapsed() const;
	[[nodiscard]] int minutesElapsed() const { return static_cast<int>(gameElapsed() / 60); }

	void updateHighscore();
	void activateHighscore();
	void queueSound(const GameSound s) { _pendingSounds.push_back(s); }

	// Public sub-structs
	GameConfig config;
	Stats stats;
	LockDownState lockDown;
	PieceState pieces;
	FrameFlags flags;
	GameMatrix matrix;
	GamePhase phase = GamePhase::Falling;
	LineClearState lineClear;

private:
	bool _isDirty{};
	bool _shouldExit{};
	std::string _playerName;
	std::vector<HighScoreRecord> _highscores; // sorted by score desc, max 10
	std::vector<GameSound> _pendingSounds;

	std::chrono::steady_clock::time_point _gameTimerStart{};
	double _gameElapsedAccum{};
	bool _gameTimerRunning{};
};
