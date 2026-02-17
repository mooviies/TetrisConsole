#pragma once

#include <cstdint>
#include <memory>

struct ScoreResult {
    int64_t points{};
    int awardedLines{};
    bool continuesBackToBack{};
};

class ScoringRule {
public:
    virtual ~ScoringRule() = default;
    [[nodiscard]] virtual ScoreResult compute(int linesCleared, bool tSpin, bool miniTSpin, bool b2bActive,
                                              int level) const = 0;
};

class GuidelineScoringRule final : public ScoringRule {
public:
    [[nodiscard]] ScoreResult compute(int linesCleared, bool tSpin, bool miniTSpin, bool b2bActive,
                                      int level) const override;
};

std::unique_ptr<ScoringRule> makeDefaultScoringRule();
