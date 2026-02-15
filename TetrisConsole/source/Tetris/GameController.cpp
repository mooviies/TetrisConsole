#include "GameController.h"

#include "GameState.h"
#include "Timer.h"
#include "Constants.h"
#include "Random.h"

using namespace std;

#define FALL "fall"
#define AUTOREPEAT_LEFT "autorepeatleft"
#define AUTOREPEAT_RIGHT "autorepeatright"
#define LOCK_DOWN "lockdown"

#define AUTOREPEAT_DELAY 0.25
#define AUTOREPEAT_SPEED 0.01
#define LOCK_DOWN_DELAY 0.5
#define LOCK_DOWN_SMALL_DELAY 0.2

GameController::GameController(Timer& timer)
    : _timer(timer),
      _lockDownPolicy(std::make_unique<ExtendedLockDown>()),
      _scoringRule(makeDefaultScoringRule()),
      _gravityPolicy(makeDefaultGravityPolicy()) {}

GameController::~GameController() = default;

void GameController::configurePolicies(MODE mode) {
    _lockDownPolicy = makeLockDownPolicy(mode);
}

void GameController::start(GameState& state) const {
    reset(state);
    state.flags.isStarted = true;
}

StepResult GameController::step(GameState& state, const InputSnapshot& input) {
    if (state.shouldExit())
        return StepResult::Continue;

    if (!state.flags.isStarted)
        return StepResult::Continue;

    switch (state.flags.stepState) {
        case GameStep::Idle:      stepIdle(state, input); break;
        case GameStep::MoveLeft:  stepMoveLeft(state, input); break;
        case GameStep::MoveRight: stepMoveRight(state, input); break;
        case GameStep::HardDrop:  stepHardDrop(state); break;
    }

    auto result = StepResult::Continue;

    if (state.flags.isGameOver) {
        result = StepResult::GameOver;
    } else if (input.pause) {
        result = StepResult::PauseRequested;
    } else {
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

    state.updateHighscore();

    return result;
}

void GameController::fall(GameState& state, const InputSnapshot& input) const {
    if (state.pieces.current == nullptr)
        return;

    const double interval = _gravityPolicy->fallInterval(state.stats.level, input.softDrop);
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

    const int limit = _lockDownPolicy->moveLimit();
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

void GameController::stepIdle(GameState& state, const InputSnapshot& input) {
    if (state.pieces.current == nullptr) {
        popTetrimino(state);
        if (!state.pieces.current->setPosition(state.pieces.current->getStartingPosition())) {
            state.flags.isGameOver = true;
            return;
        }
        _timer.startTimer(FALL);
        state.markDirty();
    }

    fall(state, input);
    if (state.flags.isGameOver) return;

    checkAutorepeat(state, input.left, AUTOREPEAT_LEFT, &GameController::moveLeft, GameStep::MoveLeft);
    checkAutorepeat(state, input.right, AUTOREPEAT_RIGHT, &GameController::moveRight, GameStep::MoveRight);

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
        }
        state.pieces.isNewHold = true;
    }
}

void GameController::stepMoveLeft(GameState& state, const InputSnapshot& input) const {
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

void GameController::stepMoveRight(GameState& state, const InputSnapshot& input) const {
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

void GameController::stepHardDrop(GameState& state) const {
    if (state.pieces.current == nullptr) {
        state.flags.stepState = GameStep::Idle;
        return;
    }

    while (moveDown(state)) {
        state.stats.score += 2;
    }
    lock(state);
}

void GameController::incrementMove(GameState& state) {
    state.queueSound(GameSound::Click);
    if (state.lockDown.active)
        state.lockDown.moveCount++;
}

void GameController::resetLockDown(const GameState& state) const {
    if (!_lockDownPolicy->resetsTimerOnMove())
        return;

    if (state.lockDown.active) {
        _timer.resetTimer(LOCK_DOWN);
    }
}

void GameController::moveLeft(GameState& state) const {
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

void GameController::moveRight(GameState& state) const {
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

bool GameController::moveDown(GameState& state) {
    if (state.pieces.current->move(Vector2i(1, 0))) {
        state.flags.lastMoveIsTSpin = false;
        state.flags.lastMoveIsMiniTSpin = false;
        state.markDirty();
        return true;
    }

    return false;
}

void GameController::rotate(GameState& state, const DIRECTION direction) const {
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

void GameController::checkAutorepeat(GameState& state, const bool input, const string &timer, MoveFunc move, GameStep nextState) {
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

void GameController::reset(GameState& state) const {
    state.stats = Stats{};
    state.stats.level = state.config.startingLevel;
    state.lockDown = {};
    state.flags = {};
    state.setPlayerName("");

    for (auto& row : state.matrix) row.fill(0);

    state.pieces.hold = nullptr;
    state.pieces.current = nullptr;
    state.pieces.bagIndex = 0;
    state.pieces.isNewHold = false;
    shuffle(state, 0);
    shuffle(state, 7);

    state.startGameTimer();
    state.activateHighscore();

    _timer.stopTimer(LOCK_DOWN);
    _timer.stopTimer(FALL);
    _timer.stopTimer(AUTOREPEAT_LEFT);
    _timer.stopTimer(AUTOREPEAT_RIGHT);
}

void GameController::lock(GameState& state) const {
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

    const int linesCleared = clearLines(state);
    awardScore(state, linesCleared);

    state.flags.stepState = GameStep::Idle;
    state.markDirty();
}

int GameController::clearLines(GameState& state) {
    int linesCleared = 0;
    for (int i = MATRIX_END; i >= MATRIX_START; i--) {
        bool fullLine = true;
        for (int j = 0; j < TETRIS_WIDTH; j++) {
            if (state.matrix[i][j] == 0) {
                fullLine = false;
                break;
            }
        }

        if (fullLine) {
            linesCleared++;
            state.matrix.erase(state.matrix.begin() + i);
        }
    }

    for (int i = 0; i < linesCleared; i++)
        state.matrix.push_front(MatrixRow{});

    return linesCleared;
}

void GameController::awardScore(GameState& state, const int linesCleared) const {
    if (linesCleared == 4)
        state.stats.tetris++;
    if (state.flags.lastMoveIsTSpin || state.flags.lastMoveIsMiniTSpin)
        state.stats.tSpins++;

    auto result = _scoringRule->compute(linesCleared,
        state.flags.lastMoveIsTSpin, state.flags.lastMoveIsMiniTSpin,
        state.stats.backToBackBonus, state.stats.level);

    state.stats.score += result.points;
    state.stats.backToBackBonus = result.continuesBackToBack;
    state.flags.lastMoveIsTSpin = false;
    state.flags.lastMoveIsMiniTSpin = false;

    // Combo (Ren) tracking: consecutive piece placements that clear lines.
    // currentCombo reaches 1+ on the 2nd consecutive clear (= first real combo).
    if (linesCleared > 0) {
        state.stats.currentCombo++;
        if (state.stats.currentCombo > state.stats.combos)
            state.stats.combos = state.stats.currentCombo;
    } else {
        state.stats.currentCombo = -1;
    }

    state.stats.lines += linesCleared;
    state.stats.goal += result.awardedLines;

    if (state.stats.goal >= state.stats.level * 5) {
        state.stats.level++;
        state.stats.goal = 0;
    }

    if (linesCleared == 4)
        state.queueSound(GameSound::Tetris);
    else if (linesCleared >= 1)
        state.queueSound(GameSound::LineClear);
}

void GameController::shuffle(GameState& state, size_t start) {
    for (int i = 6; i >= 0; i--) {
        int j = Random::getInteger(0, i);
        if (i != j)
            state.pieces.bag[start + static_cast<size_t>(i)].swap(state.pieces.bag[start + static_cast<size_t>(j)]);
    }
}

void GameController::popTetrimino(GameState& state) {
    state.pieces.current = state.pieces.bag[state.pieces.bagIndex++].get();
    if (state.pieces.bagIndex >= 7) {
        std::swap_ranges(state.pieces.bag.begin(), state.pieces.bag.begin() + 7,
                         state.pieces.bag.begin() + 7);
        shuffle(state, 7);
        state.pieces.bagIndex = 0;
    }
}
