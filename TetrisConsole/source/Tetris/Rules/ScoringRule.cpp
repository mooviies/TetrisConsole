#include "ScoringRule.h"

ScoreResult GuidelineScoringRule::compute(const int linesCleared, const bool tSpin, const bool miniTSpin,
                                          const bool b2bActive, const int level) const {
    ScoreResult result;
    int awardedLines = linesCleared;
    bool continuesB2B = b2bActive;

    if (tSpin) {
        int value = 0;
        switch (linesCleared) {
            case 0:
                value = 400;
                awardedLines = 4;
                break;
            case 1:
                value = 800;
                awardedLines = 8;
                break;
            case 2:
                value = 1200;
                awardedLines = 12;
                break;
            case 3:
                value = 1600;
                awardedLines = 16;
                break;
            default:;
        }

        if (linesCleared >= 1) {
            if (b2bActive) {
                value += value / 2;
                awardedLines += (linesCleared + 1) / 2;
            }
            continuesB2B = true;
        }

        result.points = static_cast<int64_t>(value) * level;
    } else if (miniTSpin) {
        int value = 0;
        if (linesCleared == 1) {
            value = 200;
            awardedLines = 2;
            if (b2bActive) {
                value += value / 2;
                awardedLines += (linesCleared + 1) / 2;
            }
            continuesB2B = true;
        } else {
            value = 100;
            awardedLines = 1;
        }

        result.points = static_cast<int64_t>(value) * level;
    } else {
        int value = 0;
        switch (linesCleared) {
            case 1:
                value = 100;
                continuesB2B = false;
                break;
            case 2:
                value = 300;
                awardedLines = 3;
                continuesB2B = false;
                break;
            case 3:
                value = 500;
                awardedLines = 5;
                continuesB2B = false;
                break;
            case 4:
                value = 800;
                awardedLines = 8;
                if (b2bActive) {
                    value += value / 2;
                    awardedLines += (linesCleared + 1) / 2;
                }
                continuesB2B = true;
                break;
            default:;
        }

        result.points = static_cast<int64_t>(value) * level;
    }

    result.awardedLines = awardedLines;
    result.continuesBackToBack = continuesB2B;
    return result;
}

std::unique_ptr<ScoringRule> makeDefaultScoringRule() {
    return std::make_unique<GuidelineScoringRule>();
}
