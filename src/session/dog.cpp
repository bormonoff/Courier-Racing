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

}

} // namespace