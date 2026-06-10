#ifndef TIME_UTILS_H
#define TIME_UTILS_H

#include <chrono>
#include <string>

std::string formatDateTime(const std::chrono::system_clock::time_point& timePoint);
std::string formatDateKey(const std::chrono::system_clock::time_point& timePoint);
bool isSameCalendarDay(const std::chrono::system_clock::time_point& a,
                       const std::chrono::system_clock::time_point& b);

#endif
