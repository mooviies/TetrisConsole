#include "GravityPolicy.h"

#include <algorithm>
#include <cmath>

#include "Constants.h"

double GuidelineGravity::fallInterval(const int level, const DROP_TYPE dropType) const {
	switch (dropType) {
		case DROP_TYPE::NORMAL: return GRAVITY_EQUATION(level);
		case DROP_TYPE::SOFT: return GRAVITY_EQUATION(level) / kSoftDropFactor;
		case DROP_TYPE::HARD: return kHardDropSpeed;
		default: return 1.0;
	}
}

std::unique_ptr<GravityPolicy> makeDefaultGravityPolicy() {
	return std::make_unique<GuidelineGravity>();
}
