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
	void setStartingLevel(const int level) { _state.setStartingLevel(level); }
	void setMode(const MODE mode) { _state.setMode(mode); }

private:
	void handlePause();
	void handleGameOver();
	void playPendingSounds();

	GameState _state;
	GameRenderer _renderer;
	GameController _controller;
	Menu& _pauseMenu;
	Menu& _gameOverMenu;
};
