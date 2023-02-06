#pragma once

#include <string>
#include <sstream>
#include "../util/utils.h"
#include <map>
#include "player.h"
#include <optional>

namespace game_session{

std::string GenerateToken();

class PlayerTokens{
public:
    const Player& AddPlayer(const Dog& dog){
        const std::string token = GenerateToken();
        players_.emplace(token, Player{token,dog}); 
        return players_.find(token)->second;
    }

    const std::optional<Player> FindPlayer(std::string token){
        if(players_.find(token) != players_.end()){
            std::optional<Player> player = players_.find(token)->second;
            return player;
        }
        return std::nullopt;
    }

    const std::map<std::string, Player>& GetPlayers() const{
        return players_;
    }
private:
    std::map<std::string, Player> players_;
};



} // namespace