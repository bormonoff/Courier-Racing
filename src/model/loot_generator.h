#pragma once

#include <chrono>
#include <functional>

namespace loot_gen {

class LootGenerator {
public:
    using RandomGenerator = std::function<double()>;
    using TimeInterval = std::chrono::milliseconds;

    LootGenerator(TimeInterval base_interval, double probability,
                  RandomGenerator random_gen = DefaultGenerator);
    
    LootGenerator(const LootGenerator&) = default;
    LootGenerator& operator=(const LootGenerator&) = default;

    const double GetProbability() const noexcept {
        return probability_;
    }

    const size_t GetInterval() const noexcept {
        return base_interval_.count();
    }

    unsigned Generate(TimeInterval time_delta, unsigned loot_count, 
                      unsigned looter_count);

private:
    static double DefaultGenerator() noexcept {
        return 1.0;
    };
    
    TimeInterval base_interval_;
    double probability_;
    TimeInterval time_without_loot_{};
    RandomGenerator random_generator_;
};
}  // namespace loot_gen