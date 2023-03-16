#include "handlers/application.h"

namespace http_handler {

void Application::UpdateState(size_t milliseconds) {
    for (auto& it : sessions_) {
        DetectData collision_collector;
        it.second.MakeOffset(milliseconds, collision_collector);

        DetectData items_collision_collector {collision_collector};
        it.second.GenerateThingsOnMap(milliseconds);
        it.second.AddItemsToCollisionDetector(items_collision_collector);
        UpdateItemCollisions(items_collision_collector, it.second);

        DetectData base_collision_collector {std::move(collision_collector)};
        it.second.AddOfficesToCollisionDetector(base_collision_collector);
        UpdateOfficeCollisions(base_collision_collector, it.second);
    }
}

void Application::UpdateItemCollisions(DetectData& item_data, 
                                       game_session::GameSession& session) {
    auto item_events = collision_detector::FindGatherEvents(item_data);
    for (auto& event : item_events) {
        game_session::Dog& dog = session.FingDogByIndex(event.gatherer_id);
        if (dog.GetItemCount() <= session.GetMap().GetMaxBagSize()) {
            dog.CollectItem(session.GetMap().GetLostThingViaIndex(event.item_id));
            session.RemoveItemViaIndex(event.item_id);
        }
    }
}

void Application::UpdateOfficeCollisions(DetectData& office_data, 
                                         game_session::GameSession& session) {
    auto office_events = collision_detector::FindGatherEvents(office_data);
    for (auto& event : office_events) {
        game_session::Dog& dog = session.FingDogByIndex(event.gatherer_id);
        if (dog.GetItemCount() <= session.GetMap().GetMaxBagSize()) {
            dog.ClearBag();
        }
    }
}

Response Application::ReturnMap(const model::Map* const this_map, const Request& req) {
    if (req.method_string() != Allow::GET && req.method_string() != Allow::HEAD) {
        return MethodNotAllowed(req);
    }
    json::object map;
        map[ID] = *this_map->GetId();
        map[NAME] = this_map->GetName();
    
    map[ROADS] = ReturnRoads(this_map);
    map[BUILDINGS] = ReturnBuildings(this_map);   
    map[OFFICES] = ReturnOffices(this_map);
    map[LOOTTYPES] = loot_.loot_types_[*this_map->GetId()];

    http::response<http::string_body> response{http::status::ok, req.version()};
    response.set(http::field::content_type, ContentType::TYPE_JSON);
    response.body() = json::serialize(map);
    response.set(http::field::cache_control, ContentType::NO_CACHE);
    response.content_length(response.body().size());
    return response;
}

Response Application::MoveDogs(const Request& req) {
    if (req.method_string() != Allow::POST) {
        return NotAllowed(req);
    }
    size_t tick;
    try {
        json::value parse_req = json::parse(req.body());
        if (!parse_req.as_object().at(TIMEDELTA).is_int64()) {
            throw std::exception();
        }
        tick = json::value_to<size_t>(parse_req.as_object().at(TIMEDELTA));
    } catch(std::exception& ex) {
        return ParseError(req); 
    }
    if (period_.count()) {
        return TickFail(req);
    }
    UpdateState(tick);
    return OkResponse(req);
}

Response Application::ChangeSpeed(const Request& req) {
    if (req.method_string() != Allow::POST) {
        return MethodNotAllowed(req);
    }
    std::string token;
    try {
        token = static_cast<std::string>(req[http::field::authorization].substr(7));
    } catch(std::exception& ex) {
        return CantAuthorize(req);
    }
    if (token.size() != 32) {
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
        move_field =  static_cast<std::string>(parse_req.at(MOVE).as_string());
    } catch(std::exception& ex) {    
        return ParseError(req);
    }
    if (req[http::field::content_type] != ContentType::TYPE_JSON) {
        return NotAllowed(req);
    }
    try {
        player.SetDogSpeed(std::move(move_field));
    } catch(std::exception& ex) {
        return ParseError(req);
    }
    response.body() = EMPTY_BODY;
    response.content_length(response.body().size());
    return response;
}

Response Application::GetState(const Request& req) {
    if (req.method_string() != Allow::GET && req.method_string() != Allow::HEAD) {
        return MethodNotAllowed(req);
    }
    std::string token;
    try {
        token = static_cast<std::string>(req[http::field::authorization].substr(7));
    } catch(std::exception& ex) {
        return CantAuthorize(req);
    }
    if (token.size() != 32) {
        return CantAuthorize(req);
    }
    std::optional<game_session::GameSession> session = FindSessionViaToken(token);
    if (session != std::nullopt) {
        return FindAllPlayerStatesOnMap(req, *session);
    }
    return CantFindPlayer(req);
}

Response Application::JoinGame(const Request& req) {
    if (req.method_string() != Allow::POST) {
        return NotAllowed(req);
    }
    std::string dog_name;
    std::string map;
    try {
        json::value parse_req = json::parse(req.body());
        dog_name = static_cast<std::string>(parse_req.at(USERNAME).as_string());
        map = static_cast<std::string>(parse_req.at(MAPID).as_string());
    } catch(std::exception& ex) {
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
    if (req.method_string() != Allow::GET && req.method_string() != Allow::HEAD) {
        return MethodNotAllowed(req);
    }
    std::string token;
    try {
        token = static_cast<std::string>(req[http::field::authorization].substr(7));
    } catch(std::exception& ex) {
        return CantAuthorize(req);
    }
    if (token.size() != 32) {
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
    json_res[AUTHTOKEN] = player.GetToken();
    json_res[PLAYERID] = player.GetDog().GetID();
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
        map_it[ID] = *it.GetId();
        map_it[NAME] = it.GetName();
        maps.push_back(map_it);
    };
    response.body() = json::serialize(maps);
    response.set(http::field::cache_control, ContentType::NO_CACHE);
    response.content_length(response.body().size());
    return response;
}
} //namespace http_handler