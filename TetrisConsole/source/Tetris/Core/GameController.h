#pragma once

#include <memory>

#include "GameState.h"
#include "GoalPolicy.h"
#include "InputSnapshot.h"
#include "LineClear.h"
#include "LockDownPolicy.h"
#include "ScoringRule.h"
#include "GravityPolicy.h"
#include "PieceMovement.h"

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
	void stepGeneration(GameState& state) const;
	void stepCompletion(GameState& state) const;

	static void shuffle(GameState& state, size_t start);
	static void popTetrimino(GameState& state);

	Timer& _timer;
	std::unique_ptr<LockDownPolicy> _lockDownPolicy;
	std::unique_ptr<ScoringRule> _scoringRule;
	std::unique_ptr<GravityPolicy> _gravityPolicy;
	std::unique_ptr<GoalPolicy> _goalPolicy;
	PieceMovement _movement;
	LineClear _lineClear;
};
