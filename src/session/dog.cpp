#include "dog.h"

namespace game_session{

uint32_t Dog::count = 0;

const std::string Dog::GetDirection() const{
    switch (direction_){
        case Direction::LEFT:
            return "L";
        case Direction::UP:
            return "U";
        case Direction::RIGHT:
            return "R";
        case Direction::DOWN:
            return "D";
    }
    throw "can't make response dog::GetDirection()";
}

void Dog::SetSpeed(std::string& direction){
    if(direction.empty()){
        speed_.dx = 0;
        speed_.dy = 0;
        return;
    }

    switch (direction.at(0)){
        case ('L'):
            direction_ = Direction::LEFT;
            speed_.dx = -default_speed;
            speed_.dy = 0.0;
            return;
        case ('U'):
            direction_ = Direction::UP;
            speed_.dx = 0;
            speed_.dy = -default_speed;
            return;
        case ('R'):
            direction_ = Direction::RIGHT;
            speed_.dx = default_speed;
            speed_.dy = 0.0;
            return;
        case ('D'):
           direction_ = Direction::DOWN;
            speed_.dx = 0;
            speed_.dy = default_speed;
            return;
    }
    throw "Invalid move field";
}
} // namespace