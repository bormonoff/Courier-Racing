#include "model.h"

#include <stdexcept>

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


}  // namespace model
