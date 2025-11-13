#pragma once

#include <chrono>

namespace Origo::Time {

using SystemClock = std::chrono::high_resolution_clock;
using TimePoint = SystemClock::time_point;
using Duration = std::chrono::duration<double>;
using DurationFloat = std::chrono::duration<float>;
using DurationInt = std::chrono::duration<int>;

TimePoint GetNow();
Duration GetTimeSinceStart();

}
