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
        //auto& road = session_map_.GetRoads()[util::GetRandomNumber(0, session_map_.GetRoads().size())];
        auto& road = session_map_.GetRoads()[0];
        float x,y;
        if(road.IsVertical()){
            x = road.GetStart().x;
            // size_t begin = road.GetStart().y;
            // size_t end = road.GetEnd().y;
            // y = util::GetRandomNumber(0, 30)+ 0.1 * util::GetRandomNumber(0, 9);
            y = road.GetStart().y;

            dogs_.push_back(Dog{std::move(name), Coordinate{x,y}, session_map_.GetDogSpeed()});
        }else{
            y = road.GetStart().y;
            // size_t begin = road.GetStart().x;
            // size_t end = road.GetEnd().x;
            // x = util::GetRandomNumber(begin, end - 1) + 0.1 * util::GetRandomNumber(0, 9);
            x = road.GetStart().x;              
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

    bool TargetInsideRoad(const model::Road& road, Coordinate& target){
        if(road.GetStart().y - 0.4 <= target.y && road.GetEnd().y + 0.4 >= target.y
           && road.GetStart().x - 0.4 <= target.x && road.GetEnd().x + 0.4 >= target.x){
            return true;
        }
        return false;
    }

    Coordinate& CalculateResult(const model::Road& road, Coordinate& target, Dog& dog){
        if(TargetInsideRoad(road, target)){
            return target;
        }
        if(target.x < road.GetStart().x - 0.4){
            target.x = road.GetStart().x - 0.4; }
        if(target.x > road.GetEnd().x + 0.4){
            target.x = road.GetEnd().x + 0.4; }
        if(target.y < road.GetStart().y - 0.4){
            target.y = road.GetStart().y - 0.4; }
        if(target.y > road.GetEnd().y + 0.4){
            target.y = road.GetEnd().y + 0.4; }
        dog.SetSpeed("");
        return target;
    }

    std::pair<std::optional<model::Road>, std::optional<model::Road>>  FindRoad(const std::vector<model::Road>& roads, Dog& dog) const {
        std::pair<std::optional<model::Road>, std::optional<model::Road>> result;
        for(const auto& it : roads){
            if(dog.GetStartPoint().x < it.GetStart().x - 0.4){continue; }
            if(dog.GetStartPoint().x > it.GetEnd().x + 0.4){continue; }
            if(dog.GetStartPoint().y < it.GetStart().y - 0.4){continue; }
            if(dog.GetStartPoint().y > it.GetEnd().y + 0.4){continue; }
            if(it.IsHorizontal()){result.first = it; }
            if(it.IsVertical()){result.second = it; }
        }        
        return result;
    }

    void MakeOffset(size_t& time){
        for(auto& it : dogs_){      
            std::pair<std::optional<model::Road>, std::optional<model::Road>> CurrentRoad = FindRoad(session_map_.GetRoadMap(), it);
            Coordinate target = it.TargetPosition(time);
            if(it.GetDirection() == "R" || it.GetDirection() == "L"){
                if(CurrentRoad.first != std::nullopt){
                    CalculateResult(*CurrentRoad.first, target, it);
                }else{
                    CalculateResult(*CurrentRoad.second, target, it);
                }
            }
            if(it.GetDirection() == "U" || it.GetDirection() == "D"){
                if(CurrentRoad.second != std::nullopt){
                    CalculateResult(*CurrentRoad.second, target, it);
                }else{
                    CalculateResult(*CurrentRoad.first, target, it);
                }
            }
            it.MoveDog(target);
        }
    }

private:
    const model::Map& session_map_;
    std::deque<Dog> dogs_;
    PlayerTokens players_;
};

} // namespace