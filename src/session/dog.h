#pragma once

#include <string>
#include <vector>

#include "model/model.h"

namespace game_session{

struct Coordinate {
    double x, y;
};

struct Speed {
    double dx, dy;
};

enum Direction { 
    LEFT,
    UP,
    RIGHT,
    DOWN
};

class Dog {
public:
    using Items = std::vector<model::Item>;

    Dog(std::string name, size_t id, Coordinate start, double speed)
        :name_{name}, coordinate_{start}, default_speed{speed} {
            id_ = id;
            speed_.dx = 0; 
            speed_.dy = 0;
            direction_ = Direction::UP;
    }

    const size_t& GetID() const {
        return id_;
    }

    const std::string& GetName() const {
        return name_;
    }

    const Coordinate& GetStartPoint() const {
        return coordinate_;
    }

    const Speed& GetSpeed() const {
        return speed_;
    }

    const Items&  GetItemsInBag() const {
        return items_in_bag_;
    }

    const size_t GetItemCount() const {
        return items_in_bag_.size();
    }

    void SetSpeed(std::string&& drection);
    void MoveDog(Coordinate& target);
    void SetCoordX(double x);
    void SetCoordY(double y);
    void CollectItem(model::Item item);
    void ClearBag();

    const std::string GetDirection() const;   
    Coordinate TargetPosition(size_t& time);

private:
    std::string name_;
    size_t id_;
    static size_t count;
    const double default_speed;

    Coordinate coordinate_;
    Speed speed_;
    size_t direction_;
    Items items_in_bag_;
};

}  // namespace game_session