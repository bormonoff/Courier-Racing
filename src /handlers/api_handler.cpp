#include "api_handler.h"
#include "../util/tagged.h"
#include <string>

namespace http_handler{

http::response<http::string_body> ApiHandler::MakeResponse (const http::request<http::string_body> &req
                                                               , const std::vector<std::string>& parseURL){  
    if(parseURL[0] == "api"){
        if(parseURL.size() > 2 && parseURL[1] == "v1"){
            if(parseURL.size() == 3 && parseURL[2] == "maps"){
                return ReturnMapsArray(game_, req);
            }
            if(parseURL.size() == 4 && parseURL[2] == "game" && parseURL[3] == "join"){
                return JoinGame(req);
            }
            if(parseURL.size() == 4 && parseURL[2] == "game" && parseURL[3] == "players"){
                return GetPlayers(req);
            }
            if(parseURL.size() == 4 && parseURL[2] == "maps"){
                const auto this_map = game_.FindMap(util::Tagged<std::string, model::Map>(parseURL[3]));
                if(this_map != nullptr){
                    return ReturnMap(this_map, req);
                }else{
                    return NotFound(req);
                }
            }
        }
    }

    return BadRequest(game_, req);
}

http::response<http::string_body> ApiHandler::GetPlayers(const http::request<http::string_body>& req){
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

    return FindPlayerViaToken(req, token);
}

http::response<http::string_body> ApiHandler::FindPlayerViaToken(const http::request<http::string_body>& req, std::string& token){
    for(auto it : sessions_){
        std::optional<game_session::Player> player = (it.second).FindPlayer(token);
        if(player != std::nullopt){
            return FindAllPlayersOnMap(req, it.second);
        }
    }
    return CantFindPlayer(req);
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

http::response<http::string_body> CantAuthorize(const http::request<http::string_body> &req){
    http::response<http::string_body> response{http::status::unauthorized, req.version()};
    response.set(http::field::content_type, ContentType::TYPE_JSON);
    response.set(http::field::cache_control, ContentType::NO_CACHE);
    
    json::object json_res;
    json_res["code"] = "InvalidToken";
    json_res["message"] = "Authorization header is miising or token is invalid";
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

http::response<http::string_body> MethodNotAllowed(const http::request<http::string_body>& req){
    http::response<http::string_body> response{http::status::method_not_allowed, req.version()};
    response.set(http::field::content_type, ContentType::TYPE_JSON);
    response.set(http::field::cache_control, ContentType::NO_CACHE);
    response.set(http::field::allow, "GET");
    response.set(http::field::allow, "HEAD");


    json::object json_res;
    json_res["code"] = "invalidMethod";
    json_res["message"] = "Invalid Method";
    response.body() = json::serialize(json_res);
    response.content_length(response.body().size());
    return response;
}

http::response<http::string_body> ApiHandler::JoinGame(const http::request<http::string_body>& req){
    if(req.method_string() != "POST"){
        return NotAllowed(req);
    }

    boost::system::error_code error;
    json::value parse_req = json::parse(req.body(), error);
    if(error){
        return ParseError(req); 
    }

    const std::string dog_name = static_cast<std::string>(parse_req.at("userName").as_string());
    const std::string map = static_cast<std::string>(parse_req.at("mapId").as_string());
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

http::response<http::string_body> ApiHandler::Authorization(const http::request<http::string_body> &req, 
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
    http::response<http::string_body> response{http::status::not_found, req.version()};
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
    http::response<http::string_body> response{http::status::not_found, req.version()};
    response.set(http::field::content_type, ContentType::TYPE_JSON);
    response.set(http::field::cache_control, ContentType::NO_CACHE);

    json::object json_res;
    json_res["code"] = "invalidArgument";
    json_res["message"] = "Can't parse file, please try again :(";
    response.body() = json::serialize(json_res);
    response.content_length(response.body().size());
    return response;
}

http::response<http::string_body> NotAllowed(const http::request<http::string_body>& req){
    http::response<http::string_body> response{http::status::method_not_allowed, req.version()};
    response.set(http::field::content_type, ContentType::TYPE_JSON);
    response.set(http::field::cache_control, ContentType::NO_CACHE);
    response.set(http::field::allow, "POST");

    json::object json_res;
    json_res["code"] = "invalidArgument";
    json_res["message"] = "Only POST method is expected :(";
    response.body() = json::serialize(json_res);
    response.content_length(response.body().size());
    return response;
}

http::response<http::string_body> BadRequest(model::Game& game_, const http::request<http::string_body>& req){
    http::response<http::string_body> response{http::status::bad_request, req.version()};
    response.set(http::field::content_type, ContentType::TYPE_JSON);
    response.body() = "{\"code\": \"badRequest\",\"message\": \"Bad request\"}";
    return response;
}

http::response<http::string_body> ReturnMapsArray(model::Game& game_, const http::request<http::string_body>& req){

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

} //namespace