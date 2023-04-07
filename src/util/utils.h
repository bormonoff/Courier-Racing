#pragma once

#include <chrono>
#include <random>

#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid.hpp>

namespace util {

using UUID = boost::uuids::uuid;

uint64_t CurrentTime();

size_t GetRandomNumber(const int min, const int max);
double GetRandomDoubleNumber(const double min, const double max);

UUID NewUUID();
std::string UUIDToString(const UUID& uuid);
UUID UUIDFromString(std::string& str);
}  // namespace util
