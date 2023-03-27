#pragma once

#include <algorithm>
#include <vector>

namespace collision_detector {

struct Point2D {
    double x;
    double y;
};

struct Item {
    Point2D position;
    double width;
    size_t id;
};

struct Gatherer {
    Point2D start_pos;
    Point2D end_pos;
    double width;
    size_t id;
};

struct GatheringEvent {
    size_t item_id;
    size_t gatherer_id;
    double sq_distance;
    double time;
};

struct CollectionResult {
    // Square distance to point
    double sq_distance;

    // Propotion of segment
    double proj_ratio;
};

// Dog move from at to and try to collect c. Func returns square distance 
// from AB vector to dot C and relative percantage of AB vector.
CollectionResult TryCollectPoint(Point2D a, Point2D b, Point2D c);

class ItemGathererProvider {
public:
    ItemGathererProvider() = default;

    ItemGathererProvider(const ItemGathererProvider&) = default;
    ItemGathererProvider& operator=(const ItemGathererProvider&) = default;

    size_t ItemsCount() const {
        return items_.size();
    }
    Item GetItem(size_t idx) const {
        return items_.at(idx);
    }
    size_t GatherersCount() const {
        return gatherers_.size();
    }
    Gatherer GetGatherer(size_t idx) const {
        return gatherers_.at(idx);
    }

    void AddItem(Point2D position, double width, size_t id) {
        items_.emplace_back(position, width, id);
    }

    void AddGatherer(Point2D start, Point2D finish, double width, size_t ID) {
        gatherers_.emplace_back(start, finish, width, ID);
    }

private:
    std::vector<Item> items_;
    std::vector<Gatherer> gatherers_;
};

std::vector<GatheringEvent> FindGatherEvents(const ItemGathererProvider& provider);
}  // namespace collision_detector