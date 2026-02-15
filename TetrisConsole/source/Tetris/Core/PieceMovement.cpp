#include "PieceMovement.h"

#include "GravityPolicy.h"
#include "LockDownPolicy.h"
#include "Timer.h"

using namespace std;

static constexpr auto FALL             = "fall";
static constexpr auto AUTOREPEAT_LEFT  = "autorepeatleft";
static constexpr auto AUTOREPEAT_RIGHT = "autorepeatright";
static constexpr auto LOCK_DOWN        = "lockdown";
static constexpr auto GENERATION       = "generation";

static constexpr double AUTOREPEAT_DELAY = 0.25;
static constexpr double AUTOREPEAT_SPEED = 0.01;
static constexpr double LOCK_DOWN_DELAY  = 0.5;
static constexpr double GENERATION_DELAY = 0.2;

PieceMovement::PieceMovement(Timer& timer, LockDownPolicy* lockDown, GravityPolicy* gravity)
    : _timer(timer), _lockDown(lockDown), _gravity(gravity) {}

void PieceMovement::stepFalling(GameState& state, const InputSnapshot& input) {
	switch (state.flags.stepState) {
		case GameStep::Idle:      stepIdle(state, input); break;
		case GameStep::MoveLeft:  stepMoveLeft(state, input); break;
		case GameStep::MoveRight: stepMoveRight(state, input); break;
		case GameStep::HardDrop:  stepHardDrop(state); break;
	}

	if (state.flags.isGameOver)
		return;

	if (!state.flags.didRotate) {
		if (input.rotateCW)
			rotate(state, RIGHT);
		else if (input.rotateCCW)
			rotate(state, LEFT);
	} else {
		if (!input.rotateCW && !input.rotateCCW)
			state.flags.didRotate = false;
	}
}

void PieceMovement::resetTimers() const {
	_timer.stopTimer(FALL);
	_timer.stopTimer(AUTOREPEAT_LEFT);
	_timer.stopTimer(AUTOREPEAT_RIGHT);
	_timer.stopTimer(LOCK_DOWN);
}

void PieceMovement::fall(GameState& state, const InputSnapshot& input) const {
	if (state.pieces.current == nullptr)
		return;

	const double interval = _gravity->fallInterval(state.stats.level, input.softDrop);
	const bool isSoftDropping = input.softDrop;

	if (_timer.getSeconds(FALL) >= interval) {
		_timer.resetTimer(FALL);
		if (moveDown(state)) {
			if (isSoftDropping)
				state.stats.score++;

			if (state.lockDown.active) {
				if (const int currentLine = state.pieces.current->getPosition().row; currentLine > state.lockDown.lowestLine) {
					state.lockDown.lowestLine = currentLine;
					state.lockDown.moveCount = 0;
					_timer.resetTimer(LOCK_DOWN);
				}
			}
		}
	}

	if (!state.pieces.current->simulateMove(Vector2i(1, 0)) && !_timer.exist(LOCK_DOWN)) {
		_timer.startTimer(LOCK_DOWN);
		state.lockDown.active = true;
	}

	if (_timer.getSeconds(LOCK_DOWN) >= LOCK_DOWN_DELAY) {
		lock(state);
		if (state.flags.isGameOver) return;
	}

	const int limit = _lockDown->moveLimit();
	if (limit >= 0 && state.lockDown.moveCount >= limit) {
		lock(state);
		if (state.flags.isGameOver) return;
	}

	if (input.hardDrop && !state.flags.shouldIgnoreHardDrop) {
		state.queueSound(GameSound::HardDrop);
		state.flags.stepState = GameStep::HardDrop;
		state.flags.shouldIgnoreHardDrop = true;
	} else if (!input.hardDrop && state.flags.shouldIgnoreHardDrop) {
		state.flags.shouldIgnoreHardDrop = false;
	}
}

void PieceMovement::stepIdle(GameState& state, const InputSnapshot& input) {
	fall(state, input);
	if (state.flags.isGameOver) return;

	checkAutorepeat(state, input.left, AUTOREPEAT_LEFT, &PieceMovement::moveLeft, GameStep::MoveLeft);
	checkAutorepeat(state, input.right, AUTOREPEAT_RIGHT, &PieceMovement::moveRight, GameStep::MoveRight);

	if (state.config.holdEnabled && !state.pieces.isNewHold && input.hold) {
		Tetrimino *buffer = state.pieces.hold;
		state.pieces.hold = state.pieces.current;
		state.pieces.current = buffer;
		if (state.pieces.current != nullptr) {
			state.pieces.current->resetRotation();
			if (!state.pieces.current->setPosition(state.pieces.current->getStartingPosition())) {
				state.flags.isGameOver = true;
				return;
			}
			_timer.startTimer(FALL);
			state.markDirty();
		} else {
			state.phase = GamePhase::Generation;
			_timer.resetTimer(GENERATION, GENERATION_DELAY);
		}
		state.pieces.isNewHold = true;
	}
}

void PieceMovement::stepMoveLeft(GameState& state, const InputSnapshot& input) const {
	if (state.pieces.current == nullptr) {
		state.flags.stepState = GameStep::Idle;
		return;
	}

	fall(state, input);
	if (state.flags.isGameOver) return;

	if (!input.left) {
		state.flags.stepState = GameStep::Idle;
		_timer.stopTimer(AUTOREPEAT_LEFT);
	}

	if (_timer.getSeconds(AUTOREPEAT_LEFT) >= AUTOREPEAT_SPEED) {
		moveLeft(state);
		_timer.resetTimer(AUTOREPEAT_LEFT);
	}
}

void PieceMovement::stepMoveRight(GameState& state, const InputSnapshot& input) const {
	if (state.pieces.current == nullptr) {
		state.flags.stepState = GameStep::Idle;
		return;
	}

	fall(state, input);
	if (state.flags.isGameOver) return;

	if (!input.right) {
		state.flags.stepState = GameStep::Idle;
		_timer.stopTimer(AUTOREPEAT_RIGHT);
	}

	if (_timer.getSeconds(AUTOREPEAT_RIGHT) >= AUTOREPEAT_SPEED) {
		moveRight(state);
		_timer.resetTimer(AUTOREPEAT_RIGHT);
	}
}

void PieceMovement::stepHardDrop(GameState& state) const {
	if (state.pieces.current == nullptr) {
		state.flags.stepState = GameStep::Idle;
		return;
	}

	while (moveDown(state)) {
		state.stats.score += 2;
	}
	lock(state);
}

void PieceMovement::incrementMove(GameState& state) {
	state.queueSound(GameSound::Click);
	if (state.lockDown.active)
		state.lockDown.moveCount++;
}

void PieceMovement::resetLockDown(const GameState& state) const {
	if (!_lockDown->resetsTimerOnMove())
		return;

	if (state.lockDown.active) {
		_timer.resetTimer(LOCK_DOWN);
	}
}

void PieceMovement::moveLeft(GameState& state) const {
	if (state.pieces.current == nullptr)
		return;

	if (state.pieces.current->move(Vector2i(0, -1))) {
		state.flags.lastMoveIsTSpin = false;
		state.flags.lastMoveIsMiniTSpin = false;
		incrementMove(state);
		resetLockDown(state);
		state.markDirty();
	}
}

void PieceMovement::moveRight(GameState& state) const {
	if (state.pieces.current == nullptr)
		return;

	if (state.pieces.current->move(Vector2i(0, 1))) {
		state.flags.lastMoveIsTSpin = false;
		state.flags.lastMoveIsMiniTSpin = false;
		incrementMove(state);
		resetLockDown(state);
		state.markDirty();
	}
}

bool PieceMovement::moveDown(GameState& state) {
	if (state.pieces.current->move(Vector2i(1, 0))) {
		state.flags.lastMoveIsTSpin = false;
		state.flags.lastMoveIsMiniTSpin = false;
		state.markDirty();
		return true;
	}

	return false;
}

void PieceMovement::rotate(GameState& state, const DIRECTION direction) const {
	if (state.pieces.current == nullptr)
		return;

	if (state.pieces.current->rotate(direction)) {
		state.flags.didRotate = true;
		state.flags.lastMoveIsTSpin = false;
		state.flags.lastMoveIsMiniTSpin = false;
		incrementMove(state);
		resetLockDown(state);
		state.markDirty();

		if (state.pieces.current->canTSpin()) {
			if (state.pieces.current->checkTSpin())
				state.flags.lastMoveIsTSpin = true;
			else if (state.pieces.current->checkMiniTSpin())
				state.flags.lastMoveIsMiniTSpin = true;
		}
	}
}

void PieceMovement::lock(GameState& state) const {
	if (state.pieces.current == nullptr || state.flags.isGameOver)
		return;

	// False alarm: piece was nudged off its resting surface (e.g. slid over a gap)
	if (state.pieces.current->simulateMove(Vector2i(1, 0))) {
		_timer.stopTimer(LOCK_DOWN);
		state.lockDown.moveCount = 0;
		if (const int row = state.pieces.current->getPosition().row; row > state.lockDown.lowestLine)
			state.lockDown.lowestLine = row;
		return;
	}

	// Lock-out: piece overlaps the buffer zone
	if (!state.pieces.current->lock()) {
		state.flags.isGameOver = true;
		return;
	}

	state.queueSound(GameSound::Lock);

	state.pieces.isNewHold = false;
	state.lockDown.active = false;
	state.lockDown.moveCount = 0;
	state.lockDown.lowestLine = 0;
	state.pieces.current = nullptr;
	state.stats.nbMinos++;

	_timer.stopTimer(LOCK_DOWN);

	state.phase = GamePhase::Pattern;
	state.flags.stepState = GameStep::Idle;
	state.markDirty();
}

void PieceMovement::checkAutorepeat(GameState& state, const bool input, const string& timer, const MoveFunc move, const GameStep nextState) {
	if (input) {
		if (!_timer.exist(timer)) {
			(this->*move)(state);
			_timer.startTimer(timer);
		}

		if (_timer.getSeconds(timer) >= AUTOREPEAT_DELAY) {
			_timer.startTimer(timer);
			(this->*move)(state);
			state.flags.stepState = nextState;
		}
	} else {
		_timer.stopTimer(timer);
	}
}
