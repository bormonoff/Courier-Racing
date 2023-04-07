#include "session/player_token.h"

namespace game_session{
    
const Player& PlayerTokens::AddPlayer(Dog& dog) {
    const std::string token = GenerateToken();
    players_.emplace(token, Player{token,dog}); 
    return players_.find(token)->second;
}

void PlayerTokens::AddPlayer(Player& player) {
    players_.emplace(player.GetToken(), player);
}

const std::optional<Player> PlayerTokens::FindPlayer(std::string token) {
    if (players_.find(token) != players_.end()) {
        std::optional<Player> player = players_.find(token)->second;
        return player;
    }
    return std::nullopt;
}

Leaders PlayerTokens::CalculateLifetime(const std::chrono::milliseconds& interval) {
    Leaders result;
    for (auto it = players_.begin(); it != players_.end();) {
        if(!it->second.CalculateLifetime(interval)) {
            auto& dog = it->second.GetDog();
            result.emplace_back(dog.GetName(), dog.GetDogScore(), dog.GetPlayTime());
            it = players_.erase(it);
            continue;
        }
        ++it;
    }
    return result;
}

std::string GenerateToken() {
    std::stringstream ss;
    std::string result;
    for (int i = 0; i < 32; ++i){
        size_t element = util::GetRandomNumber(1, 15);
        if (element > 9) { 
            if(element == 10){ss << 'a'; continue;}
            if(element == 11){ss << 'b'; continue;}
            if(element == 12){ss << 'c'; continue;}
            if(element == 13){ss << 'd'; continue;}
            if(element == 14){ss << 'e'; continue;}
            if(element == 15){ss << 'f'; continue;}
        }
        ss << element;
    }
    getline(ss, result);
    return result;
}
} // namespace