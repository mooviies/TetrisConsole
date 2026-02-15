#include "GravityPolicy.h"

#include <algorithm>

double GuidelineGravity::fallInterval(const int level, const bool softDrop) const {
	const int idx = std::clamp(level, 1, 15);
	const auto& arr = softDrop ? kFast : kNormal;
	return arr[static_cast<size_t>(idx)];
}

std::unique_ptr<GravityPolicy> makeDefaultGravityPolicy() {
	return std::make_unique<GuidelineGravity>();
}
