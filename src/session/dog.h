#pragma once

#include <string>


namespace game_session{

struct Coordinate{
    double x, y;
};

struct Speed{
    double dx, dy;
};

enum Direction{
    LEFT,
    UP,
    RIGHT,
    DOWN
};

class Dog{
public:
    Dog(std::string name, Coordinate start, double speed)
        :name_{name},start_coordinate_{start}, default_speed{speed}{
            id_ = count;
            count++;
            speed_.dx = 0;
            speed_.dy = -speed;
            direction_ = Direction::UP;
    }

    void SetSpeed(std::string& drection);
    const uint16_t& GetID() const {return id_;}
    const std::string& GetName() const {return name_;}
    const Coordinate& GetStartPoint() const {return start_coordinate_;}
    const Speed& GetSpeed() const {return speed_;}
    const std::string GetDirection() const;   

private:
    std::string name_;
    uint16_t id_;
    static uint32_t count;
    const double default_speed;

    Coordinate start_coordinate_;
    Speed speed_;
    uint8_t direction_;
};

} // namespace