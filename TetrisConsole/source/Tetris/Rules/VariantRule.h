#pragma once

#include <limits.h>
#include <memory>

#include "Constants.h"

class VariantRule {
public:
    virtual ~VariantRule() = default;
    [[nodiscard]] virtual int linesGoal() const = 0;
    [[nodiscard]] virtual bool levelUp() const = 0;
    [[nodiscard]] virtual double timeLimit() const = 0;
    [[nodiscard]] virtual bool showGoal() const = 0;
};

class MarathonVariant final : public VariantRule {
public:
    [[nodiscard]] int linesGoal() const override { return -1; }
    [[nodiscard]] bool levelUp() const override { return true; }
    [[nodiscard]] double timeLimit() const override { return -1; }
    [[nodiscard]] bool showGoal() const override { return true; }
};

class SprintVariant final : public VariantRule {
public:
    [[nodiscard]] int linesGoal() const override { return 40; }
    [[nodiscard]] bool levelUp() const override { return false; }
    [[nodiscard]] double timeLimit() const override { return -1; }
    [[nodiscard]] bool showGoal() const override { return false; }
};

class UltraVariant final : public VariantRule {
    [[nodiscard]] int linesGoal() const override { return -1; }
    [[nodiscard]] bool levelUp() const override { return false; }
    [[nodiscard]] double timeLimit() const override { return 120.0; }
    [[nodiscard]] bool showGoal() const override { return false; }
};

std::unique_ptr<VariantRule> makeVariantRule(GameVariant variant = GameVariant::Marathon);