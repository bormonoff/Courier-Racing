#pragma once

#include <string>
#include <vector>

#include "model/game.h"

namespace game_session{

struct Coordinate {
    double x, y;

    constexpr auto operator<=>(const Coordinate&) const = default;
};

struct Speed {
    double dx, dy;

    constexpr auto operator<=>(const Speed&) const = default;
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

    explicit Dog(std::string name, size_t id, Coordinate start, double speed);

    Dog() = delete;
    Dog& operator=(const Dog&) = delete;

    const size_t& GetID() const {
        return id_;
    }

    const std::string& GetName() const {
        return name_;
    }

    const Coordinate& GetStartPoint() const {
        return coordinate_;
    }

    double GetDefaultSpeed() const {
        return default_speed;
    }

    const Speed& GetSpeed() const {
        return speed_;
    }

    const Items& GetItemsInBag() const {
        return items_in_bag_;
    }
    
    const size_t GetItemCount() const {
        return items_in_bag_.size();
    }

    const size_t GetDogScore() const {
        return scored_points_;
    }

    const size_t GetDirectionViaInt() const {
        return direction_;
    } 

    void SetDirectionViaInt(size_t direction);
    void SetSpeed(std::string&& drection);
    void SetSpeed(Speed& speed);
    void SetCoordX(double x);
    void SetCoordY(double y);

    void AddScorePoints(size_t points_to_add);
    void MoveDog(Coordinate& target);
    void CollectItem(model::Item item);
    void ClearBag(const model::Map& current_map);
    const std::string GetDirection() const;   
    Coordinate TargetPosition(size_t& time);

private:
    std::string name_;
    size_t id_;
    const double default_speed;
    
    Coordinate coordinate_;
    Speed speed_;
    size_t direction_;
    Items items_in_bag_;
    size_t scored_points_;
};

bool operator==(const Dog& first, const Dog& second);

}  // namespace game_session