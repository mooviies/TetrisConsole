#pragma once

#include <array>
#include <memory>

class GravityPolicy {
public:
	virtual ~GravityPolicy() = default;
	[[nodiscard]] virtual double fallInterval(int level, bool softDrop) const = 0;
};

class GuidelineGravity final : public GravityPolicy {
public:
	[[nodiscard]] double fallInterval(int level, bool softDrop) const override;

private:
	static constexpr std::array<double, 16> kNormal = {
		0, 1.0, 0.793, 0.618, 0.473, 0.355, 0.262, 0.190, 0.135, 0.094, 0.064, 0.043, 0.028, 0.018, 0.011, 0.007
	};
	static constexpr std::array<double, 16> kFast = {
		0, 0.05, 0.03965, 0.0309, 0.02365, 0.01775, 0.0131, 0.0095, 0.00675, 0.0047, 0.0032, 0.00215, 0.0014,
		0.0009, 0.00055, 0.00035
	};
};

std::unique_ptr<GravityPolicy> makeDefaultGravityPolicy();
