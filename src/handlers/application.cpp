#include "application.h"

namespace http_handler{

http::response<http::string_body> Application::MoveDogs(const http::request<http::string_body>& req){
    if(req.method_string() != "POST"){
        return NotAllowed(req);
    }

    size_t tick;
    try{
        json::value parse_req = json::parse(req.body());
        // tick = std::stoi(json::serialize(parse_req.at("timeDelta")));
        if(!parse_req.as_object().at("timeDelta").is_int64()){
            throw "lll";
        }
        tick = json::value_to<size_t>(parse_req.as_object().at("timeDelta"));
    }catch(...){
        return ParseError(req); 
    }
    for(auto& it : sessions_){
        it.second.MakeOffset(tick);
    }
    
    return OkResponse(req);
}

http::response<http::string_body> OkResponse(const http::request<http::string_body>& req){
    http::response<http::string_body> response{http::status::ok, req.version()};
    response.set(http::field::content_type, ContentType::TYPE_JSON);
    response.set(http::field::cache_control, ContentType::NO_CACHE);

    response.body() = "{}";
    response.content_length(response.body().size());
    return response;
}

http::response<http::string_body> Application::ChangeSpeed(const http::request<http::string_body>& req){
    if(req.method_string() != "POST"){
        return MethodNotAllowed(req);
    }

    std::string token;
    try{
        token = static_cast<std::string>(req[http::field::authorization].substr(7));
        if(token.size() != 32){
            throw "Wrong token";
        }
    }catch(...){
        return CantAuthorize(req);
    }

    std::optional<game_session::Player> player = FindPlayerViaToken(token);
    if(player != std::nullopt){
        return ChangeDirectory(req, *player);
    }

    return CantFindPlayer(req);
}

http::response<http::string_body> Application::ChangeDirectory(const http::request<http::string_body>& req, game_session::Player& player){
    http::response<http::string_body> response{http::status::ok, req.version()};
    response.set(http::field::content_type, ContentType::TYPE_JSON);
    response.set(http::field::cache_control, ContentType::NO_CACHE);

    boost::system::error_code error;
    json::value parse_req = json::parse(req.body(), error);
    if(error){
        return ParseError(req); 
    }

    std::string move_field;
    try{
        move_field =  static_cast<std::string>(parse_req.at("move").as_string());
    }catch(...){    
        return ParseError(req);
    }

    if(req[http::field::content_type] != ContentType::TYPE_JSON){
        return NotAllowed(req);
    }

    try{
        player.SetDogSpeed(std::move(move_field));
    }catch(...){
        return ParseError(req);
    }
    
    response.body() = "{}";
    response.content_length(response.body().size());
    return response;
}

http::response<http::string_body> Application::GetState(const http::request<http::string_body>& req){
    if(req.method_string() != "GET"){
        return MethodGETAllowed(req);
    }

    std::string token;
    try{
        token = static_cast<std::string>(req[http::field::authorization].substr(7));
        if(token.size() != 32){
            throw "Wrong token";
        }
    }catch(...){
        return CantAuthorize(req);
    }
    
    std::optional<game_session::GameSession> session = FindSessionViaToken(token);
    if(session != std::nullopt){
        return FindAllPlayerStatesOnMap(req, *session);
    }

    return CantFindPlayer(req);
}

http::response<http::string_body> FindAllPlayerStatesOnMap(const http::request<http::string_body> &req, const game_session::GameSession& session){
    http::response<http::string_body> response{http::status::ok, req.version()};
    response.set(http::field::content_type, ContentType::TYPE_JSON);
    response.set(http::field::cache_control, ContentType::NO_CACHE);
    
    json::object json_res;
    json::object ID;
    for(auto it : session.GetPlayerTokens().GetPlayers()){
        json::object data;

        json::array position;
        position.push_back(it.second.GetDogStart().x);
        position.push_back(it.second.GetDogStart().y);
        data["pos"] = position;

        json::array speed;
        speed.push_back(it.second.GetDogSpeed().dx);
        speed.push_back(it.second.GetDogSpeed().dy);
        data["speed"] = speed;

        data["dir"] = it.second.GetDogDirection();

        ID[std::to_string(it.second.GetDogID())] = data;
    }
    json_res["players"] = ID;
    std::string response2 = json::serialize(json_res);
    json::value rew = json::parse(response2);
    response.body() = response2;
    response.content_length(response.body().size());
    return response;
}

http::response<http::string_body> Application::JoinGame(const http::request<http::string_body>& req){
    if(req.method_string() != "POST"){
        return NotAllowed(req);
    }

    std::string dog_name;
    std::string map;
    try{
        json::value parse_req = json::parse(req.body());
        dog_name = static_cast<std::string>(parse_req.at("userName").as_string());
        map = static_cast<std::string>(parse_req.at("mapId").as_string());
    }catch(...){
        return ParseError(req); 
    }
    
    if(dog_name == ""){
        return InvalidPlayerName(req);
    }

    if(sessions_.find(map) == sessions_.end()){   
        const model::Map* const map_model = game_.FindMap(util::Tagged<std::string, model::Map>(map));
        if(map_model == nullptr){
            return NotFound(req);
        }   
        sessions_.emplace(map, game_session::GameSession(*map_model));
    }

    const auto& player = sessions_.find(map) -> second.AddDog(dog_name);
    return Authorization(req, player);
}

http::response<http::string_body> NotAllowed(const http::request<http::string_body>& req){
    http::response<http::string_body> response{http::status::method_not_allowed, req.version()};
    response.set(http::field::content_type, ContentType::TYPE_JSON);
    response.set(http::field::cache_control, ContentType::NO_CACHE);
    response.set(http::field::allow, "POST");

    json::object json_res;
    json_res["code"] = "invalidMethod";
    json_res["message"] = "Only POST method is expected :(";
    response.body() = json::serialize(json_res);
    response.content_length(response.body().size());
    return response;
}

http::response<http::string_body> Application::GetPlayers(const http::request<http::string_body>& req){
    if(req.method_string() != "GET" && req.method_string() != "HEAD"){
        return MethodNotAllowed(req);
    }

    std::string token;
    try{
        token = static_cast<std::string>(req[http::field::authorization].substr(7));
        if(token.size() != 32){
            throw "Wrong token";
        }
    }catch(...){
        return CantAuthorize(req);
    }

    std::optional<game_session::GameSession> session = FindSessionViaToken(token);
    if(session != std::nullopt){
        return FindAllPlayersOnMap(req, *session);
    }

    return CantFindPlayer(req);
}

std::optional<game_session::Player> Application::FindPlayerViaToken(std::string& token){
    
    for(auto it : sessions_){
        std::optional<game_session::Player> player = (it.second).FindPlayer(token);
        if(player != std::nullopt){
            return player;
        }
    }
    return std::nullopt;
}

std::optional<game_session::GameSession> Application::FindSessionViaToken(std::string& token){
    for(auto it : sessions_){
        std::optional<game_session::Player> player = (it.second).FindPlayer(token);
        if(player != std::nullopt){
            std::optional<game_session::GameSession> session = it.second;
            return session;
        }
    }
    return std::nullopt;
}

http::response<http::string_body> MethodNotAllowed(const http::request<http::string_body>& req){
    http::response<http::string_body> response{http::status::bad_request, req.version()};
    response.set(http::field::content_type, ContentType::TYPE_JSON);
    response.set(http::field::cache_control, ContentType::NO_CACHE);
    response.set(http::field::allow, "GET, HEAD");

    json::object json_res;
    json_res["code"] = "invalidMethod";
    json_res["message"] = "Invalid Method";
    response.body() = json::serialize(json_res);
    response.content_length(response.body().size());
    return response;
}

http::response<http::string_body> MethodGETAllowed(const http::request<http::string_body>& req){
    http::response<http::string_body> response{http::status::bad_request, req.version()};
    response.set(http::field::content_type, ContentType::TYPE_JSON);
    response.set(http::field::cache_control, ContentType::NO_CACHE);
    response.set(http::field::allow, "GET");

    json::object json_res;
    json_res["code"] = "invalidMethod";
    json_res["message"] = "Invalid Method";
    response.body() = json::serialize(json_res);
    response.content_length(response.body().size());
    return response;
}

http::response<http::string_body> CantAuthorize(const http::request<http::string_body> &req){
    http::response<http::string_body> response{http::status::unauthorized, req.version()};
    response.set(http::field::content_type, ContentType::TYPE_JSON);
    response.set(http::field::cache_control, ContentType::NO_CACHE);
    
    json::object json_res;
    json_res["code"] = "invalidToken";
    json_res["message"] = "Authorization header is miising or token is invalid";
    response.body() = json::serialize(json_res);
    response.content_length(response.body().size());
    return response;
}

http::response<http::string_body> FindAllPlayersOnMap(const http::request<http::string_body> &req, const game_session::GameSession& session){
    http::response<http::string_body> response{http::status::ok, req.version()};
    response.set(http::field::content_type, ContentType::TYPE_JSON);
    response.set(http::field::cache_control, ContentType::NO_CACHE);
    
    json::object json_res;
    size_t count = 0;
    for(auto it : session.GetPlayerTokens().GetPlayers()){
        json::object temp_respond;
        temp_respond["name"] = (it.second).GetDogName();
        json_res[std::to_string(count)] = temp_respond;
        ++count;
    }
    response.body() = json::serialize(json_res);
    response.content_length(response.body().size());
    return response;
}

http::response<http::string_body> CantFindPlayer(const http::request<http::string_body> &req){
    http::response<http::string_body> response{http::status::unauthorized, req.version()};
    response.set(http::field::content_type, ContentType::TYPE_JSON);
    response.set(http::field::cache_control, ContentType::NO_CACHE);
    
    json::object json_res;
    json_res["code"] = "unknownToken";
    json_res["message"] = "Player token has not been found";
    response.body() = json::serialize(json_res);
    response.content_length(response.body().size());
    return response;
}

http::response<http::string_body> Application::Authorization(const http::request<http::string_body> &req, 
                                                            const game_session::Player& player){
    http::response<http::string_body> response{http::status::ok, req.version()};
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

http::response<http::string_body> NotFound(const http::request<http::string_body>& req){
    http::response<http::string_body> response{http::status::not_found, req.version()};
    response.set(http::field::content_type, ContentType::TYPE_JSON);
    response.set(http::field::cache_control, ContentType::NO_CACHE);

    json::object json_res;
    json_res["code"] = "mapNotFound";
    json_res["message"] = "Resource not found";
    response.body() = json::serialize(json_res);
    response.content_length(response.body().size());
    return response;
}

http::response<http::string_body> InvalidPlayerName(const http::request<http::string_body>& req){
    http::response<http::string_body> response{http::status::bad_request, req.version()};
    response.set(http::field::content_type, ContentType::TYPE_JSON);
    response.set(http::field::cache_control, ContentType::NO_CACHE);

    json::object json_res;
    json_res["code"] = "invalidArgument";
    json_res["message"] = "Invalid name";
    response.body() = json::serialize(json_res);
    response.content_length(response.body().size());
    return response;
}

http::response<http::string_body> ParseError(const http::request<http::string_body>& req){
    http::response<http::string_body> response{http::status::bad_request, req.version()};
    response.set(http::field::content_type, ContentType::TYPE_JSON);
    response.set(http::field::cache_control, ContentType::NO_CACHE);

    json::object json_res;
    json_res["code"] = "invalidArgument";
    json_res["message"] = "Can't parse file, please try again :(";
    response.body() = json::serialize(json_res);
    response.content_length(response.body().size());
    return response;
}

http::response<http::string_body> BadRequest(const http::request<http::string_body>& req){
    http::response<http::string_body> response{http::status::bad_request, req.version()};
    response.set(http::field::content_type, ContentType::TYPE_JSON);
    response.body() = "{\"code\": \"badRequest\",\"message\": \"Bad request\"}";
    response.set(http::field::cache_control, ContentType::NO_CACHE);
    response.content_length(response.body().size());
    return response;
}

http::response<http::string_body> Application::ReturnMapsArray(const http::request<http::string_body>& req){

    http::response<http::string_body> response{http::status::ok, req.version()};

    response.set(http::field::content_type, ContentType::TYPE_JSON);

    json::array maps;
    for(const auto& it : game_.GetMaps()){
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

http::response<http::string_body> ReturnMap(const model::Map* const this_map, const http::request<http::string_body>& req){
    json::object map;
        map["id"] = *this_map->GetId();
        map["name"] = this_map->GetName();

    map["roads"] = ReturnRoads(this_map);
    map["buildings"] = ReturnBuildings(this_map);   
    map["offices"] = ReturnOffices(this_map);

    http::response<http::string_body> response{http::status::ok, req.version()};
    response.set(http::field::content_type, ContentType::TYPE_JSON);
    response.body() = json::serialize(map);
    response.set(http::field::cache_control, ContentType::NO_CACHE);
    response.content_length(response.body().size());
    return response;
}

json::array ReturnRoads(const model::Map* const this_map){
    json::array roads;
    for(const auto &it : this_map-> GetRoads()){
        json::object road;
        model::Point start_point = it.GetStart();
        road["x0"] = start_point.x;
        road["y0"] = start_point.y; 
       
        model::Point end_point = it.GetEnd();
        if(it.IsHorizontal()){
            road["x1"] = end_point.x;
        }else{
            road["y1"] = end_point.y;
        }
        roads.push_back(road);
    }
    return roads;
}

json::array ReturnBuildings(const model::Map* const this_map){
    json::array buildings;
    for(const auto &it : this_map-> GetBuildings()){
        json::object building;
        model::Rectangle bounds= it.GetBounds();
        building["x"] = bounds.position.x;
        building["y"] = bounds.position.y;
        building["w"] = bounds.size.width;
        building["h"] = bounds.size.height;
        buildings.push_back(building);
    }
    return buildings;
}

json::array ReturnOffices(const model::Map* const this_map){
    json::array offices;
    for(const auto &it : this_map-> GetOffices()){
        json::object office;
        office["id"] = *it.GetId();

        model::Point position = it.GetPosition();
        office["x"] = position.x;
        office["y"] = position.y;

        model::Offset offset = it.GetOffset();
        office["offsetX"] = offset.dx;
        office["offsetY"] = offset.dy;
        offices.push_back(office);
    }
    return offices;
}

const model::Game& Application::GetGame() const{
    return game_;
}

} //namespace