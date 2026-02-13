#pragma once

#include "GameState.h"
#include "GameRenderer.h"
#include "GameController.h"
#include "Timer.h"
#include "Menu.h"

class Tetris
{
public:
	Tetris(Menu& pauseMenu, Menu& gameOverMenu);
	~Tetris();

	void start()  { _controller.start(_state); }
	void step()   { _controller.step(_state); }
	void exit()   { _state._shouldExit = true; }
	[[nodiscard]] bool doExit() const { return _state._shouldExit; }
	void setStartingLevel(int level) { _controller.setStartingLevel(_state, level); }
	void setMode(MODE mode) { _state._mode = mode; }

private:
	GameState _state;
	GameRenderer _renderer;
	GameController _controller;
};
