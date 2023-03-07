#pragma once

#include <chrono>
#include <random>

namespace util {

uint64_t CurrentTime();
size_t GetRandomNumber(const int min, const int max);
double GetRandomDoubleNumber(const double min, const double max);
}  // namespace util
