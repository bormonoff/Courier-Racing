#pragma once

#include "logging_request_handler.h"
#include "api_handler.h"
#include <string_view>
#include <filesystem>
#include <optional>
#include <cctype>
#include "sstream"
#include <mutex>

namespace http_handler {

std::vector<std::string> ReadURL(const http::request<http::string_body>& req);
http::response<http::string_body> FileNotFound(const http::request<http::string_body>& req);

static std::mutex mutex_;

class RequestHandler{
public:
    explicit RequestHandler(model::Game& game, const std::filesystem::path& path_to_content)
        : api_handler_{game}
        , path_to_content_{path_to_content}{
    }

    RequestHandler& operator=(const RequestHandler&) = delete;

    template <typename Body, typename Allocator, typename Send>
    void operator()(http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send) {
        std::string_view http_request = req.target();
        std::vector<std::string> parseURL{std::move(ReadURL(req))};

        if(parseURL[0] == "api"){
            mutex_.lock();
            send(std::move(api_handler_.MakeResponse(req, parseURL)));
            mutex_.unlock();
        }else{
            std::optional<http::response<http::file_body>> response = MakeFileResponse(req, parseURL);
            if(response){
                send(std::move(*response));
            }else{
                send(std::move(FileNotFound(req)));
            }   
        }
    }

std::optional<http::response<http::file_body>> MakeFileResponse(const http::request<http::string_body> &req
                                                 , const std::vector<std::string>& URL_path);                                             

private:
    ApiHandler api_handler_;
    const std::filesystem::path path_to_content_;
};

std::filesystem::path MakePath (const std::vector<std::string>& path,std::filesystem::path path_to_content_);
std::string DecodeURL(const std::string& target_url);
bool IsInRoot(const std::filesystem::path& root, const std::filesystem::path& resource);
std::string MimeType(std::string& expand);

void DownScale(std::string& string);

}  // namespace http_handler
