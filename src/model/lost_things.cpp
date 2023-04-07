#include "model/lost_things.h"

#include "util/utils.h"

namespace model {

LostThings::LostThings(size_t period, double probability) 
        : generator_{std::chrono::milliseconds(period), probability}, 
          items_types_count_{1},
          id_{0} {}

void LostThings::SetItemTypesCount(size_t count) noexcept {
    items_types_count_ = count;
}

void LostThings::SetId(size_t value) noexcept {
    id_ = value;
}

void LostThings::AddLostThing(size_t key, const Item& value) {
    lost_things_[key] = value;
}

void LostThings::CreateLostThing(const std::vector<Road> &roads, 
                                      std::chrono::milliseconds delta, 
                                      size_t dogs_count) {
    if (lost_things_.size() > dogs_count) {
        for (auto it = lost_things_.begin();it != lost_things_.end();) {
            it = lost_things_.erase(it);
            if (lost_things_.size() <= dogs_count) {
                break;
            }
        }
    }
    size_t items_to_add = generator_.Generate(delta, GetThingsCount(), dogs_count);
    
    for (int i = 0; i < items_to_add; ++i) {
        AddThing(roads);
    }
}

void LostThings::AddThing(const std::vector<Road> &roads) {
    Road road = roads[util::GetRandomNumber(0, roads.size() - 1)];
    double x = util::GetRandomDoubleNumber(road.GetStart().x - 0.4, 
                                           road.GetEnd().x + 0.4);
    double y = util::GetRandomDoubleNumber(road.GetStart().y - 0.4, 
                                           road.GetEnd().y + 0.4);
    size_t type = util::GetRandomNumber(0, items_types_count_-1);
    lost_things_[id_] = {x, y, type, id_};
    id_++;
}

void LostThings::RemoveItemViaIndex(size_t index) {
    lost_things_.erase(index);
}

bool operator==(const LostThings& a, const LostThings& b) {
    return a.GetLostThings() == b.GetLostThings() &&
           a.GetGeneratorProbability() == b.GetGeneratorProbability() &&
           a.GetGeneratorInterval() == b.GetGeneratorInterval() &&
           a.GetItemTypesCount() == b.GetItemTypesCount() &&
           a.GetItemIdCount() == b.GetItemIdCount();
}
}  // namespace model