#include "GravityPolicy.h"

#include <algorithm>
#include <cmath>

#include "Constants.h"

double GuidelineGravity::fallInterval(const int level, const DropType dropType) const {
	switch (dropType) {
		case DropType::Normal: return GRAVITY_EQUATION(level);
		case DropType::Soft: return GRAVITY_EQUATION(level) / kSoftDropFactor;
		case DropType::Hard: return kHardDropSpeed;
		default: return 1.0;
	}
}

std::unique_ptr<GravityPolicy> makeDefaultGravityPolicy() {
	return std::make_unique<GuidelineGravity>();
}
