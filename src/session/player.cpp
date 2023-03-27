#include "session/player.h"

namespace game_session{

Player::Player(const std::string& token, Dog& dog)
        : dog_{dog}, token_{token} {};

void Player::SetDogSpeed(std::string&& direction) { 
    dog_.SetSpeed(std::move(direction));
}
}  // namespace game_session