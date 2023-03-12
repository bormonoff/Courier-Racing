#pragma once

#include <string>

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
    Dog(std::string name, size_t id, Coordinate start, double speed)
        :name_{name}, coordinate_{start}, default_speed{speed} {
            id_ = id;
            speed_.dx = 0;
            speed_.dy = 0;
            direction_ = Direction::UP;
    }

    

    const uint16_t& GetID() const {
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

    void SetSpeed(std::string&& drection);
    void MoveDog(Coordinate& target);
    void SetCoordX(double x);
    void SetCoordY(double y);
    const std::string GetDirection() const;   
    Coordinate TargetPosition(size_t& time);

private:
    std::string name_;
    uint16_t id_;
    static uint32_t count;
    const double default_speed;

    Coordinate coordinate_;
    Speed speed_;
    uint8_t direction_;
};

}  // namespace game_session