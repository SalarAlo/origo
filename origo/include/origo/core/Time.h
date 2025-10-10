#pragma once

#include <chrono>

namespace Origo::Time {
using SystemClock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<SystemClock>;
using Duration = std::chrono::duration<double>;

TimePoint GetNow();
}
