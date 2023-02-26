#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include "boost/json.hpp"

#include "session/game_session.h"

namespace http_handler{

namespace http = boost::beast::http;
namespace json = boost::json;

using Response = http::response<http::string_body>;
using Request = http::request<http::string_body>;

Response FindAllPlayerStatesOnMap(const Request& req, 
                                  const game_session::GameSession& session);
Response FindAllPlayersOnMap(const Request& req,
                             const game_session::GameSession& session);
Response NotFound(const Request& req);
Response OkResponse(const Request& req);
Response MethodNotAllowed(const Request& req);
Response CantAuthorize(const Request& req);
Response CantFindPlayer(const Request& req);
Response NotAllowed(const Request& req);
Response InvalidPlayerName(const Request& req);
Response ParseError(const Request& req);
Response ReturnMap(const model::Map* const this_map, const Request& req);
Response BadRequest(const Request& req);
Response MethodGETAllowed(const Request& req);
Response TickFail(const Request& req);
json::array ReturnRoads(const model::Map* const this_map);
json::array ReturnBuildings(const model::Map* const this_map);
json::array ReturnOffices(const model::Map* const this_map);

}  // namespace http_handler