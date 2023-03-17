#pragma once 

#include <string>

namespace http_handler {
using namespace std::literals;

struct ContentType {
    ContentType() = delete;
    constexpr static std::string_view TYPE_TEXT_HTML = "text/html"sv;
    constexpr static std::string_view TYPE_JSON = "application/json"sv;
    constexpr static std::string_view NO_CACHE = "no-cache"sv;
};

struct Allow {
    Allow() = delete;
    constexpr static std::string_view GET = "GET"sv;
    constexpr static std::string_view HEAD = "HEAD"sv;
    constexpr static std::string_view POST = "POST"sv;
    constexpr static std::string_view GETANDHEAD = "GET, HEAD"sv;
};
    //Response handler answers
    const std::string BAD_REQUEST_CODE = "badRequest"s;
    const std::string BAD_REQUEST_MESSAGE = "Bad request"s;
    const std::string INVALID_ENDPOINT_MESSAGE = "Bad endpoint"s;

    const std::string MAP_NOT_FOUND_CODE = "mapNotFound"s;
    const std::string MAP_NOT_FOUND_MESSAGE = "Resource not found"s;

    const std::string INVALID_METHOD_CODE = "invalidMethod"s;
    const std::string GET_ALLOW_MESSAGE = "Only GET method allow"s;
    const std::string GET_AND_HEAD_ALLOW_MESSAGE = "Only GET and HEAD methods allow"s;
    const std::string POST_ALLOW_MESSAGE = "Only POST method allow"s;

    const std::string INVALID_TOKEN_CODE = "invalidToken"s;
    const std::string INVALID_FORMAT_MESSAGE = "Invlaid JSON format"s;
    const std::string INVALID_NAME_MESSAGE = "Invalid dog name"s;
    const std::string INVALID_TOKEN_MESSAGE =
            "Authorization header is missing or token is invalid"s;

    const std::string UNCNOWN_TOKEN_CODE = "unknownToken"s;
    const std::string UNCNOWN_TOKEN_MESSAGE = "Player token has not been found"s;
    
    //Basic response bodies
    const std::string CODE = "code"s;
    const std::string MESSAGE = "message"s;
    const std::string EMPTY_BODY = "{}"s;
    //Parce JSON request
    const std::string TIMEDELTA = "timeDelta"s;
    const std::string MOVE = "move"s;
    const std::string USERNAME = "userName"s;
    const std::string MAPID = "mapId"s;
    //JSON answers
    const std::string NAME = "name"s;
    const std::string ID = "id"s;
    const std::string POSITION = "pos"s;
    const std::string TYPE = "type"s;
    const std::string SPEED = "speed"s;
    const std::string DIRECTION = "dir"s;
    const std::string PLAYERS = "players"s;
    const std::string AUTHTOKEN = "authToken"s;
    const std::string PLAYERID = "playerId"s;
    const std::string LOSTOBJECTS = "lostObjects"s;
    const std::string BAG = "bag"s;
    const std::string SCORE = "score"s;
    //Map literals
    const std::string ROADS = "roads"s;
    const std::string BUILDINGS = "buildings"s;
    const std::string OFFICES = "offices"s;
    const std::string LOOTTYPES = "lootTypes"s;
    const std::string X = "x"s;
    const std::string X0 = "x0"s;
    const std::string X1 = "x1"s;
    const std::string Y = "y"s;
    const std::string Y0 = "y0"s;
    const std::string Y1 = "y1"s;
    const std::string WIDHT = "w"s;
    const std::string HEIGHT = "h"s;
    const std::string OFFSETX = "offsetX"s;
    const std::string OFFSETY = "offsetY"s;
}  // namespace http_handler

namespace boost_log {
    using namespace std::string_literals;

    const std::string PORT = "port"s;
    const std::string ADDRESS = "address"s;
    const std::string IP = "ip"s;
    const std::string URL = "URI"s;
    const std::string METHOD = "method"s;
    const std::string MESSAGE = "message"s;
    const std::string TIMESTAMP = "method"s;
    const std::string DATA = "data"s;
    const std::string CODE = "code"s;
    const std::string CONTENT_TYPE = "content_type"s;
    const std::string RESPONSE_TIME = "response_time"s;
    const std::string EXCEPTION = "text"s;
    const std::string WHERE = "where"s;
}  //namespace boost_log

namespace json_loader {
    using namespace std::string_literals;

    const std::string MAPS = "maps"s;
    const std::string VALUE = "value"s;
    const std::string LOOTCONFIG = "lootGeneratorConfig"s;
    const std::string PERIOD = "period"s;
    const std::string DEFAULTBAGCAPACITY = "defaultBagCapacity"s;
    const std::string BAGCAPACITY = "bagCapacity"s;
    const std::string PROBABILITY = "probability"s;
    const std::string DEFAULTDOGSPEED = "defaultDogSpeed"s;
    const std::string DOGSPEED = "dogSpeed"s;
}  //namespace boost_log
