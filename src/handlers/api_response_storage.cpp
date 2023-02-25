#include "handlers/api_response_storage.h"

namespace http_handler{

Response NotFound(const Request& req) {
    Response response{http::status::not_found, req.version()};
    response.set(http::field::content_type, ContentType::TYPE_JSON);
    response.set(http::field::cache_control, ContentType::NO_CACHE);

    json::object json_res;
    json_res["code"] = "mapNotFound";
    json_res["message"] = "Resource not found";
    response.body() = json::serialize(json_res);
    response.content_length(response.body().size());
    return response;
}

Response OkResponse(const Request& req) {
    Response response{http::status::ok, req.version()};
    response.set(http::field::content_type, ContentType::TYPE_JSON);
    response.set(http::field::cache_control, ContentType::NO_CACHE);
    response.body() = "{}";
    response.content_length(response.body().size());
    return response;
}

Response FindAllPlayerStatesOnMap(const Request& req, 
                                  const game_session::GameSession& session) {
    Response response{http::status::ok, req.version()};
    response.set(http::field::content_type, ContentType::TYPE_JSON);
    response.set(http::field::cache_control, ContentType::NO_CACHE);
    
    json::object json_res;
    json::object ID;
    for (auto it : session.GetPlayerTokens().GetPlayers()) {
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
    response.body() = json::serialize(json_res);
    response.content_length(response.body().size());
    return response;
}

Response FindAllPlayersOnMap(const Request& req, 
                             const game_session::GameSession& session) {
    Response response{http::status::ok, req.version()};
    response.set(http::field::content_type, ContentType::TYPE_JSON);
    response.set(http::field::cache_control, ContentType::NO_CACHE);
    
    json::object json_res;
    size_t count = 0;
    for (auto it : session.GetPlayerTokens().GetPlayers()) {
        json::object temp_respond;
        temp_respond["name"] = (it.second).GetDogName();
        json_res[std::to_string(count)] = temp_respond;
        ++count;
    }
    response.body() = json::serialize(json_res);
    response.content_length(response.body().size());
    return response;
}

Response MethodNotAllowed(const Request& req) {
    Response response{http::status::bad_request, req.version()};
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

Response CantAuthorize(const Request& req) {
    Response response{http::status::unauthorized, req.version()};
    response.set(http::field::content_type, ContentType::TYPE_JSON);
    response.set(http::field::cache_control, ContentType::NO_CACHE);
    
    json::object json_res;
    json_res["code"] = "invalidToken";
    json_res["message"] = "Authorization header is miising or token is invalid";
    response.body() = json::serialize(json_res);
    response.content_length(response.body().size());
    return response;
}

Response CantFindPlayer(const Request& req) {
    Response response{http::status::unauthorized, req.version()};
    response.set(http::field::content_type, ContentType::TYPE_JSON);
    response.set(http::field::cache_control, ContentType::NO_CACHE);
    
    json::object json_res;
    json_res["code"] = "unknownToken";
    json_res["message"] = "Player token has not been found";
    response.body() = json::serialize(json_res);
    response.content_length(response.body().size());
    return response;
}

Response NotAllowed(const Request& req) {
    Response response{http::status::method_not_allowed, req.version()};
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

Response InvalidPlayerName(const Request& req) {
    Response response{http::status::bad_request, req.version()};
    response.set(http::field::content_type, ContentType::TYPE_JSON);
    response.set(http::field::cache_control, ContentType::NO_CACHE);

    json::object json_res;
    json_res["code"] = "invalidArgument";
    json_res["message"] = "Invalid name";
    response.body() = json::serialize(json_res);
    response.content_length(response.body().size());
    return response;
}

Response ParseError(const Request& req) {
    Response response{http::status::bad_request, req.version()};
    response.set(http::field::content_type, ContentType::TYPE_JSON);
    response.set(http::field::cache_control, ContentType::NO_CACHE);

    json::object json_res;
    json_res["code"] = "invalidArgument";
    json_res["message"] = "Can't parse file, please try again :(";
    response.body() = json::serialize(json_res);
    response.content_length(response.body().size());
    return response;
}

Response BadRequest(const Request& req) {
    Response response{http::status::bad_request, req.version()};
    response.set(http::field::content_type, ContentType::TYPE_JSON);
    response.set(http::field::cache_control, ContentType::NO_CACHE);

    json::object json_res;
    json_res["code"] = "badRequest";
    json_res["message"] = "Bad request";
    response.body() = json::serialize(json_res);
    response.content_length(response.body().size());
    return response;
}

Response MethodGETAllowed(const Request& req) {
    Response response{http::status::bad_request, req.version()};
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

Response TickFail(const Request& req) {
    Response response{http::status::bad_request, req.version()};
    response.set(http::field::content_type, ContentType::TYPE_JSON);
    response.set(http::field::cache_control, ContentType::NO_CACHE);

    json::object json_res;
    json_res["code"] = "badRequest";
    json_res["message"] = "Invalid endpoint";
    response.body() = json::serialize(json_res);
    response.content_length(response.body().size());
    return response;
}

Response ReturnMap(const model::Map* const this_map, const Request& req) {
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

json::array ReturnRoads(const model::Map* const this_map) {
    json::array roads;
    for (const auto &it : this_map-> GetRoads()) {
        json::object road;
        model::Point start_point = it.GetStart();
        road["x0"] = start_point.x;
        road["y0"] = start_point.y; 
       
        model::Point end_point = it.GetEnd();
        if (it.IsHorizontal()) {
            road["x1"] = end_point.x;
        } else {
            road["y1"] = end_point.y;
        }
        roads.push_back(road);
    }
    return roads;
}

json::array ReturnBuildings(const model::Map* const this_map) {
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

json::array ReturnOffices(const model::Map* const this_map) {
    json::array offices;
    for (const auto &it : this_map-> GetOffices()) {
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
}  // namespace http_handler