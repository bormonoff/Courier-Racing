#pragma once

#include "session/dog.h"

namespace game_session{

class Player {
public:
    explicit Player(const std::string& token, Dog& dog);

    Player() = delete;
    Player& operator=(const Player&) = delete;

    const Dog& GetDog() const {
        return dog_;
    }

    const std::string& GetToken() const {
        return token_;
    }
    
    const std::string& GetDogName() const {
        return dog_.GetName();
    }

    const size_t& GetDogID() const {
        return dog_.GetID();
    }

    const Coordinate& GetDogStart() const {
        return dog_.GetStartPoint();
    }

    const Speed& GetDogSpeed() const {
        return dog_.GetSpeed();
    }

    const std::string GetDogDirection() const {
        return dog_.GetDirection();
    }

    const size_t GetDogScore() const {
        return dog_.GetDogScore();
    }

    void SetDogSpeed(std::string&& direction);
    
private:
    Dog& dog_;
    const std::string token_;
};
}  // namespace game_session