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
	void lock(GameState& state) const;
	static void shuffle(GameState& state, size_t start);
	static void popTetrimino(GameState& state);

	void fall(GameState& state) const;
	void stepIdle(GameState& state);
	void stepMoveLeft(GameState& state);
	void stepMoveRight(GameState& state);
	void stepHardDrop(GameState& state);
	static void incrementMove(GameState& state);

	void smallResetLockDown(const GameState& state) const;
	void moveLeft(GameState& state) const;
	void moveRight(GameState& state) const;
	[[nodiscard]] static bool moveDown(GameState& state);
	void rotate(GameState& state, DIRECTION direction) const;

	using MoveFunc = void(GameController::*)(GameState&) const;
	void checkAutorepeat(GameState& state, bool input, const std::string& timer, MoveFunc move, GameStep nextState);

	Timer& _timer;
	bool _wasMutePressed{};
};
