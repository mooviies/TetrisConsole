#include "LineClear.h"

#include "Constants.h"
#include "ScoringRule.h"
#include "Timer.h"

using namespace std;

static constexpr auto ANIMATE = "animate";

static constexpr double ANIMATE_DURATION = 0.4;
static constexpr double FLASH_INTERVAL   = 0.1;

LineClear::LineClear(Timer& timer, ScoringRule* scoringRule)
    : _timer(timer), _scoringRule(scoringRule) {}

void LineClear::resetTimers() const {
	_timer.stopTimer(ANIMATE);
}

void LineClear::stepPattern(GameState& state) const {
	if (auto rows = detectFullRows(state); !rows.empty()) {
		state.lineClear.rows = std::move(rows);
		if (const int linesCleared = static_cast<int>(state.lineClear.rows.size()); linesCleared == 4)
			state.queueSound(GameSound::Tetris);
		else
			state.queueSound(GameSound::LineClear);
		state.phase = GamePhase::Iterate;
	} else {
		// No lines cleared — still need to run scoring for combo reset
		awardScore(state, 0);
		state.phase = GamePhase::Completion;
	}
}

void LineClear::stepAnimate(GameState& state) const {
	if (!_timer.exist(ANIMATE)) {
		_timer.startTimer(ANIMATE);
		state.lineClear.flashOn = true;
		state.markDirty();
	}

	const double elapsed = _timer.getSeconds(ANIMATE);

	if (elapsed >= ANIMATE_DURATION) {
		_timer.stopTimer(ANIMATE);
		state.lineClear.flashOn = false;
		state.phase = GamePhase::Eliminate;
		state.markDirty();
		return;
	}

	if (const bool shouldBeOn = static_cast<int>(elapsed / FLASH_INTERVAL) % 2 == 0;
		shouldBeOn != state.lineClear.flashOn) {
		state.lineClear.flashOn = shouldBeOn;
		state.markDirty();
	}
}

void LineClear::stepEliminate(GameState& state) const {
	const int linesCleared = static_cast<int>(state.lineClear.rows.size());
	eliminateRows(state, state.lineClear.rows);
	awardScore(state, linesCleared);
	state.lineClear.rows.clear();

	if (auto cascaded = detectFullRows(state); !cascaded.empty()) {
		state.lineClear.rows = std::move(cascaded);
	} else {
		state.phase = GamePhase::Completion;
	}
	state.markDirty();
}

vector<int> LineClear::detectFullRows(const GameState& state) {
	vector<int> rows;
	for (int i = MATRIX_END; i >= MATRIX_START; i--) {
		bool full = true;
		for (int j = 0; j < TETRIS_WIDTH; j++) {
			if (state.matrix[i][j] == 0) {
				full = false;
				break;
			}
		}
		if (full)
			rows.push_back(i);
	}
	return rows;
}

void LineClear::eliminateRows(GameState& state, const vector<int>& rows) {
	// rows are sorted descending (highest index first) from detectFullRows
	for (const int r : rows)
		state.matrix.erase(state.matrix.begin() + r);

	for (size_t i = 0; i < rows.size(); i++)
		state.matrix.push_front(MatrixRow{});
}

void LineClear::awardScore(GameState& state, const int linesCleared) const {
	if (linesCleared == 4)
		state.stats.tetris++;
	if (state.flags.lastMoveIsTSpin || state.flags.lastMoveIsMiniTSpin)
		state.stats.tSpins++;

	auto [points, awardedLines, continuesBackToBack] =
		_scoringRule->compute(linesCleared,
	state.flags.lastMoveIsTSpin, state.flags.lastMoveIsMiniTSpin,
	state.stats.backToBackBonus, state.stats.level);

	state.stats.score += points;
	state.stats.backToBackBonus = continuesBackToBack;
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
	state.stats.goal += awardedLines;
}
