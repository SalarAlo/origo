#include "origo/core/Time.h"

namespace Origo::Time {
TimePoint get_now() {
	return SystemClock::now();
}

Duration get_time_since_start() {
	static TimePoint start = SystemClock::now();
	return get_now() - start;
}

}
