#include "LockDownPolicy.h"

std::unique_ptr<LockDownPolicy> makeLockDownPolicy(const MODE mode) {
	switch (mode) {
		case MODE::EXTENDED:          return std::make_unique<ExtendedLockDown>();
		case MODE::EXTENDED_INFINITY: return std::make_unique<InfiniteLockDown>();
		case MODE::CLASSIC:           return std::make_unique<ClassicLockDown>();
	}
	return std::make_unique<ExtendedLockDown>();
}
