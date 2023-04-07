#include "model/map.h"

namespace model {

using namespace std::literals;

Map::Map(Id id, std::string name, size_t speed, size_t period, size_t bag_capacity,
         double probability, size_t lifetime) noexcept
    : id_(std::move(id)),
        name_(std::move(name)),
        dog_speed_(speed), 
        bag_capacity_ (bag_capacity),
        lost_things_{period, probability},
        dog_lifetime_{lifetime} {
}


void Map::AddOffice(Office office) {
    if (warehouse_id_to_index_.contains(office.GetId())) {
        throw std::runtime_error("Duplicate warehouse");
    }

    const size_t index = offices_.size();
    Office& o = offices_.emplace_back(std::move(office));
    try {
        warehouse_id_to_index_.emplace(o.GetId(), index);
    } catch (...) {
        offices_.pop_back();
        throw std::runtime_error("Can't emplace office!");
    }
}

void Map::GenerateThings(size_t delta_time, size_t dogs_count) const {
    lost_things_.CreateLostThing(dog_map_.GetRoadMap(), 
                                std::chrono::milliseconds(delta_time),
                                dogs_count);
}

void Map::SetTypesCount(size_t count) {
    lost_things_.SetItemTypesCount(count);
}

void Map::AddRoad(const Road& road) {
    roads_.emplace_back(road);
    dog_map_.AddRoad(road);
}

const Item Map::GetLostThingViaIndex(size_t index) const noexcept {
    return lost_things_.GetLostThingViaIndex(index);
}

void Map::RemoveItemFromMap(size_t index) const {
    lost_things_.RemoveItemViaIndex(index);
}

void Map::AddItemCost(size_t count) {
    items_cost_.emplace_back(count);
}

void Map::AddItem(size_t index, const Item& item) const {
    lost_things_.AddLostThing(index, item);
}

void Map::AddBuilding(const Building& building) {
    buildings_.emplace_back(building);
}
}  // namespace model