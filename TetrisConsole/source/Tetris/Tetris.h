#pragma once

#include "GameState.h"
#include "GameRenderer.h"
#include "GameController.h"

class Menu;

class Tetris
{
public:
	Tetris(Menu& pauseMenu, Menu& gameOverMenu);
	~Tetris();

	void start();
	void step();
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
	[[nodiscard]] int startingLevel() const { return _state.startingLevel(); }
	[[nodiscard]] MODE mode() const { return _state.mode(); }
	[[nodiscard]] const std::map<HighScoreKey, HighScoreRecord>& highscoreMap() const { return _state.highscoreMap(); }
	void setPlayerName(const std::string& n) { _state.setPlayerName(n); }

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
};
