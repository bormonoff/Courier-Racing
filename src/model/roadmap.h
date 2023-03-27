#pragma once

#include <vector>

#include "model/lost_things.h"

namespace model {

class RoadMap {
public:
    using DogRoads = std::vector<Road>;

    RoadMap() = default;
    RoadMap& operator=(const RoadMap&) = default;

    const DogRoads& GetRoadMap() const {
        return RoadMap_;
    }

    void AddRoad(Road road);
    void InsertRoad(Road& road);
    Road& InvertRoad(Road& road);
    
private:
    DogRoads RoadMap_;
};
}  // namespace model