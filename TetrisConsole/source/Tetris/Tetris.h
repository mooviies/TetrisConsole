#pragma once

#include "GameState.h"
#include "GameRenderer.h"
#include "GameController.h"
#include "InputSnapshot.h"

class Menu;

class Tetris
{
public:
	Tetris(Menu& pauseMenu, Menu& gameOverMenu);
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
	void setMode(const MODE mode) { _state.setMode(mode); }
	void setGhostEnabled(bool v) { _state.setGhostEnabled(v); }
	void setHoldEnabled(bool v) { _state.setHoldEnabled(v); }
	void setPreviewCount(int n) { _state.setPreviewCount(n); }
	[[nodiscard]] int startingLevel() const { return _state.startingLevel(); }
	[[nodiscard]] MODE mode() const { return _state.mode(); }
	[[nodiscard]] bool ghostEnabled() const { return _state.ghostEnabled(); }
	[[nodiscard]] bool holdEnabled() const { return _state.holdEnabled(); }
	[[nodiscard]] int previewCount() const { return _state.previewCount(); }
	[[nodiscard]] const std::vector<HighScoreRecord>& highscores() const { return _state.highscores(); }
	void setPlayerName(const std::string& n) { _state.setPlayerName(n); }
	void saveOptions() const { _state.saveOptions(); }

private:
	void handlePause();
	void handleGameOver();
	void playPendingSounds();
	std::string promptPlayerName();

	GameState _state;
	GameRenderer _renderer;
	GameController _controller;
	Menu& _pauseMenu;
	Menu& _gameOverMenu;
	bool _backToMenu{};
	bool _wasMutePressed{};
};
