#include "GameController.h"

#include "GameState.h"
#include "Timer.h"
#include "Constants.h"
#include "Random.h"
#include "Input.h"

using namespace std;

#define FALL "fall"
#define AUTOREPEAT_LEFT "autorepeatleft"
#define AUTOREPEAT_RIGHT "autorepeatright"
#define LOCK_DOWN "lockdown"

#define AUTOREPEAT_DELAY 0.25
#define AUTOREPEAT_SPEED 0.01
#define LOCK_DOWN_DELAY 0.5
#define LOCK_DOWN_SMALL_DELAY 0.2
#define LOCK_DOWN_MOVE 15

GameController::GameController(Timer& timer)
    : _timer(timer) {}

GameController::~GameController() = default;

void GameController::start(GameState& state) const {
    reset(state);
    state._isStarted = true;
}

StepResult GameController::step(GameState& state) {
    if (state._shouldExit)
        return StepResult::Continue;

    if (!state._isStarted)
        return StepResult::Continue;

    switch (state._stepState) {
        case GameStep::Idle:      stepIdle(state); break;
        case GameStep::MoveLeft:  stepMoveLeft(state); break;
        case GameStep::MoveRight: stepMoveRight(state); break;
        case GameStep::HardDrop:  stepHardDrop(state); break;
    }

    auto result = StepResult::Continue;

    if (state._isGameOver) {
        result = StepResult::GameOver;
    } else if (Input::pause()) {
        result = StepResult::PauseRequested;
    } else {
        if (!state._didRotate) {
            if (Input::rotateClockwise())
                rotate(state, RIGHT);
            else if (Input::rotateCounterClockwise())
                rotate(state, LEFT);
        } else {
            if (!Input::rotateClockwise() && !Input::rotateCounterClockwise())
                state._didRotate = false;
        }
    }

    const bool mutePressed = Input::mute();
    if (mutePressed && !_wasMutePressed)
        state._muteRequested = true;
    _wasMutePressed = mutePressed;

    state.updateHighscore();

    return result;
}

void GameController::fall(GameState& state) const {
    if (state._currentTetrimino == nullptr)
        return;

    int speedIndex = state._level;
    if (speedIndex > 15)
        speedIndex = 15;

    const auto& speedArray = Input::softDrop() ? GameState::kSpeedFast : GameState::kSpeedNormal;
    const bool isSoftDropping = Input::softDrop();

    if (_timer.getSeconds(FALL) >= speedArray[static_cast<size_t>(speedIndex)]) {
        _timer.resetTimer(FALL);
        if (moveDown(state)) {
            if (isSoftDropping)
                state._score++;

            if (state._isInLockDown) {
                if (const int currentLine = state._currentTetrimino->getPosition().row; currentLine > state._lowestLine) {
                    state._lowestLine = currentLine;
                    state._nbMoveAfterLockDown = 0;
                    _timer.resetTimer(LOCK_DOWN);
                }
            }
        }
    }

    if (!state._currentTetrimino->simulateMove(Vector2i(1, 0)) && !_timer.exist(LOCK_DOWN)) {
        _timer.startTimer(LOCK_DOWN);
        state._isInLockDown = true;
    }

    if (_timer.getSeconds(LOCK_DOWN) >= LOCK_DOWN_DELAY) {
        lock(state);
        if (state._isGameOver) return;
    }

    if (state._mode == EXTENDED && state._nbMoveAfterLockDown >= LOCK_DOWN_MOVE) {
        lock(state);
        if (state._isGameOver) return;
    }

    if (Input::hardDrop() && !state._shouldIgnoreHardDrop) {
        state.queueSound(GameSound::HardDrop);
        state._stepState = GameStep::HardDrop;
        state._shouldIgnoreHardDrop = true;
    } else if (!Input::hardDrop() && state._shouldIgnoreHardDrop) {
        state._shouldIgnoreHardDrop = false;
    }
}

void GameController::stepIdle(GameState& state) {
    if (state._currentTetrimino == nullptr) {
        popTetrimino(state);
        if (!state._currentTetrimino->setPosition(state._currentTetrimino->getStartingPosition())) {
            state._isGameOver = true;
            return;
        }
        _timer.startTimer(FALL);
        state.markDirty();
    }

    fall(state);
    if (state._isGameOver) return;

    checkAutorepeat(state, Input::left(), AUTOREPEAT_LEFT, &GameController::moveLeft, GameStep::MoveLeft);
    checkAutorepeat(state, Input::right(), AUTOREPEAT_RIGHT, &GameController::moveRight, GameStep::MoveRight);

    if (!state._isNewHold && Input::hold()) {
        Tetrimino *buffer = state._holdTetrimino;
        state._holdTetrimino = state._currentTetrimino;
        state._currentTetrimino = buffer;
        if (state._currentTetrimino != nullptr) {
            state._currentTetrimino->resetRotation();
            if (!state._currentTetrimino->setPosition(state._currentTetrimino->getStartingPosition())) {
                state._isGameOver = true;
                return;
            }
            _timer.startTimer(FALL);
            state.markDirty();
        }
        state._isNewHold = true;
    }
}

void GameController::stepMoveLeft(GameState& state) const {
    if (state._currentTetrimino == nullptr) {
        state._stepState = GameStep::Idle;
        return;
    }

    fall(state);
    if (state._isGameOver) return;

    if (!Input::left()) {
        state._stepState = GameStep::Idle;
        _timer.stopTimer(AUTOREPEAT_LEFT);
    }

    if (_timer.getSeconds(AUTOREPEAT_LEFT) >= AUTOREPEAT_SPEED) {
        moveLeft(state);
        _timer.resetTimer(AUTOREPEAT_LEFT);
    }
}

void GameController::stepMoveRight(GameState& state) const {
    if (state._currentTetrimino == nullptr) {
        state._stepState = GameStep::Idle;
        return;
    }

    fall(state);
    if (state._isGameOver) return;

    if (!Input::right()) {
        state._stepState = GameStep::Idle;
        _timer.stopTimer(AUTOREPEAT_RIGHT);
    }

    if (_timer.getSeconds(AUTOREPEAT_RIGHT) >= AUTOREPEAT_SPEED) {
        moveRight(state);
        _timer.resetTimer(AUTOREPEAT_RIGHT);
    }
}

void GameController::stepHardDrop(GameState& state) const {
    if (state._currentTetrimino == nullptr) {
        state._stepState = GameStep::Idle;
        return;
    }

    while (moveDown(state)) {
        state._score += 2;
    }
    lock(state);
}

void GameController::incrementMove(GameState& state) {
    state.queueSound(GameSound::Click);
    if (state._isInLockDown)
        state._nbMoveAfterLockDown++;
}

void GameController::resetLockDown(const GameState& state) const {
    if (state._mode == CLASSIC)
        return;

    if (state._isInLockDown) {
        _timer.resetTimer(LOCK_DOWN);
    }
}

void GameController::moveLeft(GameState& state) const {
    if (state._currentTetrimino == nullptr)
        return;

    if (state._currentTetrimino->move(Vector2i(0, -1))) {
        state._lastMoveIsTSpin = false;
        state._lastMoveIsMiniTSpin = false;
        incrementMove(state);
        resetLockDown(state);
        state.markDirty();
    }
}

void GameController::moveRight(GameState& state) const {
    if (state._currentTetrimino == nullptr)
        return;

    if (state._currentTetrimino->move(Vector2i(0, 1))) {
        state._lastMoveIsTSpin = false;
        state._lastMoveIsMiniTSpin = false;
        incrementMove(state);
        resetLockDown(state);
        state.markDirty();
    }
}

bool GameController::moveDown(GameState& state) {
    if (state._currentTetrimino->move(Vector2i(1, 0))) {
        state._lastMoveIsTSpin = false;
        state._lastMoveIsMiniTSpin = false;
        state.markDirty();
        return true;
    }

    return false;
}

void GameController::rotate(GameState& state, const DIRECTION direction) const {
    if (state._currentTetrimino == nullptr)
        return;

    if (state._currentTetrimino->rotate(direction)) {
        state._didRotate = true;
        state._lastMoveIsTSpin = false;
        state._lastMoveIsMiniTSpin = false;
        incrementMove(state);
        resetLockDown(state);
        state.markDirty();

        if (state._currentTetrimino->canTSpin()) {
            if (state._currentTetrimino->checkTSpin())
                state._lastMoveIsTSpin = true;
            else if (state._currentTetrimino->checkMiniTSpin())
                state._lastMoveIsMiniTSpin = true;
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
            state._stepState = nextState;
        }
    } else {
        _timer.stopTimer(timer);
    }
}

void GameController::reset(GameState& state) const {
    state._level = state._startingLevel;
    state._lines = 0;
    state._goal = 0;
    state._score = 0;
    state._tSpins = 0;
    state._combos = 0;
    state._currentCombo = -1;
    state._tetris = 0;
    state._nbMinos = 0;
    state._shouldIgnoreHardDrop = false;
    state._lastMoveIsTSpin = false;
    state._lastMoveIsMiniTSpin = false;
    state._backToBackBonus = false;
    state._isInLockDown = false;
    state._isNewHold = false;
    state._hasBetterHighscore = false;
    state._isGameOver = false;
    state._playerName.clear();

    for (auto& row : state._matrix) row.fill(0);

    state._holdTetrimino = nullptr;
    state._currentTetrimino = nullptr;
    state._bagIndex = 0;
    shuffle(state, 0);
    shuffle(state, 7);

    state._stepState = GameStep::Idle;
    state._didRotate = false;
    state._nbMoveAfterLockDown = 0;
    state._lowestLine = 0;
    state.startGameTimer();
    state.activateHighscore();

    _timer.stopTimer(LOCK_DOWN);
    _timer.stopTimer(FALL);
    _timer.stopTimer(AUTOREPEAT_LEFT);
    _timer.stopTimer(AUTOREPEAT_RIGHT);
}

void GameController::lock(GameState& state) const {
    if (state._currentTetrimino == nullptr || state._isGameOver)
        return;

    // False alarm: piece was nudged off its resting surface (e.g. slid over a gap)
    if (state._currentTetrimino->simulateMove(Vector2i(1, 0))) {
        _timer.stopTimer(LOCK_DOWN);
        state._nbMoveAfterLockDown = 0;
        if (const int row = state._currentTetrimino->getPosition().row; row > state._lowestLine)
            state._lowestLine = row;
        return;
    }

    // Lock-out: piece overlaps the buffer zone
    if (!state._currentTetrimino->lock()) {
        state._isGameOver = true;
        return;
    }

    state.queueSound(GameSound::Lock);

    state._isNewHold = false;
    state._isInLockDown = false;
    state._nbMoveAfterLockDown = 0;
    state._lowestLine = 0;
    state._currentTetrimino = nullptr;
    state._nbMinos++;

    _timer.stopTimer(LOCK_DOWN);

    const int linesCleared = clearLines(state);
    awardScore(state, linesCleared);

    state._stepState = GameStep::Idle;
    state.markDirty();
}

int GameController::clearLines(GameState& state) {
    int linesCleared = 0;
    for (int i = MATRIX_END; i >= MATRIX_START; i--) {
        bool fullLine = true;
        for (int j = 0; j < TETRIS_WIDTH; j++) {
            if (state._matrix[i][j] == 0) {
                fullLine = false;
                break;
            }
        }

        if (fullLine) {
            linesCleared++;
            state._matrix.erase(state._matrix.begin() + i);
        }
    }

    for (int i = 0; i < linesCleared; i++)
        state._matrix.push_front(MatrixRow{});

    return linesCleared;
}

void GameController::awardScore(GameState& state, const int linesCleared) {
    int awardedLines = linesCleared;

    if (linesCleared == 4)
        state._tetris++;

    if (state._lastMoveIsTSpin) {
        state._tSpins++;
        int value = 0;
        switch (linesCleared) {
            case 0:  value = 400;  awardedLines = 4;  break;
            case 1:  value = 800;  awardedLines = 8;  break;
            case 2:  value = 1200; awardedLines = 12; break;
            case 3:  value = 1600; awardedLines = 16; break;
            default: ;
        }

        if (linesCleared >= 1) {
            if (state._backToBackBonus) {
                value += value / 2;
                awardedLines += (linesCleared + 1) / 2;
            }
            state._backToBackBonus = true;
        }

        state._score += value * state._level;
    } else if (state._lastMoveIsMiniTSpin) {
        state._tSpins++;
        int value = 0;
        if (linesCleared == 1) {
            value = 200;
            awardedLines = 2;
            if (state._backToBackBonus) {
                value += value / 2;
                awardedLines += (linesCleared + 1) / 2;
            }
            state._backToBackBonus = true;
        } else {
            value = 100;
            awardedLines = 1;
        }

        state._score += value * state._level;
    } else {
        int value = 0;
        switch (linesCleared) {
            case 1:
                value = 100;
                state._backToBackBonus = false;
                break;
            case 2:
                value = 300;
                awardedLines = 3;
                state._backToBackBonus = false;
                break;
            case 3:
                value = 500;
                awardedLines = 5;
                state._backToBackBonus = false;
                break;
            case 4:
                value = 800;
                awardedLines = 8;
                if (state._backToBackBonus) {
                    value += value / 2;
                    awardedLines += (linesCleared + 1) / 2;
                }
                state._backToBackBonus = true;
                break;
            default: ;
        }

        state._score += value * state._level;
    }

    state._lastMoveIsTSpin = false;
    state._lastMoveIsMiniTSpin = false;

    // Combo (Ren) tracking: consecutive piece placements that clear lines.
    // _currentCombo reaches 1+ on the 2nd consecutive clear (= first real combo).
    if (linesCleared > 0) {
        state._currentCombo++;
        if (state._currentCombo > state._combos)
            state._combos = state._currentCombo;
    } else {
        state._currentCombo = -1;
    }

    state._lines += linesCleared;
    state._goal += awardedLines;

    if (state._goal >= state._level * 5) {
        state._level++;
        state._goal = 0;
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
            state._bag[start + static_cast<size_t>(i)].swap(state._bag[start + static_cast<size_t>(j)]);
    }
}

void GameController::popTetrimino(GameState& state) {
    state._currentTetrimino = state._bag[state._bagIndex++].get();
    if (state._bagIndex >= 7) {
        std::swap_ranges(state._bag.begin(), state._bag.begin() + 7,
                         state._bag.begin() + 7);
        shuffle(state, 7);
        state._bagIndex = 0;
    }
}
