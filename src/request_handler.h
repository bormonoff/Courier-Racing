#pragma once

#include "http_server.h"
#include "model.h"
#include "sstream"
#include <boost/json.hpp>
#include <string_view>
#include <filesystem>
#include <optional>
#include <cctype>

namespace http_handler {
    
namespace beast = boost::beast;
namespace http = beast::http;
namespace json = boost::json;
using namespace std::literals;

struct ContentType {
    ContentType() = delete;
    constexpr static std::string_view TYPE_TEXT_HTML = "text/html"sv;
    constexpr static std::string_view TYPE_JSON = "application/json"sv;
};

std::vector<std::string> ReadURL(const http::request<http::string_body>& req);
http::response<http::string_body> NotFound(model::Game& game_, const http::request<http::string_body>& req);
http::response<http::string_body> FileNotFound(model::Game& game_, const http::request<http::string_body>& req);

class RequestHandler {
public:
    explicit RequestHandler(model::Game& game, const std::filesystem::path& path_to_content)
        : game_{game}
        , path_to_content_{path_to_content}{
    }

    RequestHandler(const RequestHandler&)   = delete;
    RequestHandler& operator=(const RequestHandler&) = delete;

    template <typename Body, typename Allocator, typename Send>
    void operator()(http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send) {
        std::string_view http_request = req.target();
        std::vector<std::string> parseURL{std::move(ReadURL(req))};

        if(parseURL[0] == "api"){
            send(std::move(MakeResponse(req, parseURL)));
        }else{
            std::optional<http::response<http::file_body>> response = MakeFileResponse(req, parseURL);
            if(response){
                send(std::move(*response));
            }else{
                send(std::move(FileNotFound(game_, req)));
            }
        }
    }

http::response<http::string_body> MakeResponse(const http::request<http::string_body> &req
                                               , const std::vector<std::string>& URL_path);
std::optional<http::response<http::file_body>> MakeFileResponse(const http::request<http::string_body> &req
                                                 , const std::vector<std::string>& URL_path);                                             

private:
    model::Game& game_;
    const std::filesystem::path path_to_content_;
};

std::filesystem::path MakePath (const std::vector<std::string>& path,std::filesystem::path path_to_content_);
std::string DecodeURL(const std::string& target_url);
bool IsInRoot(const std::filesystem::path& root, const std::filesystem::path& resource);
std::string MimeType(std::string& expand);


http::response<http::string_body> ReturnMapsArray(model::Game& game_, const http::request<http::string_body>& req);
http::response<http::string_body> ReturnMap(const model::Map* const this_map, const http::request<http::string_body>& req);
http::response<http::string_body> BadRequest(model::Game& game_, const http::request<http::string_body>& req);

json::array ReturnRoads(const model::Map* const this_map);
json::array ReturnBuildings(const model::Map* const this_map);
json::array ReturnOffices(const model::Map* const this_map);

void DownScale(std::string& string);

}  // namespace http_handler
