#include <map>
#include <optional>
#include "boost/json.hpp"
#include "../model/model.h"
#include "../core/http_server.h"
#include "../session/game_session.h"
#include <chrono>
#include <functional>
#include "../time/ticker.h"

namespace http_handler{

namespace http = boost::beast::http;
namespace json = boost::json;
namespace net = boost::asio;

using Strand = net::strand<net::io_context::executor_type>;

struct ContentType {
    ContentType() = delete;
    constexpr static std::string_view TYPE_TEXT_HTML = "text/html";
    constexpr static std::string_view TYPE_JSON = "application/json";
    constexpr static std::string_view NO_CACHE = "no-cache";
};

http::response<http::string_body> NotFound(const http::request<http::string_body>& req);

class Application{
public:
    Application(model::Game& game, Strand& strand, size_t period, bool random_spawn)
        : game_{game}
        , strand_{strand}
        , period_{std::chrono::milliseconds(period)}
        , random_spawn_{random_spawn}{
        if(period){
            ticker_ = std::make_shared<time_control::Ticker>(
                strand_, 
                period_, 
                std::bind(&Application::UpdateState, this, std::placeholders::_1)
            );
            ticker_->Start();
        }
    }

    const model::Game& GetGame() const;
    http::response<http::string_body> GetPlayers(const http::request<http::string_body>& req);
    http::response<http::string_body> GetState(const http::request<http::string_body>& req);
    http::response<http::string_body> MoveDogs(const http::request<http::string_body>& req);
    std::optional<game_session::Player> FindPlayerViaToken(std::string& token);
    std::optional<game_session::GameSession> FindSessionViaToken(std::string& token);
    http::response<http::string_body> JoinGame(const http::request<http::string_body>& req);
    http::response<http::string_body> Authorization(const http::request<http::string_body>& req, const game_session::Player& player);
    http::response<http::string_body> ReturnMapsArray(const http::request<http::string_body>& req);
    http::response<http::string_body> ChangeSpeed(const http::request<http::string_body>& req);
    http::response<http::string_body> ChangeDirectory(const http::request<http::string_body>& req, game_session::Player& player);
   
    void UpdateState(size_t milliseconds){
        for(auto& it : sessions_){
            it.second.MakeOffset(milliseconds);
        }
    }

private:
    model::Game& game_;
    bool random_spawn_;
    Strand& strand_;
    std::shared_ptr<time_control::Ticker> ticker_;
    std::chrono::milliseconds period_;
    std::map<std::string, game_session::GameSession> sessions_;
};

http::response<http::string_body> OkResponse(const http::request<http::string_body>& req);
http::response<http::string_body> FindAllPlayerStatesOnMap(const http::request<http::string_body> &req, const game_session::GameSession& session);
http::response<http::string_body> FindAllPlayersOnMap(const http::request<http::string_body> &req, const game_session::GameSession& session);
http::response<http::string_body> MethodNotAllowed(const http::request<http::string_body>& req);
http::response<http::string_body> CantAuthorize(const http::request<http::string_body> &req);
http::response<http::string_body> CantFindPlayer(const http::request<http::string_body>& req);
http::response<http::string_body> NotAllowed(const http::request<http::string_body>& req);
http::response<http::string_body> InvalidPlayerName(const http::request<http::string_body>& req);
http::response<http::string_body> ParseError(const http::request<http::string_body>& req);
http::response<http::string_body> CantAuthorize(const http::request<http::string_body> &req);
http::response<http::string_body> ReturnMap(const model::Map* const this_map, const http::request<http::string_body>& req);
http::response<http::string_body> BadRequest(const http::request<http::string_body>& req);
http::response<http::string_body> MethodGETAllowed(const http::request<http::string_body>& req);
http::response<http::string_body> TickFail(const http::request<http::string_body>& req);
json::array ReturnRoads(const model::Map* const this_map);
json::array ReturnBuildings(const model::Map* const this_map);
json::array ReturnOffices(const model::Map* const this_map);


} //namespace