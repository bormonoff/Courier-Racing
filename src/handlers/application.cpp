#include "handlers/application.h"

namespace http_handler{

void Application::UpdateState(size_t milliseconds) {
    for (auto& it : sessions_) {
        it.second.MakeOffset(milliseconds);
    }
}

Response Application::MoveDogs(const Request& req) {
    if (req.method_string() != "POST") {
        return NotAllowed(req);
    }
    size_t tick;
    try {
        json::value parse_req = json::parse(req.body());
        if (!parse_req.as_object().at("timeDelta").is_int64()) {
            throw std::runtime_error("Invalid timeDelta");
        }
        tick = json::value_to<size_t>(parse_req.as_object().at("timeDelta"));
    } catch(...) {
        return ParseError(req); 
    }
    if (period_.count()) {
        return TickFail(req);
    }
    UpdateState(tick);
    return OkResponse(req);
}

Response Application::ChangeSpeed(const Request& req) {
    if (req.method_string() != "POST") {
        return MethodNotAllowed(req);
    }
    std::string token;
    try {
        token = static_cast<std::string>(req[http::field::authorization].substr(7));
        if (token.size() != 32) {
            throw "Wrong token";
        }
    } catch(...) {
        return CantAuthorize(req);
    }
    std::optional<game_session::Player> player = FindPlayerViaToken(token);
    if (player != std::nullopt) {
        return ChangeDirectory(req, *player);
    }
    return CantFindPlayer(req);
}

Response Application::ChangeDirectory(const Request& req, 
                                      game_session::Player& player) {
    Response response{http::status::ok, req.version()};
    response.set(http::field::content_type, ContentType::TYPE_JSON);
    response.set(http::field::cache_control, ContentType::NO_CACHE);
    boost::system::error_code error;
    json::value parse_req = json::parse(req.body(), error);
    if (error) {
        return ParseError(req); 
    }
    std::string move_field;
    try {
        move_field =  static_cast<std::string>(parse_req.at("move").as_string());
    } catch(...) {    
        return ParseError(req);
    }
    if (req[http::field::content_type] != ContentType::TYPE_JSON) {
        return NotAllowed(req);
    }
    try {
        player.SetDogSpeed(std::move(move_field));
    } catch(...) {
        return ParseError(req);
    }
    response.body() = "{}";
    response.content_length(response.body().size());
    return response;
}

Response Application::GetState(const Request& req) {
    if (req.method_string() != "GET") {
        return MethodGETAllowed(req);
    }
    std::string token;
    try {
        token = static_cast<std::string>(req[http::field::authorization].substr(7));
        if (token.size() != 32) {
            throw "Wrong token";
        }
    } catch(...) {
        return CantAuthorize(req);
    }
    std::optional<game_session::GameSession> session = FindSessionViaToken(token);
    if (session != std::nullopt) {
        return FindAllPlayerStatesOnMap(req, *session);
    }
    return CantFindPlayer(req);
}

Response Application::JoinGame(const Request& req) {
    if (req.method_string() != "POST") {
        return NotAllowed(req);
    }
    std::string dog_name;
    std::string map;
    try {
        json::value parse_req = json::parse(req.body());
        dog_name = static_cast<std::string>(parse_req.at("userName").as_string());
        map = static_cast<std::string>(parse_req.at("mapId").as_string());
    } catch(...) {
        return ParseError(req); 
    }
    if (dog_name == "") {
        return InvalidPlayerName(req);
    }
    if (sessions_.find(map) == sessions_.end()) {    
        const model::Map* const map_model = game_.FindMap(util::Tagged<std::string, 
                                                          model::Map>(map));
        if (map_model == nullptr) {
            return NotFound(req);
        }   
        sessions_.emplace(map, game_session::GameSession(*map_model));
    }
    const auto& player = sessions_.find(map) -> second.AddDog(dog_name, random_spawn_);
    return Authorization(req, player);
}

Response Application::GetPlayers(const Request& req) {
    if (req.method_string() != "GET" && req.method_string() != "HEAD") {
        return MethodNotAllowed(req);
    }
    std::string token;
    try {
        token = static_cast<std::string>(req[http::field::authorization].substr(7));
        if (token.size() != 32) {
            throw "Wrong token";
        }
    } catch(...) {
        return CantAuthorize(req);
    }
    std::optional<game_session::GameSession> session = FindSessionViaToken(token);
    if (session != std::nullopt) {
        return FindAllPlayersOnMap(req, *session);
    }
    return CantFindPlayer(req);
}

std::optional<game_session::Player> Application::FindPlayerViaToken(
        std::string& token) {
    for (auto it : sessions_) {
        std::optional<game_session::Player> player = (it.second).FindPlayer(token);
        if (player != std::nullopt) {
            return player;
        }
    }
    return std::nullopt;
}

std::optional<game_session::GameSession> Application::FindSessionViaToken(
        std::string& token) {
    for (auto it : sessions_) {
        std::optional<game_session::Player> player = (it.second).FindPlayer(token);
        if (player != std::nullopt) {
            std::optional<game_session::GameSession> session = it.second;
            return session;
        }
    }
    return std::nullopt;
}

Response Application::Authorization(const Request& req, 
                                    const game_session::Player& player) {
    Response response{http::status::ok, req.version()};
    response.set(http::field::content_type, ContentType::TYPE_JSON);
    response.set(http::field::cache_control, ContentType::NO_CACHE);
    
    json::object json_res;
    std::string pl = player.GetToken();
    json_res["authToken"] = player.GetToken();
    json_res["playerId"] = player.GetDog().GetID();
    response.body() = json::serialize(json_res);
    response.content_length(response.body().size());
    return response;
}

const model::Game& Application::GetGame() const {
    return game_;
}

Response Application::ReturnMapsArray(const Request& req){
    Response response{http::status::ok, req.version()};
    response.set(http::field::content_type, ContentType::TYPE_JSON);

    json::array maps;
    for (const auto& it : game_.GetMaps()) {
        json::object map_it;
        map_it["id"] = *it.GetId();
        map_it["name"] = it.GetName();
        maps.push_back(map_it);
    };
    response.body() = json::serialize(maps);
    response.set(http::field::cache_control, ContentType::NO_CACHE);
    response.content_length(response.body().size());
    return response;
}
} //namespace http_handler