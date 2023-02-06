#pragma once

#include "application.h"

namespace http_handler{

class ApiHandler{
public:
    ApiHandler(model::Game& game)
        : scenarios_{game}{}
    
    http::response<http::string_body> MakeResponse(const http::request<http::string_body> &req,
                                                   const std::vector<std::string>& URL_path);

private:
    Application scenarios_;
};


} //namespace