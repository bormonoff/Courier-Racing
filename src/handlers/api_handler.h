#pragma once

#include "application.h"

namespace http_handler{

using Strand = net::strand<net::io_context::executor_type>;

class ApiHandler{
public:
    ApiHandler(model::Game& game, Strand& strand, size_t period, bool random_spawn)
        : scenarios_{game, strand, period, random_spawn}{
    }
    
    http::response<http::string_body> MakeResponse(const http::request<http::string_body> &req,
                                                   const std::vector<std::string>& URL_path);


private:
    Application scenarios_;
};


} //namespace