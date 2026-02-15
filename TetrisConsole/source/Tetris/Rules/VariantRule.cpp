#include "VariantRule.h"

std::unique_ptr<VariantRule> makeVariantRule(const VARIANT variant) {
    switch (variant) {
        case VARIANT::MARATHON: return std::make_unique<MarathonVariant>();
        case VARIANT::SPRINT:   return std::make_unique<SprintVariant>();
        case VARIANT::ULTRA:    return std::make_unique<UltraVariant>();
    }
    return std::make_unique<MarathonVariant>();
}