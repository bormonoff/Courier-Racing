#pragma once

#include <algorithm>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <model/loot_generator.h>
#include "util/tagged.h"

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

struct Item {
    double x;
    double y;
    size_t type;
    size_t ID;
};

class LostThings {
public:
    using Things = std::unordered_map<size_t, Item>;

    explicit LostThings(size_t period, double probability) 
        : generator_{std::chrono::milliseconds(period), probability} {}

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

    void RemoveItemViaIndex(size_t index);
    void SetItemTypesCount(size_t count) noexcept;
    void AddLostThings(const std::vector<Road> &roads, std::chrono::milliseconds delta, 
                       size_t dogs_count);

private:
    void AddThing(const std::vector<Road> &roads);

    mutable Things lost_things_;
    loot_gen::LootGenerator generator_;
    size_t items_types_count_ {1};
    size_t id_ {0};
};

class RoadMap {
public:
    using DogRoads = std::vector<Road>;

    const DogRoads& GetRoadMap() const {
        return RoadMap_;
    }

    void AddRoad(Road road);
    void InsertRoad(Road& road);
    Road& InvertRoad(Road& road);
    
private:
    DogRoads RoadMap_;
};

class Map {
public:
    using Id = util::Tagged<std::string, Map>;
    using Roads = std::vector<Road>;
    using Buildings = std::vector<Building>;
    using Offices = std::vector<Office>;
    using Things = std::unordered_map<size_t, Item>;

    explicit Map(Id id, std::string name, size_t speed, size_t period,
                 size_t bag_capacity, double probability) noexcept
        : id_(std::move(id)),
          name_(std::move(name)),
          dog_speed_(speed), 
          bag_capacity_ (bag_capacity),
          lost_things_{period, probability} {
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

    const Things& GetLostThings() const noexcept {
        return lost_things_.GetLostThings();
    }

    const Offices& GetOffices() const noexcept {
        return offices_;
    }

    double GetDogSpeed() const noexcept {
        return dog_speed_;
    }

    size_t GetMaxBagSize() const noexcept {
        return bag_capacity_;
    }

    void AddBuilding(const Building& building) {
        buildings_.emplace_back(building);
    }

    void AddRoad(const Road& road);
    void AddOffice(Office office);
    void RemoveItemFromMap(size_t index) const;
    void GenerateThings(size_t delta_time, size_t dogs_count) const;
    void SetTypesCount(size_t count);
    const Item GetLostThingViaIndex(size_t index) const noexcept;

private:
    using OfficeIdToIndex = std::unordered_map<Office::Id, size_t, util::TaggedHasher<Office::Id>>;

    Id id_;
    std::string name_;
    Roads roads_;
    Buildings buildings_;
    Offices offices_;
    RoadMap dog_map_; 
    mutable LostThings lost_things_;

    double dog_speed_;
    size_t bag_capacity_;

    OfficeIdToIndex warehouse_id_to_index_;
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