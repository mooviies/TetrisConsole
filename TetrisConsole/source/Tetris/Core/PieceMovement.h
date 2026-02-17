#pragma once

#include <string>

#include "Facing.h"
#include "GameState.h"
#include "InputSnapshot.h"

class GravityPolicy;
class LockDownPolicy;
class Timer;

class PieceMovement {
public:
    PieceMovement(Timer &timer, LockDownPolicy *lockDown, GravityPolicy *gravity);

    void stepFalling(GameState &state, const InputSnapshot &input);
    void resetTimers() const;
    void setLockDownPolicy(LockDownPolicy *p) { _lockDown = p; }

private:
    void fall(GameState &state, const InputSnapshot &input) const;
    void stepIdle(GameState &state, const InputSnapshot &input);
    void stepMoveLeft(GameState &state, const InputSnapshot &input) const;
    void stepMoveRight(GameState &state, const InputSnapshot &input) const;
    void stepHardDrop(GameState &state) const;

    static void incrementMove(GameState &state);
    void resetLockDown(const GameState &state) const;
    void moveLeft(GameState &state) const;
    void moveRight(GameState &state) const;
    [[nodiscard]] static bool moveDown(GameState &state);
    void rotate(GameState &state, Direction direction) const;
    void lock(GameState &state) const;

    using MoveFunc = void (PieceMovement::*)(GameState &) const;
    void checkAutorepeat(GameState &state, bool input, const std::string &timer, MoveFunc move, GameStep nextState);

    Timer &_timer;
    LockDownPolicy *_lockDown;
    GravityPolicy *_gravity;
};
