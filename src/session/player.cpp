#include "session/player.h"

namespace game_session{

Player::Player(const std::string& token, Dog& dog)
        : dog_{dog}, token_{token} {};

void Player::SetDogSpeed(std::string&& direction) { 
    dog_.SetSpeed(std::move(direction));
}

void Player::SetDogId(size_t id) {
    dog_.SetId(id);
}

bool Player::CalculateLifetime(const std::chrono::milliseconds& interval) {
    return dog_.CalculateLive(interval, dog_.IsMoving());
}
    
}  // namespace game_session