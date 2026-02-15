#pragma once

#include <memory>

#define GRAVITY_EQUATION(level) pow((0.8 - ((level - 1) * 0.007)), level - 1)

enum class DROP_TYPE;
static constexpr double kSoftDropFactor = 20.0;
static constexpr double kHardDropSpeed = 0.0001;

class GravityPolicy {
public:
	virtual ~GravityPolicy() = default;
	[[nodiscard]] virtual double fallInterval(int level, DROP_TYPE dropType) const = 0;
};

class GuidelineGravity final : public GravityPolicy {
public:
	[[nodiscard]] double fallInterval(int level, DROP_TYPE dropType) const override;
};

std::unique_ptr<GravityPolicy> makeDefaultGravityPolicy();
