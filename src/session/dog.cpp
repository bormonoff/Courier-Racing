#include <exception>

#include "session/dog.h"

namespace game_session{

Dog::Dog(std::string name, size_t id, Coordinate start, double speed, size_t lifetime_s)
    : name_{name}, coordinate_{start}, default_speed_{speed}, scored_points_{0}, 
      retriement_time_{lifetime_s}, life_time_{0}, play_time_{0} {
        id_ = id;
        speed_.dx = 0; 
        speed_.dy = 0;
        direction_ = Direction::UP;
}

const std::string Dog::GetDirection() const {
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

void Dog::SetSpeed(std::string&& direction) {
    if (direction.empty()) {
        speed_.dx = 0;
        speed_.dy = 0;
        return;
    }

    switch (direction.at(0)) {
        case ('L'):
            direction_ = Direction::LEFT;
            speed_.dx = -default_speed_;
            speed_.dy = 0.0;
            return;
        case ('U'):
            direction_ = Direction::UP;
            speed_.dx = 0;
            speed_.dy = -default_speed_;
            return;
        case ('R'):
            direction_ = Direction::RIGHT;
            speed_.dx = default_speed_;
            speed_.dy = 0.0;
            return;
        case ('D'):
           direction_ = Direction::DOWN;
            speed_.dx = 0;
            speed_.dy = default_speed_;
            return;
    }
    throw "Error in dog SetSpeed func";
}

Coordinate Dog::TargetPosition(size_t& time) {
    return Coordinate{coordinate_.x + speed_.dx * time* 0.001, 
                      coordinate_.y + speed_.dy * time * 0.001};
}

void Dog::SetCoordY(double y) {
    coordinate_.y = y;
}

void Dog::CollectItem(model::Item item) {
    items_in_bag_.push_back(item);
}

void Dog::ClearBag(const model::Map& current_map) {
    for (auto& it : items_in_bag_) {
        scored_points_ += current_map.GetScoreArray().at(it.type);
    }
    items_in_bag_.clear();
}

void Dog::SetCoordX(double x) {
    coordinate_.x = x;
}

void Dog::MoveDog(Coordinate& target) {
    coordinate_.x = target.x;
    coordinate_.y = target.y;
}

void Dog::SetDirectionViaInt(size_t direction) {
    direction_ = direction;
} 

void Dog::SetSpeed(Speed& speed) {
    speed_ = speed;
}
void Dog::AddScorePoints(size_t points_to_add) {
    scored_points_ += points_to_add;
}

bool Dog::CalculateLive(const std::chrono::milliseconds& interval, bool move) {
        play_time_ += interval;
        if (move) {
            life_time_ = std::chrono::milliseconds{0};
            return true;
        }
        life_time_ += interval;
        if (life_time_ > retriement_time_) {
            return false;
        }
        return true;
}

bool Dog::IsMoving() const {
        if(speed_.dx == 0.0 && speed_.dy == 0.0) {
            return false;
        }
    return true;
}

void Dog::SetId(size_t id) {
    id_ = id;
}

bool operator==(const Dog& first, const Dog& second) {
    return first.GetName() == second.GetName() &&
           first.GetDefaultSpeed() == second.GetDefaultSpeed() && 
           first.GetStartPoint() == second.GetStartPoint() &&
           first.GetSpeed() == second.GetSpeed() &&
           first.GetDirection() == second.GetDirection() &&
           first.GetItemsInBag() == second.GetItemsInBag() &&
           first.GetDogScore() == second.GetDogScore() &&
           first.GetRetriementTime() == second.GetRetriementTime();
}
}  // namespace game_session