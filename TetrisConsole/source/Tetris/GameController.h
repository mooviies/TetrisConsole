#pragma once

#include <string>

#include "Facing.h"
#include "GameState.h"

class Timer;

class GameController
{
public:
	explicit GameController(Timer& timer);
	~GameController();

	void start(GameState& state);
	StepResult step(GameState& state);
	void reset(GameState& state);

private:
	void lock(GameState& state);
	void shuffle(GameState& state);
	void popTetrimino(GameState& state);

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
	bool _wasMutePressed{};
};
