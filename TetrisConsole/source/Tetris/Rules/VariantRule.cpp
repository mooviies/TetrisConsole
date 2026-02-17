#include "VariantRule.h"

std::unique_ptr<VariantRule> makeVariantRule(const GameVariant variant) {
    switch (variant) {
        case GameVariant::MARATHON: return std::make_unique<MarathonVariant>();
        case GameVariant::SPRINT:   return std::make_unique<SprintVariant>();
        case GameVariant::ULTRA:    return std::make_unique<UltraVariant>();
    }
    return std::make_unique<MarathonVariant>();
}