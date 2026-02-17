#include "LineClear.h"

#include "Color.h"
#include "Constants.h"
#include "ScoringRule.h"
#include "Timer.h"

using namespace std;

static constexpr auto kAnimate = "animate";

static constexpr double kAnimateDuration = 0.4;
static constexpr double kFlashInterval = 0.1;

LineClear::LineClear(Timer &timer, ScoringRule *scoringRule, GoalPolicy *goalPolicy, VariantRule *variantRule)
    : _timer(timer), _scoringRule(scoringRule), _goalPolicy(goalPolicy), _variantRule(variantRule) {
}

void LineClear::resetTimers() const {
    _timer.stopTimer(kAnimate);
}

void LineClear::stepPattern(GameState &state) const {
    if (auto rows = detectFullRows(state); !rows.empty()) {
        state.lineClear.rows = std::move(rows);
        const int linesCleared = static_cast<int>(state.lineClear.rows.size());
        if (linesCleared == 4)
            state.queueSound(GameSound::Tetris);
        else
            state.queueSound(GameSound::LineClear);

        // Build notification text for impressive manoeuvres
        const bool isTSpin = state.flags.lastMoveIsTSpin;
        const bool isMiniTSpin = state.flags.lastMoveIsMiniTSpin;
        const bool isTetris = (linesCleared == 4);
        const bool isDifficult = isTetris || (isTSpin && linesCleared > 0) || (isMiniTSpin && linesCleared == 1);
        const bool isB2B = state.stats.backToBackBonus && isDifficult;

        string text;
        int color = 0;
        if (isTSpin) {
            static const char *const names[] = {"T-SPIN", "T-SPIN SINGLE", "T-SPIN DOUBLE", "T-SPIN TRIPLE"};
            text = names[min(linesCleared, 3)];
            color = Color::LIGHTMAGENTA;
        } else if (isMiniTSpin) {
            text = "MINI T-SPIN";
            color = Color::MAGENTA;
        } else if (isTetris) {
            text = "TETRIS!";
            color = Color::YELLOW;
        }

        if (!text.empty()) {
            if (isB2B) text = "B2B " + text;
            state.lineClear.notificationText = text;
            state.lineClear.notificationColor = color;
        }

        if (state.stats.currentCombo >= 0) {
            state.lineClear.comboText = "COMBO x" + to_string(state.stats.currentCombo + 1);
            state.lineClear.comboColor = Color::LIGHTCYAN;
        }

        state.phase = GamePhase::Iterate;
    } else {
        // No lines cleared — still need to run scoring for combo reset
        awardScore(state, 0);
        state.phase = GamePhase::Completion;
    }
}

void LineClear::stepAnimate(GameState &state) const {
    if (!_timer.exist(kAnimate)) {
        _timer.startTimer(kAnimate);
        state.lineClear.flashOn = true;
        state.markDirty();
    }

    const double elapsed = _timer.getSeconds(kAnimate);

    if (elapsed >= kAnimateDuration) {
        _timer.stopTimer(kAnimate);
        state.lineClear.flashOn = false;
        state.phase = GamePhase::Eliminate;
        state.markDirty();
        return;
    }

    if (const bool shouldBeOn = static_cast<int>(elapsed / kFlashInterval) % 2 == 0;
        shouldBeOn != state.lineClear.flashOn) {
        state.lineClear.flashOn = shouldBeOn;
        state.markDirty();
    }
}

void LineClear::stepEliminate(GameState &state) const {
    const int linesCleared = static_cast<int>(state.lineClear.rows.size());
    eliminateRows(state, state.lineClear.rows);
    awardScore(state, linesCleared);
    state.lineClear.rows.clear();
    state.lineClear.notificationText.clear();
    state.lineClear.comboText.clear();

    if (auto cascaded = detectFullRows(state); !cascaded.empty()) {
        state.lineClear.rows = std::move(cascaded);
    } else {
        state.phase = GamePhase::Completion;
    }
    state.markDirty();
}

vector<int> LineClear::detectFullRows(const GameState &state) {
    vector<int> rows;
    for (int i = MATRIX_END; i >= MATRIX_START; i--) {
        bool full = true;
        for (int j = 0; j < TETRIS_WIDTH; j++) {
            if (state.matrix[i][j] == 0) {
                full = false;
                break;
            }
        }
        if (full) rows.push_back(i);
    }
    return rows;
}

void LineClear::eliminateRows(GameState &state, const vector<int> &rows) {
    // rows are sorted descending (highest index first) from detectFullRows
    for (const int r : rows)
        state.matrix.erase(state.matrix.begin() + r);

    for (size_t i = 0; i < rows.size(); i++)
        state.matrix.push_front(MatrixRow{});
}

void LineClear::awardScore(GameState &state, const int linesCleared) const {
    if (linesCleared == 4) state.stats.tetris++;
    if (state.flags.lastMoveIsTSpin || state.flags.lastMoveIsMiniTSpin) state.stats.tSpins++;

    auto [points, awardedLines, continuesBackToBack] =
        _scoringRule->compute(linesCleared, state.flags.lastMoveIsTSpin, state.flags.lastMoveIsMiniTSpin,
                              state.stats.backToBackBonus, state.stats.level);

    state.stats.score += points;
    state.stats.backToBackBonus = continuesBackToBack;
    state.flags.lastMoveIsTSpin = false;
    state.flags.lastMoveIsMiniTSpin = false;

    // Combo (Ren) tracking: consecutive piece placements that clear lines.
    // currentCombo reaches 1+ on the 2nd consecutive clear (= first real combo).
    if (linesCleared > 0) {
        state.stats.currentCombo++;
        if (state.stats.currentCombo > state.stats.combos) state.stats.combos = state.stats.currentCombo;
    } else {
        state.stats.currentCombo = -1;
    }

    if (_variantRule->linesGoal() > 0) {
        state.stats.lines -= linesCleared;
        if (state.stats.lines < 0) {
            state.stats.lines = 0;
            state.flags.isGameOver = true;
        }
    } else {
        state.stats.lines += linesCleared;
    }


    if (_goalPolicy->useAwardedLines())
        state.stats.goal -= awardedLines;
    else
        state.stats.goal -= linesCleared;
}
