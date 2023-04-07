#pragma once

#include <pqxx/connection>
#include <pqxx/transaction>

#include "session/dog.h"
#include "postgres/connection_pool.h"

namespace postgres {

using Leaders = std::vector<game_session::LeaderInfo>;

class DogReposiotoryImpl : public game_session::DogRepository {
public:
    explicit DogReposiotoryImpl(postgres::ConnectionPool& connection)
        : connection_{connection} {
    }

    void Save(game_session::LeaderInfo& dog) override;
    Leaders GetLeaders() override;

private:
    postgres::ConnectionPool& connection_;
    Leaders leaders_;
};

class DataBase {
public:
    explicit DataBase(size_t num_threads, const char* db_url);

    DogReposiotoryImpl& GetDogs() {
        return dogs_;
    }

private:
    postgres::ConnectionPool connections_;
    DogReposiotoryImpl dogs_{connections_};
};
} // namespace postgres