#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>

#include "../util/tagged.h"

namespace model {

using Dimension = int;
using Coord = Dimension;

struct Point {
    Coord x, y;
};

bool operator ==(const Point& a, const Point& b);

struct Size {
    Dimension width, height;
};

struct Rectangle {
    Point position;
    Size size;
};

struct Offset {
    Dimension dx, dy;
};

class Road {
    struct HorizontalTag {
        explicit HorizontalTag() = default;
    };

    struct VerticalTag {
        explicit VerticalTag() = default;
    };

public:
    constexpr static HorizontalTag HORIZONTAL{};
    constexpr static VerticalTag VERTICAL{};

    Road(HorizontalTag, Point start, Coord end_x) noexcept
        : start_{start}
        , end_{end_x, start.y} {
    }

    Road(VerticalTag, Point start, Coord end_y) noexcept
        : start_{start}
        , end_{start.x, end_y} {
    }

    Road() = default;
    
    bool IsHorizontal() const noexcept {
        return start_.y == end_.y;
    }

    bool IsVertical() const noexcept {
        return start_.x == end_.x;
    }

    Point GetStart() const noexcept {
        return start_;
    }

    Point GetEnd() const noexcept {
        return end_;
    }

    void SetStart(Point point){ 
        start_ = point;
    }

    void SetEnd(Point point) {
        end_ = point;
    }

private:
    Point start_;
    Point end_;
};

// bool XSort(const Road& a, const Road& b);
// bool YSort(const Road& a, const Road& b);

class Building {
public:
    explicit Building(Rectangle bounds) noexcept
        : bounds_{bounds} {
    }

    const Rectangle& GetBounds() const noexcept {
        return bounds_;
    }

private:
    Rectangle bounds_;
};

class Office {
public:
    using Id = util::Tagged<std::string, Office>;

    Office(Id id, Point position, Offset offset) noexcept
        : id_{std::move(id)}
        , position_{position}
        , offset_{offset} {
    }

    const Id& GetId() const noexcept {
        return id_;
    }

    Point GetPosition() const noexcept {
        return position_;
    }

    Offset GetOffset() const noexcept {
        return offset_;
    }

private:
    Id id_;
    Point position_;
    Offset offset_;
};

class RoadMap{
public:
    using DogRoads = std::vector<Road>;

    // void AddRoad(Road road){
    //     if(road.IsHorizontal()){
    //         Road temp = InvertRoad(road);
    //         InsertHorizontalRoad(temp); 
    //     }
    //     if(road.IsVertical()){
    //         Road temp = InvertRoad(road);
    //         InsertVerticalRoad(temp);
    //     }
    // }

    void AddRoad(Road road){
        Road temp = InvertRoad(road);
        InsertRoad(temp);
    }

    // void SortDogMaps(){
    //     std::sort(RoadMap_.begin(), RoadMap_.end());
    // }

    Road& InvertRoad(Road& road){
        if(road.GetStart().y > road.GetEnd().y){
                Point temp = road.GetStart();
                road.SetStart(road.GetEnd());
                road.SetEnd(std::move(temp));
        }
        if(road.GetStart().x > road.GetEnd().x){
                Point temp = road.GetStart();
                road.SetStart(road.GetEnd());
                road.SetEnd(std::move(temp));
        }
        return road;
    }

    void InsertRoad(Road& road){
        for(auto& it : RoadMap_){
            if(it.IsHorizontal() && road.IsHorizontal()){
                if(it.GetEnd() == road.GetStart()){
                    it.SetEnd(road.GetEnd());
                    return;
                }
                if(it.GetStart() == road.GetEnd()){
                    it.SetStart(road.GetStart());
                    return;
                }
            }
            if(it.IsVertical() && road.IsVertical()){
                if(it.GetEnd() == road.GetStart()){
                    it.SetEnd(road.GetEnd());
                    return;
                }
                if(it.GetStart() == road.GetEnd()){
                    it.SetStart(road.GetStart());
                    return;
                }
            }
        }
        RoadMap_.push_back(road);
    }

    const DogRoads& GetRoadMap() const {
        return RoadMap_;
    }

    // const DogRoads& GetHorizontalRoads() const {
    //     return HorizontalRoads_;
    // }

private:
    DogRoads RoadMap_;
};

class Map {
public:
    using Id = util::Tagged<std::string, Map>;
    using Roads = std::vector<Road>;
    using Buildings = std::vector<Building>;
    using Offices = std::vector<Office>;

    Map(Id id, std::string name, int64_t speed) noexcept
        : id_(std::move(id))
        , name_(std::move(name))
        , dog_speed_(speed) {
    }

    const Id& GetId() const noexcept {
        return id_;
    }

    const std::string& GetName() const noexcept {
        return name_;
    }

    const Buildings& GetBuildings() const noexcept {
        return buildings_;
    }

    const Roads& GetRoads() const noexcept {
        return roads_;
    }

    const std::vector<Road>& GetRoadMap() const noexcept {
        return dog_map_.GetRoadMap();
    }

    const Offices& GetOffices() const noexcept {
        return offices_;
    }

    double GetDogSpeed() const noexcept {
        return dog_speed_;
    }

    void AddRoad(const Road& road) {
        roads_.emplace_back(road);
        dog_map_.AddRoad(road);
    }

    void AddBuilding(const Building& building) {
        buildings_.emplace_back(building);
    }

    void AddOffice(Office office);

private:
    using OfficeIdToIndex = std::unordered_map<Office::Id, size_t, util::TaggedHasher<Office::Id>>;

    Id id_;
    std::string name_;
    Roads roads_;
    Buildings buildings_;
    double dog_speed_;

    OfficeIdToIndex warehouse_id_to_index_;
    Offices offices_;
    RoadMap dog_map_;
};

class Game {
public:
    using Maps = std::vector<Map>;

    void AddMap(Map map);

    const Maps& GetMaps() const noexcept {
        return maps_;
    }

    const Map* FindMap(const Map::Id& id) const noexcept {
        if (auto it = map_id_to_index_.find(id); it != map_id_to_index_.end()) {
            return &maps_.at(it->second);
        }
        return nullptr;
    }

private:
    using MapIdHasher = util::TaggedHasher<Map::Id>;
    using MapIdToIndex = std::unordered_map<Map::Id, size_t, MapIdHasher>;

    std::vector<Map> maps_;
    MapIdToIndex map_id_to_index_;
};


}  // namespace model