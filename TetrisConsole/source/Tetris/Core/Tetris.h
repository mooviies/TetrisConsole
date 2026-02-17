#pragma once

#include "GameState.h"
#include "GameRenderer.h"
#include "GameController.h"

class HighScoreDisplay;
class Menu;

class Tetris
{
public:
	Tetris(Menu& pauseMenu, Menu& gameOverMenu, HighScoreDisplay& highScoreDisplay);
	~Tetris();

	void start();
	void step(const InputSnapshot& input);
	void render();
	void redraw();
	void pauseGameTimer() { _state.pauseGameTimer(); }
	void resumeGameTimer() { _state.resumeGameTimer(); }
	void exit()   { _state.setShouldExit(true); }
	[[nodiscard]] bool doExit() const { return _state.shouldExit(); }
	[[nodiscard]] bool backToMenu() const { return _backToMenu; }
	void clearBackToMenu() { _backToMenu = false; }
	void setStartingLevel(const int level) { _state.setStartingLevel(level); }
	void setLockDownMode(const LOCKDOWN_MODE mode) { _state.config.mode = mode; }
	void setVariant(const GameVariant variant) { _state.config.variant = variant; }
	void setGhostEnabled(const bool v) { _state.config.ghostEnabled = v; }
	void setHoldEnabled(const bool v) { _state.config.holdEnabled = v; }
	void setPreviewCount(const int n) { _state.config.previewCount = n; }
	[[nodiscard]] int startingLevel() const { return _state.config.startingLevel; }
	[[nodiscard]] LOCKDOWN_MODE mode() const { return _state.config.mode; }
	[[nodiscard]] GameVariant variant() const { return _state.config.variant; }
	[[nodiscard]] bool ghostEnabled() const { return _state.config.ghostEnabled; }
	[[nodiscard]] bool holdEnabled() const { return _state.config.holdEnabled; }
	[[nodiscard]] int previewCount() const { return _state.config.previewCount; }
	[[nodiscard]] const std::vector<HighScoreRecord>& highscores() const { return _state.highscores(); }
	[[nodiscard]] const HighScoreTable& allHighscores() const { return _state.allHighscores(); }
	void setPlayerName(const std::string& n) { _state.setPlayerName(n); }
	void saveOptions() const { _state.saveOptions(); }

private:
	void handlePause();
	void handleGameOver();
	void playPendingSounds();

	GameState _state;
	GameRenderer _renderer;
	GameController _controller;
	Menu& _pauseMenu;
	Menu& _gameOverMenu;
	HighScoreDisplay& _highScoreDisplay;
	bool _backToMenu{};
	bool _wasPausePressed{};
	bool _wasMutePressed{};
};
