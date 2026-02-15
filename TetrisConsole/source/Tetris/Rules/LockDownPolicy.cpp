#include "LockDownPolicy.h"

std::unique_ptr<LockDownPolicy> makeLockDownPolicy(const LOCKDOWN_MODE mode) {
	switch (mode) {
		case LOCKDOWN_MODE::EXTENDED:          return std::make_unique<ExtendedLockDown>();
		case LOCKDOWN_MODE::EXTENDED_INFINITY: return std::make_unique<InfiniteLockDown>();
		case LOCKDOWN_MODE::CLASSIC:           return std::make_unique<ClassicLockDown>();
	}
	return std::make_unique<ExtendedLockDown>();
}
