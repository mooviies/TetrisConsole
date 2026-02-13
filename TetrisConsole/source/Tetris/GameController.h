#pragma once

#include <string>

#include "Facing.h"
#include "GameState.h"

class Timer;
class Menu;
class GameRenderer;

class GameController
{
public:
	GameController(Timer& timer, Menu& pauseMenu, Menu& gameOverMenu, GameRenderer& renderer);
	~GameController();

	void start(GameState& state);
	void step(GameState& state);

	void setStartingLevel(GameState& state, int level);
	void setMode(GameState& state, int mode);

private:
	void reset(GameState& state);
	void lock(GameState& state);
	void shuffle(GameState& state);
	void popTetrimino(GameState& state);
	void gameOver(GameState& state);
	void fall(GameState& state);

	void stepIdle(GameState& state);
	void stepMoveLeft(GameState& state);
	void stepMoveRight(GameState& state);
	void stepHardDrop(GameState& state);
	void incrementMove(GameState& state);

	void smallResetLockDown(const GameState& state) const;
	void moveLeft(GameState& state);
	void moveRight(GameState& state);
	[[nodiscard]] bool moveDown(GameState& state);
	void rotate(GameState& state, DIRECTION direction);

	using MoveFunc = void(GameController::*)(GameState&);
	void checkAutorepeat(GameState& state, bool input, const std::string& timer, MoveFunc move, GameStep nextState);

	Timer& _timer;
	Menu& _pauseMenu;
	Menu& _gameOverMenu;
	GameRenderer& _renderer;
};
