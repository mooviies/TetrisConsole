#include "GameController.h"

#include "Timer.h"
#include "Random.h"

using namespace std;

static constexpr auto kGeneration = "generation";

static constexpr double kGenerationDelay = 0.2;

GameController::GameController(Timer &timer)
    : _timer(timer), _lockDownPolicy(makeLockDownPolicy()), _scoringRule(makeDefaultScoringRule()),
      _gravityPolicy(makeDefaultGravityPolicy()), _goalPolicy(makeDefaultGoalPolicy()), _variantRule(makeVariantRule()),
      _movement(timer, _lockDownPolicy.get(), _gravityPolicy.get()),
      _lineClear(timer, _scoringRule.get(), _goalPolicy.get(), _variantRule.get()) {
}

GameController::~GameController() = default;

void GameController::configurePolicies(const LockDownMode mode) {
    _lockDownPolicy = makeLockDownPolicy(mode);
    _movement.setLockDownPolicy(_lockDownPolicy.get());
}

void GameController::configureVariant(const GameVariant variant, GameState &state) {
    _variantRule = makeVariantRule(variant);
    _lineClear.setVariantRule(_variantRule.get());
    state.config.timeLimit = _variantRule->timeLimit();
    state.config.showGoal = _variantRule->levelUp();
}

void GameController::start(GameState &state) const {
    reset(state);
    state.flags.isStarted = true;
    state.phase = GamePhase::Generation;
    _timer.resetTimer(kGeneration, kGenerationDelay);
}

StepResult GameController::step(GameState &state, const InputSnapshot &input) {
    if (state.shouldExit()) return StepResult::Continue;

    if (!state.flags.isStarted) return StepResult::Continue;

    if (state.hardDropTrail.active) {
        constexpr double kTrailDuration = 0.15;
        const double elapsed = _timer.getSeconds("harddroptrail");
        if (elapsed >= kTrailDuration) {
            state.hardDropTrail.active = false;
            state.markDirty();
        } else {
            const double t = elapsed / kTrailDuration;
            const int total = state.hardDropTrail.endRow - state.hardDropTrail.startRow;
            const int newStart = state.hardDropTrail.startRow + static_cast<int>(t * static_cast<double>(total));
            if (newStart != state.hardDropTrail.visibleStartRow) {
                state.hardDropTrail.visibleStartRow = newStart;
                state.markDirty();
            }
        }
    }

    switch (state.phase) {
        case GamePhase::Generation: stepGeneration(state); break;
        case GamePhase::Falling: _movement.stepFalling(state, input); break;
        case GamePhase::Pattern: _lineClear.stepPattern(state); break;
        case GamePhase::Iterate: state.phase = GamePhase::Animate; break;
        case GamePhase::Animate: _lineClear.stepAnimate(state); break;
        case GamePhase::Eliminate: _lineClear.stepEliminate(state); break;
        case GamePhase::Completion: stepCompletion(state); break;
    }

    if (state.displayTime() <= 0) {
        state.flags.isGameOver = true;
    }

    auto result = StepResult::Continue;

    if (state.flags.isGameOver)
        result = StepResult::GameOver;
    else if (input.pause)
        result = StepResult::PauseRequested;

    state.updateHighscore();

    return result;
}

void GameController::reset(GameState &state) const {
    state.stats = Stats{};
    state.stats.level = state.config.startingLevel;
    state.stats.goal = _goalPolicy->startingGoalValue(state.stats.level);
    state.lockDown = {};
    state.flags = {};
    state.phase = GamePhase::Falling;
    state.lineClear = {};
    state.hardDropTrail = {};
    state.setPlayerName("");

    if (_variantRule->linesGoal() > 0) {
        state.stats.lines = _variantRule->linesGoal();
    }

    for (auto &row : state.matrix)
        row.fill(0);

    state.pieces.hold = nullptr;
    state.pieces.current = nullptr;
    state.pieces.bagIndex = 0;
    state.pieces.isNewHold = false;
    shuffle(state, 0);
    shuffle(state, 7);

    state.startGameTimer();
    state.activateHighscore();

    _movement.resetTimers();
    _lineClear.resetTimers();
    _timer.stopTimer(kGeneration);
}

void GameController::stepGeneration(GameState &state) const {
    if (_timer.getSeconds(kGeneration) >= kGenerationDelay) {
        _timer.stopTimer(kGeneration);
        popTetrimino(state);
        if (!state.pieces.current->setPosition(state.pieces.current->getStartingPosition())) {
            state.flags.isGameOver = true;
            return;
        }
        _timer.startTimer("fall");
        state.phase = GamePhase::Falling;
        state.markDirty();
    }
}

void GameController::stepCompletion(GameState &state) const {
    if (_variantRule->levelUp() && state.stats.goal <= 0) {
        state.stats.level++;
        state.stats.goal = _goalPolicy->goalValue(state.stats.level) + state.stats.goal;

        if (state.stats.level > MAX_LEVEL) {
            state.flags.isGameOver = true;
            return;
        }
    }

    _timer.startTimer(kGeneration);
    state.phase = GamePhase::Generation;
    state.markDirty();
}

void GameController::shuffle(GameState &state, const size_t start) {
    for (int i = 6; i >= 0; i--) {
        const int j = Random::getInteger(0, i);
        if (i != j)
            state.pieces.bag[start + static_cast<size_t>(i)].swap(state.pieces.bag[start + static_cast<size_t>(j)]);
    }
}

void GameController::popTetrimino(GameState &state) {
    state.pieces.current = state.pieces.bag[state.pieces.bagIndex++].get();
    if (state.pieces.bagIndex >= 7) {
        std::swap_ranges(state.pieces.bag.begin(), state.pieces.bag.begin() + 7, state.pieces.bag.begin() + 7);
        shuffle(state, 7);
        state.pieces.bagIndex = 0;
    }
}
