#pragma once

#include "GameState.h"
#include "GameRenderer.h"
#include "GameController.h"
#include "Utility.h"

class Tetris
{
public:
	Tetris(Menu& pauseMenu, Menu& gameOverMenu);
	~Tetris();

	void start()  { _controller.start(_state); }
	void step()   { _controller.step(_state); }
	void redraw() { _renderer.invalidate(); Utility::showTitle("A classic in console!"); _renderer.render(_state); }
	void exit()   { _state.setShouldExit(true); }
	[[nodiscard]] bool doExit() const { return _state.shouldExit(); }
	void setStartingLevel(const int level) { _state.setStartingLevel(level); }
	void setMode(const MODE mode) { _state.setMode(mode); }

private:
	GameState _state;
	GameRenderer _renderer;
	GameController _controller;
};
