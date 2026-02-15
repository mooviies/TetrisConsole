#pragma once

#include <memory>
#include <string>

#include "Facing.h"
#include "GameState.h"
#include "InputSnapshot.h"
#include "LockDownPolicy.h"
#include "ScoringRule.h"
#include "GravityPolicy.h"

class Timer;

class GameController
{
public:
	explicit GameController(Timer& timer);
	~GameController();

	void start(GameState& state) const;
	StepResult step(GameState& state, const InputSnapshot& input);
	void reset(GameState& state) const;
	void configurePolicies(MODE mode);

private:
	// Phase dispatchers
	void stepGeneration(GameState& state);
	void stepFalling(GameState& state, const InputSnapshot& input);
	void stepPattern(GameState& state);
	void stepAnimate(GameState& state);
	void stepEliminate(GameState& state);
	void stepCompletion(GameState& state);

	static std::vector<int> detectFullRows(const GameState& state);
	static void eliminateRows(GameState& state, const std::vector<int>& rows);

	void lock(GameState& state) const;
	void awardScore(GameState& state, int linesCleared) const;
	static void shuffle(GameState& state, size_t start);
	static void popTetrimino(GameState& state);

	void fall(GameState& state, const InputSnapshot& input) const;
	void stepIdle(GameState& state, const InputSnapshot& input);
	void stepMoveLeft(GameState& state, const InputSnapshot& input) const;
	void stepMoveRight(GameState& state, const InputSnapshot& input) const;
	void stepHardDrop(GameState& state) const;
	static void incrementMove(GameState& state);

	void resetLockDown(const GameState& state) const;
	void moveLeft(GameState& state) const;
	void moveRight(GameState& state) const;
	[[nodiscard]] static bool moveDown(GameState& state);
	void rotate(GameState& state, DIRECTION direction) const;

	using MoveFunc = void(GameController::*)(GameState&) const;
	void checkAutorepeat(GameState& state, bool input, const std::string& timer, MoveFunc move, GameStep nextState);

	Timer& _timer;
	std::unique_ptr<LockDownPolicy> _lockDownPolicy;
	std::unique_ptr<ScoringRule> _scoringRule;
	std::unique_ptr<GravityPolicy> _gravityPolicy;
};
