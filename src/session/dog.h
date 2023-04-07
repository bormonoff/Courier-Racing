#pragma once

#include <string>
#include <vector>

#include "model/game.h"

namespace game_session{

struct Coordinate {
    double x, y;

    constexpr auto operator<=>(const Coordinate&) const = default;
};

struct Speed {
    double dx, dy;

    constexpr auto operator<=>(const Speed&) const = default;
};

enum Direction { 
    LEFT,
    UP,
    RIGHT,
    DOWN
};

struct LeaderInfo {
    std::string name;
    size_t scored_points;
    size_t seconds_in_game;
};

using Leaders = std::vector<game_session::LeaderInfo>;

class Dog {
public:
    using Items = std::vector<model::Item>;

    explicit Dog(std::string name, size_t id, Coordinate start, double speed, 
                 size_t lifetime_ = 0);

    Dog() = delete;
    Dog& operator=(const Dog&) = delete;

    const size_t& GetID() const {
        return id_;
    }

    const std::string& GetName() const {
        return name_;
    }

    const Coordinate& GetStartPoint() const {
        return coordinate_;
    }

    double GetDefaultSpeed() const {
        return default_speed_;
    }

    const Speed& GetSpeed() const {
        return speed_;
    }

    const Items& GetItemsInBag() const {
        return items_in_bag_;
    }
    
    const size_t GetItemCount() const {
        return items_in_bag_.size();
    }

    const size_t GetDogScore() const {
        return scored_points_;
    }

    const size_t GetDirectionViaInt() const {
        return direction_;
    } 

    size_t GetRetriementTime() const {
        return retriement_time_.count();
    }

    size_t GetPlayTime() const {
        return play_time_.count();
    }

    void SetId(size_t id);
    void SetDirectionViaInt(size_t direction);
    void SetSpeed(std::string&& drection);
    void SetSpeed(Speed& speed);
    void SetCoordX(double x);
    void SetCoordY(double y);

    void AddScorePoints(size_t points_to_add);
    void MoveDog(Coordinate& target);
    void CollectItem(model::Item item);
    void ClearBag(const model::Map& current_map);
    bool CalculateLive(const std::chrono::milliseconds& interval, bool move);
    bool IsMoving() const;
    const std::string GetDirection() const;   
    Coordinate TargetPosition(size_t& time);

private:
    size_t id_;
    std::string name_;
    
    size_t direction_;
    Coordinate coordinate_;
    size_t scored_points_;
    Items items_in_bag_;

    Speed speed_;
    const double default_speed_;

    std::chrono::milliseconds play_time_;
    std::chrono::milliseconds life_time_;
    const std::chrono::milliseconds retriement_time_;
};

bool operator==(const Dog& first, const Dog& second);

class DogRepository {
public:
    virtual void Save(LeaderInfo& dog) = 0;
    virtual Leaders GetLeaders() = 0;

protected:
    ~DogRepository() = default;
};
}  // namespace game_session