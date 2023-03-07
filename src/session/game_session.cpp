#include "session/game_session.h"

namespace game_session{

const Player& GameSession::AddDog(std::string name, bool randomize) {
    if (randomize) {
        auto& road = session_map_.GetRoads()[util::GetRandomNumber(0, 
                                             session_map_.GetRoads().size())];
        float x,y;
        if (road.IsVertical()) {
            x = road.GetStart().x;
            size_t begin = road.GetStart().y;
            size_t end = road.GetEnd().y;
            y = util::GetRandomNumber(0, 30)+ 0.1 * util::GetRandomNumber(0, 9);
            dogs_.push_back(Dog{std::move(name), Coordinate{x,y}, 
                            session_map_.GetDogSpeed()});
        } else {
            y = road.GetStart().y;
            size_t begin = road.GetStart().x;
            size_t end = road.GetEnd().x;
            x = util::GetRandomNumber(begin, end - 1) + 0.1 * util::GetRandomNumber(0, 9);
            dogs_.push_back(Dog{std::move(name), Coordinate{x,y}, 
                            session_map_.GetDogSpeed()});
        }
        return players_.AddPlayer(dogs_.back());
    } else {
        auto& road = session_map_.GetRoads()[0];
        float x,y;
        if (road.IsVertical()) {
            x = road.GetStart().x;
            y = road.GetStart().y;
            dogs_.push_back(Dog{std::move(name), Coordinate{x,y}, 
                            session_map_.GetDogSpeed()});
        } else {
            y = road.GetStart().y;
            x = road.GetStart().x;              
            dogs_.push_back(Dog{std::move(name), Coordinate{x,y}, 
                            session_map_.GetDogSpeed()});
        }
        return players_.AddPlayer(dogs_.back());
    }
}

const std::optional<Player> GameSession::FindPlayer(std::string& token) {
    std::optional<Player> player = players_.FindPlayer(token);
    return player;
}

bool GameSession::TargetInsideRoad(const model::Road& road, Coordinate& target) {
    if(road.GetStart().y - 0.4 <= target.y && road.GetEnd().y + 0.4 >= target.y &&
       road.GetStart().x - 0.4 <= target.x && road.GetEnd().x + 0.4 >= target.x) {
        return true;
    }
    return false;
}

Coordinate& GameSession::CalculateResult(const model::Road& road, 
                                         Coordinate& target, Dog& dog){
    if (TargetInsideRoad(road, target)){
        return target;
    }
    if (target.x < road.GetStart().x - 0.4) {
        target.x = road.GetStart().x - 0.4; }

    if (target.x > road.GetEnd().x + 0.4) {
        target.x = road.GetEnd().x + 0.4; }

    if (target.y < road.GetStart().y - 0.4) {
        target.y = road.GetStart().y - 0.4; }

    if (target.y > road.GetEnd().y + 0.4) {
        target.y = road.GetEnd().y + 0.4; }

    dog.SetSpeed("");
    return target;
}

FindRoadOpt GameSession::FindRoad(const std::vector<model::Road>& roads,
                                  Dog& dog) const {
    FindRoadOpt result;
    for (const auto& it : roads) { 
        if (dog.GetStartPoint().x < it.GetStart().x - 0.4) {continue; }
        if (dog.GetStartPoint().x > it.GetEnd().x + 0.4) {continue; }
        if (dog.GetStartPoint().y < it.GetStart().y - 0.4) {continue; }
        if (dog.GetStartPoint().y > it.GetEnd().y + 0.4) {continue; }
        if (it.IsHorizontal()) {result.first = it; }
        if (it.IsVertical()) {result.second = it; }
    }        
    return result;
}

void GameSession::MakeOffset(size_t& time) {
    for (auto& it : dogs_) {      
        FindRoadOpt CurrentRoad = FindRoad(session_map_.GetRoadMap(), it);
        Coordinate target = it.TargetPosition(time);
        if (it.GetDirection() == "R" || it.GetDirection() == "L") {
            if (CurrentRoad.first != std::nullopt) {
                CalculateResult(*CurrentRoad.first, target, it);
            }else{
                CalculateResult(*CurrentRoad.second, target, it);
            }
        }
        if (it.GetDirection() == "U" || it.GetDirection() == "D") {
            if (CurrentRoad.second != std::nullopt) {
                CalculateResult(*CurrentRoad.second, target, it);
            } else {
                CalculateResult(*CurrentRoad.first, target, it);
            }
        }
        it.MoveDog(target);
    }
}

void GameSession::GenerateThingsOnMap(size_t time) { 
    session_map_.GenerateThings(time, dogs_.size());
}
}  // namespace game_session