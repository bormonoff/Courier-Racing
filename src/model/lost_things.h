#pragma once 

#include <unordered_map>

#include "model/office.h"
#include "model/loot_generator.h"

namespace model {

struct Item {
    double x;
    double y;
    size_t type;
    size_t ID;

    constexpr auto operator<=>(const Item&) const = default;
};

class LostThings {
public:
    using Things = std::unordered_map<size_t, Item>;

    explicit LostThings(size_t period, double probability);

    LostThings(const LostThings&) = default;
    LostThings& operator=(const LostThings&) = default;

    const Things& GetLostThings() const noexcept {
        return lost_things_;
    }

    const Item GetLostThingViaIndex(size_t index) const noexcept {
        return lost_things_.at(index);
    }

    const size_t GetThingsCount() const noexcept {
        return lost_things_.size();
    }
    
    const size_t GetItemTypesCount() const noexcept {
        return items_types_count_;
    }

    const size_t GetItemIdCount() const noexcept {
        return id_;
    }

    const double GetGeneratorProbability() const noexcept {
        return generator_.GetProbability();
    }

    const size_t GetGeneratorInterval() const noexcept {
        return generator_.GetInterval();
    }

    void RemoveItemViaIndex(size_t index);
    void SetItemTypesCount(size_t count) noexcept;
    void SetId(size_t value) noexcept;
    void CreateLostThing(const std::vector<Road> &roads, std::chrono::milliseconds delta, 
                         size_t dogs_count);
    void AddLostThing(size_t key, const Item& value);

private:
    void AddThing(const std::vector<Road> &roads);

    Things lost_things_;
    loot_gen::LootGenerator generator_;
    size_t items_types_count_;
    size_t id_;
};

bool operator==(const LostThings& a, const LostThings& b);
}  // namespace model