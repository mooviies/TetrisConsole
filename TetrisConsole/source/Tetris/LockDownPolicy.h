#pragma once

#include <memory>

#include "GameState.h"

class LockDownPolicy {
public:
	virtual ~LockDownPolicy() = default;
	[[nodiscard]] virtual bool resetsTimerOnMove() const = 0;
	[[nodiscard]] virtual int moveLimit() const = 0; // -1 = unlimited
};

class ExtendedLockDown final : public LockDownPolicy {
public:
	[[nodiscard]] bool resetsTimerOnMove() const override { return true; }
	[[nodiscard]] int moveLimit() const override { return 15; }
};

class InfiniteLockDown final : public LockDownPolicy {
public:
	[[nodiscard]] bool resetsTimerOnMove() const override { return true; }
	[[nodiscard]] int moveLimit() const override { return -1; }
};

class ClassicLockDown final : public LockDownPolicy {
public:
	[[nodiscard]] bool resetsTimerOnMove() const override { return false; }
	[[nodiscard]] int moveLimit() const override { return -1; }
};

std::unique_ptr<LockDownPolicy> makeLockDownPolicy(MODE mode);
