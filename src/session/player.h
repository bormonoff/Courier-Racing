#pragma once

#include "dog.h"

namespace game_session{

class Player{
public:
    Player(const std::string& token, Dog& dog)
        : dog_{dog}, token_{token}{};

    const Dog& GetDog() const{
        return dog_;
    }

    const std::string& GetToken() const{
        return token_;
    }
    
    const std::string& GetDogName() const{
        return dog_.GetName();
    }

    const uint16_t& GetDogID() const{
        return dog_.GetID();
    }

    const Coordinate& GetDogStart() const{
        return dog_.GetStartPoint();
    }

    const Speed& GetDogSpeed() const{
        return dog_.GetSpeed();
    }

    const std::string GetDogDirection() const{
        return dog_.GetDirection();
    }

    void SetDogSpeed(std::string&& direction){
        dog_.SetSpeed(std::move(direction));
    }


private:
    Dog& dog_;
    const std::string token_;
};
} // namespace