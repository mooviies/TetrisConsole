#include "PieceMovement.h"

#include "GravityPolicy.h"
#include "LockDownPolicy.h"
#include "Timer.h"

using namespace std;

static constexpr auto kFall = "fall";
static constexpr auto kAutorepeatLeft = "autorepeatleft";
static constexpr auto kAutorepeatRight = "autorepeatright";
static constexpr auto kLockDown = "lockdown";
static constexpr auto kGeneration = "generation";
static constexpr auto kHardDropTrail = "harddroptrail";
static constexpr double kAutorepeatDelay = 0.25;
static constexpr double kAutorepeatSpeed = 0.01;
static constexpr double kLockDownDelay = 0.5;
static constexpr double kGenerationDelay = 0.2;
static constexpr int kSoftDropScore = 1;
static constexpr int kHardDropScore = 2;

PieceMovement::PieceMovement(Timer &timer, LockDownPolicy *lockDown, GravityPolicy *gravity)
    : _timer(timer), _lockDown(lockDown), _gravity(gravity) {
}

void PieceMovement::stepFalling(GameState &state, const InputSnapshot &input) {
    switch (state.flags.stepState) {
        case GameStep::Idle: stepIdle(state, input); break;
        case GameStep::MoveLeft: stepMoveLeft(state, input); break;
        case GameStep::MoveRight: stepMoveRight(state, input); break;
        case GameStep::HardDrop: stepIdle(state, {}); break;
    }

    if (state.flags.isGameOver || state.flags.stepState == GameStep::HardDrop) return;

    if (!state.flags.didRotate) {
        if (input.rotateCW)
            rotate(state, Direction::Right);
        else if (input.rotateCCW)
            rotate(state, Direction::Left);
    } else {
        if (!input.rotateCW && !input.rotateCCW) state.flags.didRotate = false;
    }
}

void PieceMovement::resetTimers() const {
    _timer.stopTimer(kFall);
    _timer.stopTimer(kAutorepeatLeft);
    _timer.stopTimer(kAutorepeatRight);
    _timer.stopTimer(kLockDown);
    _timer.stopTimer(kHardDropTrail);
}

void PieceMovement::fall(GameState &state, const InputSnapshot &input) const {
    if (state.pieces.current == nullptr) return;

    if (input.hardDrop && state.flags.stepState != GameStep::HardDrop) {
        state.queueSound(GameSound::HardDrop);
        state.flags.stepState = GameStep::HardDrop;

        const int startRow = state.pieces.current->getPosition().row;
        const int color = state.pieces.current->getColor();
        bool columns[BOARD_WIDTH]{};
        for (int i = 0; i < BOARD_WIDTH; i++)
            columns[i] = state.pieces.current->isMino(startRow, i)
                      || state.pieces.current->isMino(startRow + 1, i)
                      || state.pieces.current->isMino(startRow + 2, i)
                      || state.pieces.current->isMino(startRow + 3, i);

        while (moveDown(state)) {
            state.stats.score += kHardDropScore;
        }

        const int endRow = state.pieces.current->getPosition().row;
        if (endRow > startRow) {
            auto &trail = state.hardDropTrail;
            trail.startRow = startRow;
            trail.endRow = endRow;
            trail.visibleStartRow = startRow;
            trail.color = color;
            trail.active = true;
            for (int i = 0; i < BOARD_WIDTH; i++)
                trail.columns[i] = columns[i];
            _timer.resetTimer(kHardDropTrail);
        }

        lock(state);
        return;
    }

    const DropType dropType = input.softDrop ? DropType::Soft : DropType::Normal;

    if (const double interval = _gravity->fallInterval(state.stats.level, dropType);
        _timer.getSeconds(kFall) >= interval) {
        _timer.resetTimer(kFall);
        if (moveDown(state)) {
            if (dropType == DropType::Soft)
                state.stats.score += kSoftDropScore;

            if (state.lockDown.active) {
                if (const int currentLine = state.pieces.current->getPosition().row;
                    currentLine > state.lockDown.lowestLine) {
                    state.lockDown.lowestLine = currentLine;
                    state.lockDown.moveCount = 0;
                    _timer.resetTimer(kLockDown);
                }
            }
        }
    }

    if (!state.pieces.current->simulateMove(Vector2i(1, 0)) && !_timer.exist(kLockDown)) {
        _timer.startTimer(kLockDown);
        state.lockDown.active = true;
    }

    if (_timer.getSeconds(kLockDown) >= kLockDownDelay) {
        lock(state);
        if (state.flags.isGameOver) return;
    }

    if (const int limit = _lockDown->moveLimit(); limit >= 0 && state.lockDown.moveCount >= limit) {
        lock(state);
    }
}

void PieceMovement::stepIdle(GameState &state, const InputSnapshot &input) {
    fall(state, input);
    if (state.flags.isGameOver) return;

    checkAutorepeat(state, input.left, kAutorepeatLeft, &PieceMovement::moveLeft, GameStep::MoveLeft);
    checkAutorepeat(state, input.right, kAutorepeatRight, &PieceMovement::moveRight, GameStep::MoveRight);

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
            _timer.startTimer(kFall);
            state.markDirty();
        } else {
            state.phase = GamePhase::Generation;
            _timer.resetTimer(kGeneration, kGenerationDelay);
        }
        state.pieces.isNewHold = true;
    }
}

void PieceMovement::stepMoveLeft(GameState &state, const InputSnapshot &input) const {
    if (state.pieces.current == nullptr) {
        state.flags.stepState = GameStep::Idle;
        return;
    }

    fall(state, input);
    if (state.flags.isGameOver) return;

    if (!input.left) {
        state.flags.stepState = GameStep::Idle;
        _timer.stopTimer(kAutorepeatLeft);
    }

    if (_timer.getSeconds(kAutorepeatLeft) >= kAutorepeatSpeed) {
        moveLeft(state);
        _timer.resetTimer(kAutorepeatLeft);
    }
}

void PieceMovement::stepMoveRight(GameState &state, const InputSnapshot &input) const {
    if (state.pieces.current == nullptr) {
        state.flags.stepState = GameStep::Idle;
        return;
    }

    fall(state, input);
    if (state.flags.isGameOver) return;

    if (!input.right) {
        state.flags.stepState = GameStep::Idle;
        _timer.stopTimer(kAutorepeatRight);
    }

    if (_timer.getSeconds(kAutorepeatRight) >= kAutorepeatSpeed) {
        moveRight(state);
        _timer.resetTimer(kAutorepeatRight);
    }
}

void PieceMovement::stepHardDrop(GameState &state) const {
    if (state.pieces.current == nullptr) {
        state.flags.stepState = GameStep::Idle;
        return;
    }

    while (moveDown(state)) {
        state.stats.score += 2;
    }
    lock(state);
}

void PieceMovement::incrementMove(GameState &state) {
    state.queueSound(GameSound::Click);
    if (state.lockDown.active) state.lockDown.moveCount++;
}

void PieceMovement::resetLockDown(const GameState &state) const {
    if (!_lockDown->resetsTimerOnMove()) return;

    if (state.lockDown.active) {
        _timer.resetTimer(kLockDown);
    }
}

void PieceMovement::moveLeft(GameState &state) const {
    if (state.pieces.current == nullptr) return;

    if (state.flags.stepState == GameStep::HardDrop) return;

    if (state.pieces.current->move(Vector2i(0, -1))) {
        state.flags.lastMoveIsTSpin = false;
        state.flags.lastMoveIsMiniTSpin = false;
        incrementMove(state);
        resetLockDown(state);
        state.markDirty();
    }
}

void PieceMovement::moveRight(GameState &state) const {
    if (state.pieces.current == nullptr) return;

    if (state.pieces.current->move(Vector2i(0, 1))) {
        state.flags.lastMoveIsTSpin = false;
        state.flags.lastMoveIsMiniTSpin = false;
        incrementMove(state);
        resetLockDown(state);
        state.markDirty();
    }
}

bool PieceMovement::moveDown(GameState &state) {
    if (state.pieces.current->move(Vector2i(1, 0))) {
        state.flags.lastMoveIsTSpin = false;
        state.flags.lastMoveIsMiniTSpin = false;
        state.markDirty();
        return true;
    }

    return false;
}

void PieceMovement::rotate(GameState &state, const Direction direction) const {
    if (state.pieces.current == nullptr) return;

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

void PieceMovement::lock(GameState &state) const {
    if (state.pieces.current == nullptr || state.flags.isGameOver) return;

    // False alarm: the piece was nudged off its resting surface (e.g. slid over a gap)
    if (state.pieces.current->simulateMove(Vector2i(1, 0))) {
        _timer.stopTimer(kLockDown);
        state.lockDown.moveCount = 0;
        if (const int row = state.pieces.current->getPosition().row; row > state.lockDown.lowestLine)
            state.lockDown.lowestLine = row;
        return;
    }

    // Lock-out: the piece overlaps the buffer zone
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

    _timer.stopTimer(kLockDown);

    state.phase = GamePhase::Pattern;
    state.flags.stepState = GameStep::Idle;
    state.markDirty();
}

void PieceMovement::checkAutorepeat(GameState &state, const bool input, const string &timer, const MoveFunc move,
                                    const GameStep nextState) {
    if (input) {
        if (!_timer.exist(timer)) {
            (this->*move)(state);
            _timer.startTimer(timer);
        }

        if (_timer.getSeconds(timer) >= kAutorepeatDelay) {
            _timer.startTimer(timer);
            (this->*move)(state);
            state.flags.stepState = nextState;
        }
    } else {
        _timer.stopTimer(timer);
    }
}
