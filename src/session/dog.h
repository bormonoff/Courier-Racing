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
        :name_{name}, coordinate_{start}, default_speed{speed}{
            id_ = count;
            count++;
            speed_.dx = 0;
            speed_.dy = 0;
            direction_ = Direction::UP;
    }

    void SetSpeed(std::string&& drection);
    void MoveDog(Coordinate& target){
        coordinate_.x = target.x;
        coordinate_.y = target.y;
    }

    void SetCoordX(double x){
        coordinate_.x = x;
    }
    void SetCoordY(double y){
        coordinate_.y = y;
    }
    const uint16_t& GetID() const {return id_;}
    const std::string& GetName() const {return name_;}
    const Coordinate& GetStartPoint() const {return coordinate_;}
    const Speed& GetSpeed() const {return speed_;}
    const std::string GetDirection() const;   
    Coordinate TargetPosition(size_t& time){
        return Coordinate{coordinate_.x + speed_.dx * time* 0.001, 
                          coordinate_.y + speed_.dy * time * 0.001};
    }

private:
    std::string name_;
    uint16_t id_;
    static uint32_t count;
    const double default_speed;

    Coordinate coordinate_;
    Speed speed_;
    uint8_t direction_;
};

} // namespace