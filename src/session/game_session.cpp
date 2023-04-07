#include "session/game_session.h"

#include <iostream>

namespace game_session{

GameSession::GameSession(const model::Map& map)
    : session_map_{map}, 
      dogID_{0} {
}

const Player& GameSession::AddDog(std::string name, bool randomize) {
    if (randomize) {
        size_t index;
        if (session_map_.GetRoads().size() == 1) {
            index = 0;
        } else {
            index = util::GetRandomNumber(0, session_map_.GetRoads().size() - 1);
        }
        auto& road = session_map_.GetRoads()[index];
        float x,y;
        if (road.IsVertical()) {
            x = road.GetStart().x;
            size_t begin = road.GetStart().y;
            size_t end = road.GetEnd().y;
            if (begin > end) {
                std::swap(begin, end);
            }
            y = util::GetRandomNumber(begin, end - 1) + 0.1 * util::GetRandomNumber(0, 9);
            dogs_.push_back(Dog{std::move(name), dogID_++, Coordinate{x,y}, 
                            session_map_.GetDogSpeed(), session_map_.GetLifetime()});
        } else {
            y = road.GetStart().y;
            size_t begin = road.GetStart().x;
            size_t end = road.GetEnd().x;
            if (begin > end) {
                std::swap(begin, end);
            }
            x = util::GetRandomNumber(begin, end - 1) + 0.1 * util::GetRandomNumber(0, 9);
            dogs_.push_back(Dog{std::move(name), dogID_++, Coordinate{x,y}, 
                            session_map_.GetDogSpeed(), session_map_.GetLifetime()});
        }
        return players_.AddPlayer(dogs_.back());
    } else {
        auto& road = session_map_.GetRoads()[0];
        float x,y;
        if (road.IsVertical()) {
            x = road.GetStart().x;
            y = road.GetStart().y;
            dogs_.push_back(Dog{std::move(name), dogID_++, Coordinate{x,y}, 
                            session_map_.GetDogSpeed(), session_map_.GetLifetime()});
        } else {
            y = road.GetStart().y;
            x = road.GetStart().x;              
            dogs_.push_back(Dog{std::move(name), dogID_++, Coordinate{x,y}, 
                            session_map_.GetDogSpeed(), session_map_.GetLifetime()});
        }
        return players_.AddPlayer(dogs_.back());
    }
}

const std::optional<Player> GameSession::FindPlayer(const std::string& token) {
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

std::vector<LeaderInfo> GameSession::CalculateLifetime(const std::chrono::milliseconds& interval) {
    return players_.CalculateLifetime(interval);
}

void GameSession::MakeOffset(size_t& time, DetectData& detector) {
    for (auto& it : dogs_) {      
        FindRoadOpt CurrentRoad = FindRoad(session_map_.GetRoadMap(), it);
        
        // Remember dog start point to add it to ItemGathererProvider
        collision_detector::Point2D start_pos {it.GetStartPoint().x, 
                                               it.GetStartPoint().y};

        Coordinate target = it.TargetPosition(time);
        if (it.GetDirection() == "R" || it.GetDirection() == "L") {
            if (CurrentRoad.first != std::nullopt) {
                CalculateResult(*CurrentRoad.first, target, it);
            } else {
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

        // Add dog to the gatherer provider using dog's start and end points.
        // Dog width is 0.3
        detector.AddGatherer(start_pos, {target.x, target.y}, 0.3, it.GetID());
    }
}

void GameSession::AddItemsToCollisionDetector(DetectData& detector) {
    for (const auto& item : session_map_.GetLostThings()) {

        // Add item to the gatherer provider using item's location.
        // Item width is 0.
        detector.AddItem({item.second.x, item.second.y}, 0.0, item.first);
    }
}

void GameSession::AddOfficesToCollisionDetector(DetectData& detector) {
    size_t office_index {0};
    for (const auto& item : session_map_.GetOffices()) {

        // Add offices to the gatherer provider using their location.
        // Static cast is needed cause we condert ints to doubles
        // Item width is 0.25.
        detector.AddItem({static_cast<double>(item.GetPosition().x), 
                            static_cast<double>(item.GetPosition().y)}, 
                            0.75, office_index++);
    }
}

Dog& GameSession::FindDogByIndex(size_t index) {
    for (auto& it : dogs_) { 
        if (it.GetID() == index) {
            return it;
        }
    }
    return dogs_[0];
}

void GameSession::RemoveItemViaIndex(size_t index) {
    session_map_.RemoveItemFromMap(index);
}

void GameSession::GenerateThingsOnMap(size_t time) { 
    session_map_.GenerateThings(time, players_.GetPlayers().size());
}

void GameSession::SetIdCount(size_t Id) {
    dogID_ = Id;
}

void GameSession::AddDog(std::string& token, Dog dog) {
    game_session::Player player{token, dogs_.emplace_back(dog)};
    players_.AddPlayer(player);
}
}  // namespace game_session