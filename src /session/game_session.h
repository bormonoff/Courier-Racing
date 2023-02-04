#pragma once

#include "../model/model.h"
#include <deque>
#include "dog.h"
#include "player_token.h"

namespace game_session{

class GameSession{
public:
    GameSession(const model::Map& map)
        : session_map_{map}{}

    const Player& AddDog(std::string name){
        dogs_.push_back(Dog{std::move(name)});
        return players_.AddPlayer(dogs_.back());
    }

    const std::optional<Player> FindPlayer(std::string& token){
        std::optional<Player> player = players_.FindPlayer(token);
        return player;
    }

    const PlayerTokens& GetPlayerTokens() const {
        return players_;
    }

private:
    const model::Map& session_map_;
    std::deque<Dog> dogs_;
    PlayerTokens players_;
};

} // namespace