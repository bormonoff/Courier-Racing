#include "util/utils.h"

#include <boost/uuid/uuid_io.hpp>

namespace util {

uint64_t CurrentTime() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

size_t GetRandomNumber(const int min, const int max) {
    static std::random_device rand{};
    static std::mt19937_64 generator(rand());
    std::uniform_int_distribution<int> distribution{min, max};
    // std::cout<<sizeof(rand)<<" bytes - rand "<<std::endl;
    // std::cout<<sizeof(generator)<<" bytes - gen"<<std::endl;
    // std::cout<<sizeof(distribution)<<" bytes - distr"<<std::endl;
    return distribution(generator);
}

// Not a template cause we have real distribution instead
// If there would be int distribution for doubles - UB
double GetRandomDoubleNumber(const double min, const double max) {
    static std::random_device rand{};
    static std::mt19937_64 generator(rand());
    std::uniform_real_distribution<double> distribution{min, max};
    return distribution(generator);
}

UUID NewUUID() {
    return boost::uuids::random_generator()();
}

std::string UUIDToString(const UUID& uuid) {
    return to_string(uuid);
}

UUID UUIDFromString(std::string str) {
    boost::uuids::string_generator gen;
    return gen(str.begin(), str.end());
}
}  // namespace util
