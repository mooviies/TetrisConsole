#pragma once

#include <memory>

class GoalPolicy {
public:
    virtual ~GoalPolicy() = default;
    [[nodiscard]] virtual int goalValue(int level) const = 0;
    [[nodiscard]] virtual int startingGoalValue(int level) const = 0;
    [[nodiscard]] virtual bool useAwardedLines() const = 0;
};

class VariableGoal final : public GoalPolicy {
public:
    [[nodiscard]] int goalValue(int level) const override;
    [[nodiscard]] int startingGoalValue(int level) const override;
    [[nodiscard]] bool useAwardedLines() const override;
};

class FixedGoal final : public GoalPolicy {
public:
    [[nodiscard]] int goalValue(int level) const override;
    [[nodiscard]] int startingGoalValue(int level) const override;
    [[nodiscard]] bool useAwardedLines() const override;
};

std::unique_ptr<GoalPolicy> makeDefaultGoalPolicy();
