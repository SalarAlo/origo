#include "origo/core/Time.h"

namespace Origo::Time {
TimePoint GetNow() {
	return SystemClock::now();
}

Duration GetTimeSinceStart() {
	static TimePoint start = SystemClock::now();
	return GetNow() - start;
}

}
