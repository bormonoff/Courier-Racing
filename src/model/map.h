#pragma once 

#include "model/roadmap.h"

namespace model {

class Map {
public:
    using Id = util::Tagged<std::string, Map>;
    using Roads = std::vector<Road>;
    using Buildings = std::vector<Building>;
    using Offices = std::vector<Office>;
    using Things = std::unordered_map<size_t, Item>;
    using Points = std::vector<size_t>;

    explicit Map(Id id, std::string name, size_t speed, size_t period,
                 size_t bag_capacity, double probability) noexcept;

    Map(const Map&) = default;
    Map& operator=(const Map&) = default;

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

    const Points& GetScoreArray() const noexcept {
        return items_cost_;
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

    void AddBuilding(const Building& building);
    void AddRoad(const Road& road);
    void AddOffice(Office office);
    void AddItemCost(size_t count);
    void RemoveItemFromMap(size_t index) const;
    void GenerateThings(size_t delta_time, size_t dogs_count) const;
    void SetTypesCount(size_t count);
    void AddItem(size_t index, const Item& item) const;
    const Item GetLostThingViaIndex(size_t index) const noexcept;

private:
    using OfficeIdToIndex = std::unordered_map<Office::Id, size_t, util::TaggedHasher<Office::Id>>;

    Id id_;
    std::string name_;
    Roads roads_;
    Buildings buildings_;
    Offices offices_;
    RoadMap dog_map_; 
    Points items_cost_;
    mutable LostThings lost_things_;

    double dog_speed_;
    size_t bag_capacity_;

    OfficeIdToIndex warehouse_id_to_index_;
};
}  // namespace model