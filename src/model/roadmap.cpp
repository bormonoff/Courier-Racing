#include "model/roadmap.h"

namespace model {

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