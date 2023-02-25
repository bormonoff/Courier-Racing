#include "api_handler.h"

#include <string>

#include "util/tagged.h"

namespace http_handler{

Response ApiHandler::MakeResponse(const Request& req,
                                  const std::vector<std::string>& parseURL) {  
    if (parseURL[0] == "api") {
        if (parseURL.size() > 2 && parseURL[1] == "v1") {
            if (parseURL.size() == 3 && parseURL[2] == "maps") {
                return scenarios_.ReturnMapsArray(req);
            }
            if (parseURL.size() == 4 && parseURL[2] == "game" && 
                parseURL[3] == "join") {
                return scenarios_.JoinGame(req);
            }
            if (parseURL.size() == 4 && parseURL[2] == "game" && 
                parseURL[3] == "players") {
                return scenarios_.GetPlayers(req);
            }
            if (parseURL.size() == 4 && parseURL[2] == "game" && 
                parseURL[3] == "state") {
                return scenarios_.GetState(req);
            }
            if (parseURL.size() == 4 && parseURL[2] == "game" && 
                parseURL[3] == "tick") {
                return scenarios_.MoveDogs(req);
            }
            if (parseURL.size() == 4 && parseURL[2] == "maps") {
                const auto this_map = scenarios_.GetGame().FindMap(
                    util::Tagged<std::string, model::Map>(parseURL[3]));
                if (this_map != nullptr) {
                    return ReturnMap(this_map, req);
                }else{
                    return NotFound(req);
                }
            }
            if (parseURL.size() == 5 && parseURL[2] == "game" && 
                parseURL[3] == "player" && parseURL[4] == "action") {
                return scenarios_.ChangeSpeed(req);
            }
        }
    }

    return BadRequest(req);
}
}  //namespace http_handler