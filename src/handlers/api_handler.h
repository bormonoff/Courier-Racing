#pragma once

#include "handlers/application.h"

namespace http_handler{

using Strand = net::strand<net::io_context::executor_type>;
using Response = http::response<http::string_body>;
using Request = http::request<http::string_body>;

class ApiHandler{
public:
    explicit ApiHandler(model::Game& game, Strand& strand, size_t period, bool random_spawn)
        : scenarios_{game, strand, period, random_spawn}{
    }

    ApiHandler(const ApiHandler&) = delete;
    ApiHandler& operator=(const ApiHandler&) = delete;
    
    Response MakeResponse(const Request& req,
                          const std::vector<std::string>& URL_path);
private:
    Application scenarios_;
};
}  //namespace http_handler