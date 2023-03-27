 #include "handlers/request_handler.h"

namespace http_handler {

namespace fs = std::filesystem;
namespace sys = boost::system;

http::response<http::string_body> FileNotFound(
        const http::request<http::string_body>& req) {
    http::response<http::string_body> response{http::status::not_found, req.version()};
    response.set(http::field::content_type, "text/plain");
    response.body() = "404 Not found";
    return response;
}

std::vector<std::string> ReadURL(const http::request<http::string_body>& req) {
    std::vector<std::string> result;
    std::string RequestURL = DecodeURL(static_cast<std::string>(req.target()));
    std::string delim = "/";

    if (RequestURL.size() == 1) {
        result.push_back("index.html");
        return result;
    }
    auto start = 1;
    auto end = RequestURL.find(delim, start);
    if (end == std::string::npos) {
        result.push_back(RequestURL.substr(1));
        return result;
    }
    while (end != std::string::npos) {
        result.push_back(RequestURL.substr(start, end - start));
        start = end + delim.size();
        end = RequestURL.find(delim, start);
    }
    if (RequestURL.size() > start) {
            result.push_back(RequestURL.substr(start));
    }
    return result;
}

std::string DecodeURL(const std::string& target_url) {
    std::string decode_url;
    char decode_symbol;
    int encode_char;
    for (int i = 0; i < target_url.size(); ++i) {
        if (target_url[i] == '%') {
            sscanf(target_url.substr(i + 1,2).c_str(), "%x", &encode_char);
            decode_symbol = static_cast<char>(encode_char);
            decode_url += decode_symbol;
            i = i + 2;
        } else { 
            decode_url += target_url[i];
        }
    }
    return decode_url;
}

std::filesystem::path MakePath (const std::vector<std::string>& URL_path, 
                                std::filesystem::path path_to_content) {
    for (const auto& it : URL_path) { 
        path_to_content.append(it);
    }

    return path_to_content;
}

bool IsInRoot(const fs::path& root, const fs::path& resource) {
    for (auto a = root.begin(), b = resource.begin(); a != root.end(); ++a, ++b) {
        if (b == resource.end() || *a != *b) {
            return false;
        }
    }
    return true;
}

std::optional<http::response<http::file_body>> RequestHandler::MakeFileResponse(
        const http::request<http::string_body> &req,
        const std::vector<std::string>& URL_path) {
    std::filesystem::path path_to_content = MakePath(URL_path, path_to_content_);
    std::string string_for_open = path_to_content;
    const char* file_path = string_for_open.data();

    http::response<http::file_body> response{http::status::ok, req.version()};

    std::string ext = path_to_content.extension();
    response.set(http::field::content_type, MimeType(ext));

    http::file_body::value_type file;
    if (sys::error_code error; file.open(file_path, beast::file_mode::read, error),
                                         error) {
       return std::nullopt;
    }
    
    response.body() = std::move(file);
    response.prepare_payload();

    return response;
}

std::string MimeType(std::string& expand) {
    if(expand == ""){return "application/octet-stream";}
    DownScale(expand);
    if(expand==".htm" || expand == ".html"){return "text/html";}
    if(expand==".css"){return "text/css";}
    if(expand==".txt"){return "text/plain";}
    if(expand==".js"){return "text/javascript";}
    if(expand==".json"){return "application/json";}
    if(expand==".xml"){return "application/xml";}
    if(expand==".png"){return "image/png";}
    if(expand==".jpg" || expand==".jpe" || expand==".jpeg"){return "image/jpeg";}
    if(expand==".gif"){return "image/gif";}
    if(expand==".bmp"){return "image/bmp";}
    if(expand==".ico"){return "image/vnd.microsoft.icon";}
    if(expand==".tiff" || expand == ".tif"){return "image/tiff";}
    if(expand==".svg" || expand == ".svgz"){return "image/svg+xml";}
    if(expand==".mp3"){return "audio/mpeg";}
    return "application/octet-stream";
}

void DownScale(std::string& string) {
    for (int i = 0; i < string.size()-1; ++i) {
        if(string[i] > 'A' && string[i] < 'Z') {
            string[i] += 'z'-'Z';
        }
    }
}

void RequestHandler::SaveState() {
    api_handler_.SaveState();
}

}  // namespace http_handler


   