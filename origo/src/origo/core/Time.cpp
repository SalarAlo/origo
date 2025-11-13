#include "origo/core/Time.h"

namespace Origo::Time {
TimePoint GetNow() {
	return SystemClock::now();
}

Duration GetTimeSinceStart() {
	static TimePoint start = SystemClock::now(); // captured exactly once
	return GetNow() - start;
}

}
