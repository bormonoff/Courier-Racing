#include "api_handler.h"

#include <string>

#include "util/tagged.h"

namespace http_handler{

ApiHandler::ApiHandler(model::Game& game, Strand& strand, size_t period, 
                    bool random_spawn, size_t save_interval, 
                    json_loot::LootTypes&& loot_types, 
                    const std::filesystem::path& path_to_state, 
                    postgres::DataBase& data_base)
    : scenarios_{game, strand, period, random_spawn, save_interval, 
                    std::move(loot_types), path_to_state, data_base} {
                        
    handlers_["/api/v1/maps"] = {[this](const Request& req) {
            return scenarios_.ReturnMapsArray(req);
        }};
    handlers_["/api/v1/game/join"] = {[this](const Request& req) {
            return scenarios_.JoinGame(req);
        }};
    handlers_["/api/v1/game/records"] = {[this](const Request& req) {
            return scenarios_.GetRecords(req);
        }};
    handlers_["/api/v1/game/players"] = {[this](const Request& req) {
            return scenarios_.GetPlayers(req);
        }};
    handlers_["/api/v1/game/state"] = {[this](const Request& req) {
            return scenarios_.GetState(req);
        }};
    handlers_["/api/v1/game/tick"] = {[this](const Request& req) {
            return scenarios_.MoveDogs(req);
        }};
    handlers_["/api/v1/game/records"] = {[this](const Request& req) { 
            return scenarios_.GetRecords(req);
        }};
    handlers_["/api/v1/game/player/action"] = {[this](const Request& req) { 
            return scenarios_.ChangeSpeed(req);
        }};

    for (auto& it : game.GetMaps()) {
        std::string name = *it.GetId();
        std::string result = "/api/v1/maps/"s + name;

        handlers_[result] = {[this, name](const Request& req) { 
            const auto this_map = scenarios_.GetGame().FindMap(
                util::Tagged<std::string, model::Map>(name));
            if (this_map != nullptr) {
                return scenarios_.ReturnMap(this_map, req);
            }else{
                return NotFound(req);
            }
        }};
    }
}

Response ApiHandler::MakeResponse(const Request& req,
                                  const std::string& parseURL) { 
    auto it = handlers_.find(parseURL);
    if (it == handlers_.end()) { return BadRequest(req); }
    return it->second(req);
}

void ApiHandler::SaveState() {
    scenarios_.SaveTofile(0, true);
}
}  //namespace http_handler