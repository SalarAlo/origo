#pragma once

#include <chrono>

namespace Origo::Time {
using SystemClock = std::chrono::system_clock;
using TimePoint = std::chrono::time_point<SystemClock>;

TimePoint GetNow();
}
