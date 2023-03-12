#pragma once

#include <deque>

#include "model/model.h"
#include "session/dog.h"
#include "session/player_token.h"
#include "util/utils.h"

namespace game_session {

using FindRoadOpt = std::pair<std::optional<model::Road>, std::optional<model::Road>>;

class GameSession {
public:
    GameSession(const model::Map& map)
        : session_map_{map}{}

    const PlayerTokens& GetPlayerTokens() const {
        return players_;
    }

    const model::Map& GetMap() const {
        return session_map_;
    }

    const Player& AddDog(std::string name, bool randomize);
    const std::optional<Player> FindPlayer(std::string& token);
    bool TargetInsideRoad(const model::Road& road, Coordinate& target);
    Coordinate& CalculateResult(const model::Road& road, Coordinate& target, 
                                Dog& dog);
    FindRoadOpt FindRoad(const std::vector<model::Road>& roads, Dog& dog) const;
    void MakeOffset(size_t& time);
    void GenerateThingsOnMap(size_t time);
    
private:
    const model::Map& session_map_;
    std::deque<Dog> dogs_;
    PlayerTokens players_;
    size_t dogID_ {0};
};
}  // namespace game_session