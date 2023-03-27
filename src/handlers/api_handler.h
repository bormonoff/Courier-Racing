#pragma once

#include "handlers/application.h"

namespace http_handler{

using Strand = net::strand<net::io_context::executor_type>;
using Response = http::response<http::string_body>;
using Request = http::request<http::string_body>;

class ApiHandler{
public:
    explicit ApiHandler(model::Game& game, Strand& strand, size_t period, 
                        bool random_spawn, size_t save_interval, 
                        json_loot::LootTypes&& loot_types, 
                        const std::filesystem::path& path_to_state)
        : scenarios_{game, strand, period, random_spawn, save_interval, 
                     std::move(loot_types), path_to_state} {
    }

    ApiHandler(const ApiHandler&) = delete;
    ApiHandler& operator=(const ApiHandler&) = delete;
    
    Response MakeResponse(const Request& req,
                          const std::vector<std::string>& URL_path);
    void SaveState();

private:
    Application scenarios_;
};
}  //namespace http_handler