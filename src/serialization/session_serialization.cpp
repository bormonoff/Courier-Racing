#include "serialization/session_serialization.h"

namespace serialization {

GameSessionRepr::GameSessionRepr(const game_session::GameSession& source)
    : map_{*source.GetMap().GetId()} {
    size_t Id{0};
    for (auto& [token, player] : source.GetPlayerTokens().GetPlayers()) {
        dog_tokens_.emplace(token, DogRepr{player.GetDog(), Id++});
    }

    for (auto& [Id, item] : source.GetMap().GetLostThings()) {
        lost_things_.emplace(Id, item);
    }
    dogID_ = Id;
}

void GameSessionRepr::Recover(game_session::GameSession& result) {
    result.SetIdCount(dogID_);
    for (auto& [index, item] : lost_things_) {
        result.GetMap().AddItem(index, item);
    }
    for (auto& [token, dog] : dog_tokens_) {
        std::string tok = token;
        result.AddDog(tok, dog.Recover());
    }
}

void GameSessionStorage::AddSession(GameSessionRepr& session) {
    sessions_.push_back(session);
}
}  // namespace serialization