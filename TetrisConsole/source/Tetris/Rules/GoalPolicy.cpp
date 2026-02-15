#include "GoalPolicy.h"

int VariableGoal::goalValue(const int level)  const {
    return 5 * level;
}

int VariableGoal::startingGoalValue(const int level) const {
    return 5 * level;
}

bool VariableGoal::useAwardedLines() const {
    return true;
}

int FixedGoal::goalValue(int)  const {
    return 10;
}

int FixedGoal::startingGoalValue(const int level) const {
    return 10 * level;
}

bool FixedGoal::useAwardedLines() const {
    return false;
}

std::unique_ptr<GoalPolicy> makeDefaultGoalPolicy() {
    return std::make_unique<VariableGoal>();
}