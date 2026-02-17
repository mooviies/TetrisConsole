#include "VariantRule.h"

std::unique_ptr<VariantRule> makeVariantRule(const GameVariant variant) {
    switch (variant) {
        case GameVariant::Marathon: return std::make_unique<MarathonVariant>();
        case GameVariant::Sprint:   return std::make_unique<SprintVariant>();
        case GameVariant::Ultra:    return std::make_unique<UltraVariant>();
    }
    return std::make_unique<MarathonVariant>();
}