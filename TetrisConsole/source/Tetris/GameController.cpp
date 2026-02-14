#include "GameController.h"

#include <algorithm>

#include "GameState.h"
#include "GameRenderer.h"
#include "Timer.h"
#include "Menu.h"
#include "Constants.h"
#include "Random.h"
#include "Input.h"
#include "SoundEngine.h"

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

GameController::GameController(Timer& timer, Menu& pauseMenu, Menu& gameOverMenu, GameRenderer& renderer)
    : _timer(timer), _pauseMenu(pauseMenu), _gameOverMenu(gameOverMenu), _renderer(renderer) {}

GameController::~GameController() = default;

void GameController::start(GameState& state) {
    reset(state);
    state._isStarted = true;
}

void GameController::step(GameState& state) {
    if (state._shouldExit)
        return;

    if (!state._isStarted)
        return;

    switch (state._stepState) {
        case GameStep::Idle:      stepIdle(state); break;
        case GameStep::MoveLeft:  stepMoveLeft(state); break;
        case GameStep::MoveRight: stepMoveRight(state); break;
        case GameStep::HardDrop:  stepHardDrop(state); break;
    }

    if (Input::pause()) {
        SoundEngine::pauseMusic();
        _renderer.render(state, false);
        if (const OptionChoice choices = _pauseMenu.open(false, true); choices.options[choices.selected] == "Restart") {
            SoundEngine::stopMusic();
            reset(state);
            return;
        }
        _renderer.invalidate();
        _renderer.render(state);
        SoundEngine::unpauseMusic();
    }

    {
        static bool wasMutePressed = false;
        bool mutePressed = Input::mute();
        if (mutePressed && !wasMutePressed)
            SoundEngine::cycleMute();
        wasMutePressed = mutePressed;
    }

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

void GameController::setStartingLevel(GameState& state, int level) {
    state._startingLevel = std::clamp(level, 1, 15);
}

void GameController::setMode(GameState& state, int mode) {
    state._mode = static_cast<MODE>(mode);
}

void GameController::fall(GameState& state) {
    if (state._currentTetrimino == nullptr)
        return;

    int speedIndex = state._level;
    if (speedIndex > 15)
        speedIndex = 15;

    const auto& speedArray = Input::softDrop() ? GameState::kSpeedFast : GameState::kSpeedNormal;
    bool isSoftDropping = Input::softDrop();

    if (_timer.getSeconds(FALL) >= speedArray[static_cast<size_t>(speedIndex)]) {
        _timer.resetTimer(FALL);
        if (moveDown(state)) {
            if (isSoftDropping)
                state._score++;

            if (state._isInLockDown) {
                int currentLine = state._currentTetrimino->getPosition().row;
                if (currentLine > state._lowestLine) {
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
    }

    if ((state._mode == EXTENDED) && (state._nbMoveAfterLockDown >= LOCK_DOWN_MOVE))
        lock(state);

    if (Input::hardDrop() && !state._shouldIgnoreHardDrop) {
        SoundEngine::playSound("HARD_DROP");
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
            gameOver(state);
            return;
        }
        _timer.startTimer(FALL);
        _renderer.render(state);
    }

    fall(state);

    checkAutorepeat(state, Input::left(), AUTOREPEAT_LEFT, &GameController::moveLeft, GameStep::MoveLeft);
    checkAutorepeat(state, Input::right(), AUTOREPEAT_RIGHT, &GameController::moveRight, GameStep::MoveRight);

    if (!state._isNewHold && Input::hold()) {
        Tetrimino *buffer = state._holdTetrimino;
        state._holdTetrimino = state._currentTetrimino;
        state._currentTetrimino = buffer;
        if (state._currentTetrimino != nullptr) {
            state._currentTetrimino->resetRotation();
            if (!state._currentTetrimino->setPosition(state._currentTetrimino->getStartingPosition())) {
                gameOver(state);
                return;
            }
            _timer.startTimer(FALL);
            _renderer.render(state);
        }
        state._isNewHold = true;
    }
}

void GameController::stepMoveLeft(GameState& state) {
    if (state._currentTetrimino == nullptr) {
        state._stepState = GameStep::Idle;
        return;
    }

    fall(state);

    if (!Input::left()) {
        state._stepState = GameStep::Idle;
        _timer.stopTimer(AUTOREPEAT_LEFT);
    }

    if (_timer.getSeconds(AUTOREPEAT_LEFT) >= AUTOREPEAT_SPEED) {
        moveLeft(state);
        _timer.resetTimer(AUTOREPEAT_LEFT);
    }
}

void GameController::stepMoveRight(GameState& state) {
    if (state._currentTetrimino == nullptr) {
        state._stepState = GameStep::Idle;
        return;
    }

    fall(state);

    if (!Input::right()) {
        state._stepState = GameStep::Idle;
        _timer.stopTimer(AUTOREPEAT_RIGHT);
    }

    if (_timer.getSeconds(AUTOREPEAT_RIGHT) >= AUTOREPEAT_SPEED) {
        moveRight(state);
        _timer.resetTimer(AUTOREPEAT_RIGHT);
    }
}

void GameController::stepHardDrop(GameState& state) {
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
    SoundEngine::playSound("CLICK");
    if (state._isInLockDown)
        state._nbMoveAfterLockDown++;
}

void GameController::smallResetLockDown(const GameState& state) const {
    if (state._mode == CLASSIC)
        return;

    if (_timer.exist(LOCK_DOWN)) {
        if (_timer.getSeconds(LOCK_DOWN) >= LOCK_DOWN_DELAY - LOCK_DOWN_SMALL_DELAY) {
            _timer.resetTimer(LOCK_DOWN, LOCK_DOWN_DELAY - LOCK_DOWN_SMALL_DELAY);
        }
    }
}

void GameController::moveLeft(GameState& state) {
    if (state._currentTetrimino == nullptr)
        return;

    if (state._currentTetrimino->move(Vector2i(0, -1))) {
        state._lastMoveIsTSpin = false;
        state._lastMoveIsMiniTSpin = false;
        incrementMove(state);
        smallResetLockDown(state);
        _renderer.render(state);
    }
}

void GameController::moveRight(GameState& state) {
    if (state._currentTetrimino == nullptr)
        return;

    if (state._currentTetrimino->move(Vector2i(0, 1))) {
        state._lastMoveIsTSpin = false;
        state._lastMoveIsMiniTSpin = false;
        incrementMove(state);
        smallResetLockDown(state);
        _renderer.render(state);
    }
}

bool GameController::moveDown(GameState& state) {
    if (state._currentTetrimino == nullptr)
        return false;

    if (state._currentTetrimino->move(Vector2i(1, 0))) {
        state._lastMoveIsTSpin = false;
        state._lastMoveIsMiniTSpin = false;
        _renderer.render(state);
        return true;
    }

    return false;
}

void GameController::rotate(GameState& state, const DIRECTION direction) {
    if (state._currentTetrimino == nullptr)
        return;

    if (state._currentTetrimino->rotate(direction)) {
        state._didRotate = true;
        state._lastMoveIsTSpin = false;
        state._lastMoveIsMiniTSpin = false;
        incrementMove(state);
        smallResetLockDown(state);
        _renderer.render(state);

        if (state._currentTetrimino->canTSpin()) {
            if (state._currentTetrimino->checkTSpin()) {
                state._score += 400 * state._level;
                state._lastMoveIsTSpin = true;
            } else if (state._currentTetrimino->checkMiniTSpin()) {
                state._score += 100 * state._level;
                state._lastMoveIsMiniTSpin = true;
            }
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

void GameController::reset(GameState& state) {
    state._level = state._startingLevel;
    state._lines = 0;
    state._goal = 0;
    state._score = 0;
    state._shouldIgnoreHardDrop = false;
    state._lastMoveIsTSpin = false;
    state._lastMoveIsMiniTSpin = false;
    state._backToBackBonus = false;
    state._isInLockDown = false;
    state._isNewHold = false;
    state._hasBetterHighscore = false;

    for (auto& row : state._matrix) row.fill(0);

    state._holdTetrimino = nullptr;
    state._currentTetrimino = nullptr;
    state._bagIndex = 0;
    shuffle(state);

    state._stepState = GameStep::Idle;
    state._didRotate = false;
    state._nbMoveAfterLockDown = 0;
    state._lowestLine = 0;

    _timer.stopTimer(LOCK_DOWN);
    _timer.stopTimer(FALL);
    _timer.stopTimer(AUTOREPEAT_LEFT);
    _timer.stopTimer(AUTOREPEAT_RIGHT);

    _renderer.invalidate();
    _renderer.render(state);

    SoundEngine::playMusic("A");
}

void GameController::lock(GameState& state) {
    if (state._currentTetrimino == nullptr)
        return;

    if (state._currentTetrimino->simulateMove(Vector2i(1, 0))) {
        _timer.stopTimer(LOCK_DOWN);
        state._nbMoveAfterLockDown = 0;
        int row = state._currentTetrimino->getPosition().row;
        if (row > state._lowestLine)
            state._lowestLine = row;
        return;
    }

    if (!state._currentTetrimino->lock()) {
        gameOver(state);
        return;
    }

    SoundEngine::playSound("LOCK");

    state._isNewHold = false;
    state._isInLockDown = false;
    state._nbMoveAfterLockDown = 0;
    state._lowestLine = 0;
    _timer.stopTimer(LOCK_DOWN);
    state._currentTetrimino = nullptr;

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

    int awardedLines = linesCleared;

    // Scoring per Tetris Guideline
    if (state._lastMoveIsTSpin) {
        if (linesCleared >= 1) {
            int value = 0;
            switch (linesCleared) {
                case 1:
                    value = 400;
                    awardedLines = 8;
                    break;
                case 2:
                    value = 800;
                    awardedLines = 12;
                    break;
                case 3:
                    value = 1200;
                    awardedLines = 16;
                    break;
                default: ;
            }

            if (state._backToBackBonus) {
                value += value / 2;
                awardedLines += linesCleared / 2;
            }

            state._score += value * state._level;
        } else
            awardedLines = 4;
    } else if (state._lastMoveIsMiniTSpin) {
        if (linesCleared == 1) {
            int value = 100;
            awardedLines = 2;
            if (state._backToBackBonus) {
                value += value / 2;
                awardedLines += linesCleared / 2;
            }

            state._score += value * state._level;
        } else
            awardedLines = 1;
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
                    awardedLines += linesCleared / 2;
                }

                state._backToBackBonus = true;
                break;
            default: ;
        }

        state._score += value * state._level;
    }

    state._lastMoveIsTSpin = false;
    state._lastMoveIsMiniTSpin = false;

    state._lines += awardedLines;
    state._goal += awardedLines;

    if (state._goal >= state._level * 5) {
        state._level++;
        state._goal = 0;
    }

    if (linesCleared == 4)
        SoundEngine::playSound("TETRIS");
    else if (linesCleared >= 1)
        SoundEngine::playSound("LINE_CLEAR");

    state._stepState = GameStep::Idle;
    _renderer.render(state);
}

void GameController::shuffle(GameState& state) {
    for (int i = static_cast<int>(state._bag.size()) - 1; i >= 0; i--) {
        int j = Random::getInteger(0, i);
        if (i == j)
            continue;

        state._bag[static_cast<size_t>(i)].swap(state._bag[static_cast<size_t>(j)]);
    }
    state._bagIndex = 0;
}

void GameController::popTetrimino(GameState& state) {
    state._currentTetrimino = state._bag[state._bagIndex++].get();
    if (state._bagIndex >= state._bag.size()) {
        shuffle(state);
    }
}

void GameController::gameOver(GameState& state) {
    SoundEngine::stopMusic();

    state.saveHighscore();

    _gameOverMenu.open(state._hasBetterHighscore);
    reset(state);
}
