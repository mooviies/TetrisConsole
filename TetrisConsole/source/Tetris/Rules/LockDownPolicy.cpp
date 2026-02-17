#include "LockDownPolicy.h"

std::unique_ptr<LockDownPolicy> makeLockDownPolicy(const LockDownMode mode) {
	switch (mode) {
		case LockDownMode::Extended:          return std::make_unique<ExtendedLockDown>();
		case LockDownMode::ExtendedInfinity: return std::make_unique<InfiniteLockDown>();
		case LockDownMode::Classic:           return std::make_unique<ClassicLockDown>();
	}
	return std::make_unique<ExtendedLockDown>();
}
