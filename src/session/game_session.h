#pragma once

#include <deque>

#include "model/collision_detector.h"
#include "model/game.h"
#include "session/dog.h"
#include "session/player_token.h"
#include "util/utils.h"

namespace game_session {

using FindRoadOpt = std::pair<std::optional<model::Road>, std::optional<model::Road>>;
using DetectData = collision_detector::ItemGathererProvider;

class GameSession {
public:
    explicit GameSession(const model::Map& map);

    GameSession(const GameSession&) = default;
    GameSession& operator=(const GameSession&) = default;

    const PlayerTokens& GetPlayerTokens() const {
        return players_;
    }

    const model::Map& GetMap() const {
        return session_map_;
    }

    const std::deque<Dog> GetDogs() const {
        return dogs_;
    }

    size_t GetIdCount() const {
        return dogID_;
    }

    void SetIdCount(size_t Id);    
    void GenerateThingsOnMap(size_t time);
    void RemoveItemViaIndex(size_t index);
    void MakeOffset(size_t& time, DetectData& detector);
    void AddItemsToCollisionDetector(DetectData& detector);
    void AddOfficesToCollisionDetector(DetectData& detector);
    std::vector<LeaderInfo> CalculateLifetime(const std::chrono::milliseconds& interval);
    bool TargetInsideRoad(const model::Road& road, Coordinate& target);  
    Dog& FindDogByIndex(size_t index);
    FindRoadOpt FindRoad(const std::vector<model::Road>& roads, Dog& dog) const;
    Coordinate& CalculateResult(const model::Road& road, Coordinate& target, 
                                Dog& dog);
    const Player& AddDog(std::string name, bool randomize);
    void AddDog(std::string& token, Dog dog);
    const std::optional<Player> FindPlayer(const std::string& token);
    
private:
    const model::Map& session_map_;
    std::deque<Dog> dogs_;
    PlayerTokens players_;
    size_t dogID_;
};
}  // namespace game_session