#include "TimeUtils.h"

#include <iomanip>
#include <sstream>

namespace {

std::tm toLocalTime(std::time_t time) {
    std::tm localTime{};
#ifdef _WIN32
    localtime_s(&localTime, &time);
#else
    localtime_r(&time, &localTime);
#endif
    return localTime;
}

}  // namespace

std::string formatDateTime(const std::chrono::system_clock::time_point& timePoint) {
    std::time_t time = std::chrono::system_clock::to_time_t(timePoint);
    std::tm localTime = toLocalTime(time);
    std::ostringstream out;
    out << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S");
    return out.str();
}

std::string formatDateKey(const std::chrono::system_clock::time_point& timePoint) {
    std::time_t time = std::chrono::system_clock::to_time_t(timePoint);
    std::tm localTime = toLocalTime(time);
    std::ostringstream out;
    out << std::put_time(&localTime, "%Y-%m-%d");
    return out.str();
}

bool isSameCalendarDay(const std::chrono::system_clock::time_point& a,
                       const std::chrono::system_clock::time_point& b) {
    return formatDateKey(a) == formatDateKey(b);
}
