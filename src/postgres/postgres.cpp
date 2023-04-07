#include "postgres/postgres.h"

#include <pqxx/zview.hxx>

#include "util/utils.h"

namespace postgres {

using pqxx::operator"" _zv;

void DogReposiotoryImpl::Save(game_session::LeaderInfo& dog) {
    
    auto conn = connection_.GetConnection();
    pqxx::work work{*conn};
    work.exec_params(R"(INSERT INTO hall_of_fame (id, name, score, play_time_ms) 
                     VALUES ($1, $2, $3, $4);)"_zv, util::UUIDToString(util::NewUUID()), 
                                                                       dog.name, 
                     dog.scored_points, dog.seconds_in_game);
    work.commit();
}

Leaders DogReposiotoryImpl::GetLeaders() {
    Leaders result;
    auto conn = connection_.GetConnection();
    pqxx::read_transaction read(*conn);
    auto query_text = R"(SELECT name, score, play_time_ms FROM hall_of_fame
                         ORDER BY score DESC, play_time_ms, name)"_zv;
    size_t count{0};
    for (auto [name, score, play_time_ms] : 
            read.query<std::string, size_t, size_t>(query_text)) {
        result.emplace_back(name, score, play_time_ms / 1000);
        if (count++ >+15) { break; }
    }
    return result;
}

DataBase::DataBase(size_t num_threads, const char* db_url) 
    : connections_{num_threads, [db_url] {
        auto conn = std::make_shared<pqxx::connection>(db_url);
        return conn;
        }} {
    auto conn = connections_.GetConnection();
    pqxx::work work{*conn};

    work.exec(R"(CREATE TABLE IF NOT EXISTS hall_of_fame (
              id UUID CONSTRAINT dog_id PRIMARY KEY,
              name varchar(100) NOT NULL, 
              score integer NOT NULL, 
              play_time_ms integer NOT NULL);)"_zv);

    work.exec(R"(CREATE INDEX IF NOT EXISTS dogs_idx ON hall_of_fame (
              score DESC, play_time_ms, name);)"_zv);
    
    work.commit();
}
}
