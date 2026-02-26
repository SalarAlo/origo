#pragma once

namespace Origo::Time {

using SystemClock = std::chrono::steady_clock;
using TimePoint = SystemClock::time_point;
using Duration = std::chrono::duration<double>;
using DurationFloat = std::chrono::duration<float>;
using DurationInt = std::chrono::duration<int>;

TimePoint get_now();
Duration get_time_since_start();

}
