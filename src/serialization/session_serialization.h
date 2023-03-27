#pragma once 

#include <boost/serialization/map.hpp>
#include <boost/serialization/unordered_map.hpp>

#include "serialization/dog_serialization.h"
#include "serialization/item_serialization.h"
#include "session/game_session.h"

namespace serialization {

class GameSessionRepr {
public:
    explicit GameSessionRepr(const game_session::GameSession& source);

    GameSessionRepr() = default;
    GameSessionRepr& operator=(const GameSessionRepr&) = default;

    template<typename Archive>
    void serialize(Archive& ar, [[maybe_unused]] const unsigned int) {
        ar& lost_things_;
        ar& dog_tokens_;
        ar& dogID_;
        ar& map_;
    }

    const std::map<std::string, serialization::DogRepr>& GetDogInfo() const {
        return dog_tokens_;
    }

    const size_t GetDogId() const {
        return dogID_;
    }

    const model::LostThings::Things GetThings() const {
        return lost_things_;
    }

    const std::string GetMap() const {
        return map_;
    }

    void Recover(game_session::GameSession& result);

private:
    model::LostThings::Things lost_things_;
    std::map<std::string, serialization::DogRepr> dog_tokens_;
    size_t dogID_;
    std::string map_;
};

class GameSessionStorage {
public:
    std::vector<GameSessionRepr> GetSessions() {
        return sessions_;
    }

    template<typename Archive>
    void serialize(Archive& ar, [[maybe_unused]] const unsigned int) {
        ar& sessions_;
    }

    void AddSession(GameSessionRepr& session);

private:
    std::vector<GameSessionRepr> sessions_;
};
}  // namespace serialization