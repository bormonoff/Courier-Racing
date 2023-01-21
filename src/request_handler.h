#pragma once

#include "http_server.h"
#include "model.h"
#include "sstream"
#include <boost/json.hpp>

namespace http_handler {
    
namespace beast = boost::beast;
namespace http = beast::http;
namespace json = boost::json;
using namespace std::literals;

http::response<http::string_body> ReturnMapsArray(model::Game& game_, const http::request<http::string_body>& req);
http::response<http::string_body> ReturnMap(const model::Map* const this_map, const http::request<http::string_body>& req);
json::array ReturnRoads(const model::Map* const this_map);
json::array ReturnBuildings(const model::Map* const this_map);
json::array ReturnOffices(const model::Map* const this_map);

struct ContentType {
    ContentType() = delete;
    constexpr static std::string_view TYPE_TEXT_HTML = "text/html"sv;
    constexpr static std::string_view TYPE_JSON = "application/json"sv;
};

class RequestHandler {
public:
    explicit RequestHandler(model::Game& game)
        : game_{game} {
    }

    RequestHandler(const RequestHandler&)   = delete;
    RequestHandler& operator=(const RequestHandler&) = delete;

    template <typename Body, typename Allocator, typename Send>
    void operator()(http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send) {
        
        send(std::move(MakeResponse(req)));
    }
    
    http::response<http::string_body> MakeResponse(const http::request<http::string_body>& req);

private:
    model::Game& game_;
};

}  // namespace http_handler
