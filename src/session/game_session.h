#pragma once

#include "../model/model.h"
#include <deque>
#include "dog.h"
#include "player_token.h"
#include "../util/utils.h"

namespace game_session{

class GameSession{
public:
    GameSession(const model::Map& map)
        : session_map_{map}{}

    const Player& AddDog(std::string name){
        auto& road = session_map_.GetRoads()[util::GetRandomNumber(0, session_map_.GetRoads().size())];
        float x,y;
        if(road.IsVertical()){
            x = road.GetStart().x;
            size_t begin = road.GetStart().y;
            size_t end = road.GetEnd().y;
            y = util::GetRandomNumber(0, 30)+ 0.1 * util::GetRandomNumber(0, 9);

            dogs_.push_back(Dog{std::move(name), Coordinate{x,y}, session_map_.GetDogSpeed()});
        }else{
            y = road.GetStart().y;
            size_t begin = road.GetStart().x;
            size_t end = road.GetEnd().x;
            x = util::GetRandomNumber(begin, end - 1) + 0.1 * util::GetRandomNumber(0, 9);

            dogs_.push_back(Dog{std::move(name), Coordinate{x,y}, session_map_.GetDogSpeed()});
        }

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