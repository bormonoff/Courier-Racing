#pragma once 

#include "postgres/postgres.h"

namespace http_handler {

using Leaders = std::vector<game_session::LeaderInfo>;

class UseCases {
public:
    explicit UseCases(postgres::DataBase& db) 
        : dogs_{db.GetDogs()} {
    }

    void Save(game_session::LeaderInfo& dog) {
        dogs_.Save(dog);
    }

    Leaders GetLeaders();
    
private:
    game_session::DogRepository& dogs_;
};
}  // namespace http_handler