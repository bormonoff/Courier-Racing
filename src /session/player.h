#pragma once

#include "dog.h"

namespace game_session{

class Player{
public:
    Player(const std::string& token, const Dog& dog)
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

private:
    const Dog& dog_;
    const std::string token_;
};
} // namespace