#pragma once

#include <vector>

#include "GameState.h"
#include "GoalPolicy.h"
#include "VariantRule.h"

class ScoringRule;
class Timer;

class LineClear {
public:
    LineClear(Timer &timer, ScoringRule *scoringRule, GoalPolicy *goalPolicy, VariantRule *variantRule);

    void stepPattern(GameState &state) const;
    void stepAnimate(GameState &state) const;
    void stepEliminate(GameState &state) const;
    void resetTimers() const;
    void setVariantRule(VariantRule *rule) { _variantRule = rule; }

private:
    [[nodiscard]] static std::vector<int> detectFullRows(const GameState &state);
    static void eliminateRows(GameState &state, const std::vector<int> &rows);
    void awardScore(GameState &state, int linesCleared) const;

    Timer &_timer;
    ScoringRule *_scoringRule;
    GoalPolicy *_goalPolicy;
    VariantRule *_variantRule;
};
