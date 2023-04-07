#include "handlers/api_response_storage.h"
#include "util/literals_storage.h"

namespace http_handler{

Response NotFound(const Request& req) {
    Response response{http::status::not_found, req.version()};
    response.set(http::field::content_type, ContentType::TYPE_JSON);
    response.set(http::field::cache_control, ContentType::NO_CACHE);

    json::object json_res;
    json_res[CODE] = MAP_NOT_FOUND_CODE;
    json_res[MESSAGE] = MAP_NOT_FOUND_MESSAGE;
    response.body() = json::serialize(json_res);
    response.content_length(response.body().size());
    return response;
}

Response OkResponse(const Request& req) {
    Response response{http::status::ok, req.version()};
    response.set(http::field::content_type, ContentType::TYPE_JSON);
    response.set(http::field::cache_control, ContentType::NO_CACHE);
    
    response.body() = EMPTY_BODY;
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
    FormPlayersJSON(session, ID);
    json_res[PLAYERS] = ID;

    json::object lost_objects;
    FormItemsJSON(session, lost_objects);
    json_res[LOSTOBJECTS] = lost_objects;

    response.body() = json::serialize(json_res);
    response.content_length(response.body().size());

    // std::cout << json_res << "\n";
    return response;
}

void FormPlayersJSON(const game_session::GameSession& session, json::object& obj) {
    for (auto& it : session.GetPlayerTokens().GetPlayers()) {
        json::object data;
        json::array position;
        position.push_back(it.second.GetDogStart().x);
        position.push_back(it.second.GetDogStart().y);
        data[POSITION] = position;

        json::array speed;
        speed.push_back(it.second.GetDogSpeed().dx);
        speed.push_back(it.second.GetDogSpeed().dy);
        data[SPEED] = speed;

        data[DIRECTION] = it.second.GetDogDirection();

        json::array bag;
        for (auto& items : it.second.GetDog().GetItemsInBag()) {
            json::object bag_data;
            bag_data[ID] = items.ID;
            bag_data[TYPE] = items.type;
            bag.push_back(bag_data);
        }
        data[BAG] = bag;
        
        data[SCORE] = it.second.GetDogScore();

        obj[std::to_string(it.second.GetDogID())] = data;
    }
}

void FormItemsJSON(const game_session::GameSession& session, json::object& lost_objects) {
    for (auto& it : session.GetMap().GetLostThings()) {
        json::object data;
        data[TYPE] = it.second.type;

        json::array position;
        position.push_back(it.second.x);
        position.push_back(it.second.y);
        data[POSITION] = position;

        lost_objects[std::to_string(it.first)] = data;
    }
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
        temp_respond[NAME] = (it.second).GetDogName();
        json_res[std::to_string(it.second.GetDogID())] = temp_respond;
    }

    response.body() = json::serialize(json_res);
    response.content_length(response.body().size());

    // std::cout << json_res << "\n";
    return response;
}

Response MethodNotAllowed(const Request& req) {
    Response response{http::status::method_not_allowed, req.version()};
    response.set(http::field::content_type, ContentType::TYPE_JSON);
    response.set(http::field::cache_control, ContentType::NO_CACHE);
    response.set(http::field::allow, Allow::GETANDHEAD);

    json::object json_res;
    json_res[CODE] = INVALID_METHOD_CODE;
    json_res[MESSAGE] = GET_AND_HEAD_ALLOW_MESSAGE;
    response.body() = json::serialize(json_res);
    response.content_length(response.body().size());
    return response;
}

Response CantAuthorize(const Request& req) {
    Response response{http::status::unauthorized, req.version()};
    response.set(http::field::content_type, ContentType::TYPE_JSON);
    response.set(http::field::cache_control, ContentType::NO_CACHE);
    
    json::object json_res;
    json_res[CODE] = INVALID_TOKEN_CODE;
    json_res[MESSAGE] = INVALID_TOKEN_MESSAGE;
    response.body() = json::serialize(json_res);
    response.content_length(response.body().size());
    return response;
}

Response CantFindPlayer(const Request& req) {
    Response response{http::status::unauthorized, req.version()};
    response.set(http::field::content_type, ContentType::TYPE_JSON);
    response.set(http::field::cache_control, ContentType::NO_CACHE);
    
    json::object json_res;
    json_res[CODE] = UNCNOWN_TOKEN_CODE;
    json_res[MESSAGE] = UNCNOWN_TOKEN_MESSAGE;
    response.body() = json::serialize(json_res);
    response.content_length(response.body().size());
    return response;
}

Response NotAllowed(const Request& req) {
    Response response{http::status::method_not_allowed, req.version()};
    response.set(http::field::content_type, ContentType::TYPE_JSON);
    response.set(http::field::cache_control, ContentType::NO_CACHE);
    response.set(http::field::allow, Allow::POST);

    json::object json_res;
    json_res[CODE] = INVALID_METHOD_CODE;
    json_res[MESSAGE] = POST_ALLOW_MESSAGE;
    response.body() = json::serialize(json_res);
    response.content_length(response.body().size());
    return response;
}

Response InvalidPlayerName(const Request& req) {
    Response response{http::status::bad_request, req.version()};
    response.set(http::field::content_type, ContentType::TYPE_JSON);
    response.set(http::field::cache_control, ContentType::NO_CACHE);

    json::object json_res;
    json_res[CODE] = INVALID_TOKEN_CODE;
    json_res[MESSAGE] = INVALID_NAME_MESSAGE;
    response.body() = json::serialize(json_res);
    response.content_length(response.body().size());
    return response;
}

Response ParseError(const Request& req) {
    Response response{http::status::bad_request, req.version()};
    response.set(http::field::content_type, ContentType::TYPE_JSON);
    response.set(http::field::cache_control, ContentType::NO_CACHE);

    json::object json_res;
    json_res[CODE] = INVALID_TOKEN_CODE;
    json_res[MESSAGE] = INVALID_FORMAT_MESSAGE;
    response.body() = json::serialize(json_res);
    response.content_length(response.body().size());
    return response;
}

Response BadRequest(const Request& req) {
    Response response{http::status::bad_request, req.version()};
    response.set(http::field::content_type, ContentType::TYPE_JSON);
    response.set(http::field::cache_control, ContentType::NO_CACHE);

    json::object json_res;
    json_res[CODE] = BAD_REQUEST_CODE;
    json_res[MESSAGE] = BAD_REQUEST_MESSAGE;
    response.body() = json::serialize(json_res);
    response.content_length(response.body().size());
    return response;
}

Response TickFail(const Request& req) {
    Response response{http::status::bad_request, req.version()};
    response.set(http::field::content_type, ContentType::TYPE_JSON);
    response.set(http::field::cache_control, ContentType::NO_CACHE);

    json::object json_res;
    json_res[CODE] = BAD_REQUEST_CODE;
    json_res[MESSAGE] = INVALID_ENDPOINT_MESSAGE;
    response.body() = json::serialize(json_res);
    response.content_length(response.body().size());
    return response;
}

json::array ReturnRoads(const model::Map* const this_map) {
    json::array roads;
    for (const auto &it : this_map -> GetRoads()) {
        json::object road;
        model::Point start_point = it.GetStart();
        road[X0] = start_point.x;
        road[Y0] = start_point.y; 
       
        model::Point end_point = it.GetEnd();
        if (it.IsHorizontal()) {
            road[X1] = end_point.x;
        } else {
            road[Y1] = end_point.y;
        }
        roads.push_back(road);
    }
    return roads;
}

json::array ReturnBuildings(const model::Map* const this_map) {
    json::array buildings;
    for(const auto &it : this_map -> GetBuildings()){
        json::object building;
        model::Rectangle bounds = it.GetBounds();
        building[X] = bounds.position.x;
        building[Y] = bounds.position.y;
        building[WIDHT] = bounds.size.width;
        building[HEIGHT] = bounds.size.height;
        buildings.push_back(building);
    }
    return buildings;
}

json::array ReturnOffices(const model::Map* const this_map) {
    json::array offices;
    for (const auto &it : this_map -> GetOffices()) {
        json::object office;
        office[ID] = *it.GetId();

        model::Point position = it.GetPosition();
        office[X] = position.x;
        office[Y] = position.y;

        model::Offset offset = it.GetOffset();
        office[OFFSETX] = offset.dx;
        office[OFFSETY] = offset.dy;
        offices.push_back(office);
    }
    return offices;
}
}  // namespace http_handler