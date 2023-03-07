#pragma once

#include <chrono>
#include <functional>
#include <map>
#include <optional>

#include "boost/json.hpp"

#include "core/http_server.h"
#include "handlers/api_response_storage.h"
#include "json/json_loot_types_storage.h"
#include "model/model.h"
#include "time/ticker.h"

namespace http_handler {

namespace net = boost::asio;
namespace http = boost::beast::http;
namespace json = boost::json;
namespace sys = boost::system;

using Strand = net::strand<net::io_context::executor_type>;
using Response = http::response<http::string_body>;
using Request = http::request<http::string_body>;

class Application {
public:
    explicit Application(model::Game& game, Strand& strand, 
                         size_t period, bool random_spawn, 
                         json_loot::LootTypes&& loot_types)
        : game_{game},
          strand_{strand},
          period_{std::chrono::milliseconds(period)},
          random_spawn_{random_spawn},
          loot_{std::move(loot_types)} {
        if(period){
            ticker_ = std::make_shared<time_control::Ticker>(
                strand_, 
                period_, 
                std::bind(&Application::UpdateState, this, std::placeholders::_1)
            );
            ticker_->Start();
        }
    }

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    const model::Game& GetGame() const;

    Response GetPlayers(const Request& req);
    Response GetState(const Request& req);
    Response MoveDogs(const Request& req);
    Response JoinGame(const Request& req);
    Response Authorization(const Request& req, const game_session::Player& player);
    Response ReturnMapsArray(const Request& req);
    Response ChangeSpeed(const Request& req);
    Response ChangeDirectory(const Request& req, game_session::Player& player);
    Response ReturnMap(const model::Map* const this_map, const Request& req);
    std::optional<game_session::Player> FindPlayerViaToken(std::string& token);
    std::optional<game_session::GameSession> FindSessionViaToken(std::string& token);
    void UpdateState(size_t milliseconds);

    json_loot::LootTypes loot_;
private:
    bool random_spawn_;
    model::Game& game_;
    Strand& strand_;
    std::shared_ptr<time_control::Ticker> ticker_;
    std::chrono::milliseconds period_;
    std::map<std::string, game_session::GameSession> sessions_;
};
} //namespace http_handler
