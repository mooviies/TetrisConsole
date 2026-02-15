#include "LockDownPolicy.h"

std::unique_ptr<LockDownPolicy> makeLockDownPolicy(MODE mode) {
	switch (mode) {
		case EXTENDED:          return std::make_unique<ExtendedLockDown>();
		case EXTENDED_INFINITY: return std::make_unique<InfiniteLockDown>();
		case CLASSIC:           return std::make_unique<ClassicLockDown>();
	}
	return std::make_unique<ExtendedLockDown>();
}
