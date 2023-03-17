#include "model.h"

#include <stdexcept>

#include "util/utils.h"

namespace model {

using namespace std::literals;

void Map::AddOffice(Office office) {
    if (warehouse_id_to_index_.contains(office.GetId())) {
        throw std::invalid_argument("Duplicate warehouse");
    }

    const size_t index = offices_.size();
    Office& o = offices_.emplace_back(std::move(office));
    try {
        warehouse_id_to_index_.emplace(o.GetId(), index);
    } catch (...) {
        offices_.pop_back();
        throw std::invalid_argument("Can't emplace office!");
    }
}

void Map::GenerateThings(size_t delta_time, size_t dogs_count) const {
    lost_things_.AddLostThings(dog_map_.GetRoadMap(), 
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

#ifdef DEBUG
void Map::GenerateDebugThings(double x, double y) const {
    lost_things_.AddDebugLostThings(x, y);
}
#endif

void Game::AddMap(Map map) {
    const size_t index = maps_.size();
    if (auto [it, inserted] = map_id_to_index_.emplace(map.GetId(), index); !inserted) {
        throw std::invalid_argument("Map with id "s + *map.GetId() + " already exists"s);
    } else {
        try {
            maps_.emplace_back(std::move(map));
        } catch (...) {
            map_id_to_index_.erase(it);
            throw std::invalid_argument("Can't emplace map!");
        }
    }
}

void LostThings::SetItemTypesCount(size_t count) noexcept {
    items_types_count_ = count;
}

bool operator ==(const Point& a, const Point& b){
    return a.x == b.x && a.y == b.y;
}

void RoadMap::AddRoad(Road road) {
    Road temp = InvertRoad(road);
    InsertRoad(temp);
}

Road& RoadMap::InvertRoad(Road& road) {
    if (road.GetStart().y > road.GetEnd().y) {
        Point temp = road.GetStart();
        road.SetStart(road.GetEnd());
        road.SetEnd(std::move(temp));
    }
    if (road.GetStart().x > road.GetEnd().x) {
        Point temp = road.GetStart();
        road.SetStart(road.GetEnd());
        road.SetEnd(std::move(temp));
    }
    return road;
}

void RoadMap::InsertRoad(Road& road){
    for (auto& it : RoadMap_) {
        if (it.IsHorizontal() && road.IsHorizontal()) {
            if (it.GetEnd() == road.GetStart()) {
                it.SetEnd(road.GetEnd());
                return;
            }
            if (it.GetStart() == road.GetEnd()) {
                it.SetStart(road.GetStart());
                return;
            }
        }
        if (it.IsVertical() && road.IsVertical()) {
            if (it.GetEnd() == road.GetStart()) {
                it.SetEnd(road.GetEnd());
                return;
            }
            if (it.GetStart() == road.GetEnd()) {
                it.SetStart(road.GetStart());
                return;
            }
        }
    }
    RoadMap_.push_back(road);
}

void LostThings::AddLostThings(const std::vector<Road> &roads, 
                                      std::chrono::milliseconds delta, 
                                      size_t dogs_count) {
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

#ifdef DEBUG
void LostThings::AddDebugLostThings(double x, double y) {
    lost_things_[id_] = {x, y, 0, id_};
    id_++;
}
#endif
}  // namespace model
