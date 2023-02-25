#include "session/player.h"

namespace game_session{

void Player::SetDogSpeed(std::string&& direction) { 
    dog_.SetSpeed(std::move(direction));
}
}  // namespace game_session