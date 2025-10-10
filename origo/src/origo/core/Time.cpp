#include "origo/core/Time.h"

namespace Origo::Time {
TimePoint GetNow() {
	return SystemClock::now();
}

}
