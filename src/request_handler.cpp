 #include "request_handler.h"

namespace http_handler {

namespace fs = std::filesystem;
namespace sys = boost::system;

std::vector<std::string> ReadURL(const http::request<http::string_body>& req){
    std::vector<std::string> result;
    std::string RequestURL = DecodeURL(static_cast<std::string>(req.target()));
    std::string delim = "/";

    if(RequestURL.size() == 1){
        result.push_back("index.html");
        return result;
    }

    auto start = 1;
    auto end = RequestURL.find(delim, start);
    if(end == std::string::npos){
        result.push_back(RequestURL.substr(1));
        return result;
    }

    while(end != std::string::npos){
        result.push_back(RequestURL.substr(start, end - start));
        start = end + delim.size();
        end = RequestURL.find(delim, start);
    }
    
    if(RequestURL.size() > start){
            result.push_back(RequestURL.substr(start));
    }

    return result;
}

std::string DecodeURL(const std::string& target_url){
    std::string decode_url;
    char decode_symbol;
    int encode_char;
    for(int i = 0; i < target_url.size(); i++){
        if(target_url[i] == '%'){
            sscanf(target_url.substr(i + 1,2).c_str(), "%x", &encode_char);
            decode_symbol = static_cast<char>(encode_char);
            decode_url += decode_symbol;
            i = i + 2;
        }else{ 
            decode_url += target_url[i];
        }
    }
    return decode_url;
}

std::filesystem::path MakePath (const std::vector<std::string>& URL_path, std::filesystem::path path_to_content){
    for(const auto& it : URL_path){
        path_to_content.append(it);
    }

    return path_to_content;
}

bool IsInRoot(const fs::path& root, const fs::path& resource){
    for(auto a = root.begin(), b = resource.begin(); a != root.end(); a++, b++){
        if(b == resource.end() || *a != *b){
            return false;
        }
    }
    return true;
}

http::response<http::string_body> RequestHandler::MakeResponse(const http::request<http::string_body> &req
                                                               , const std::vector<std::string>& parseURL){  
    if(parseURL[0] == "api"){
        if(parseURL.size() > 2 && parseURL[1] == "v1"){
            if(parseURL[2] == "maps" && parseURL.size() == 3){
                return ReturnMapsArray(game_, req);
            }
            if(parseURL[2] == "maps" && parseURL.size() == 4){
                const auto this_map = game_.FindMap(util::Tagged<std::string, model::Map>(parseURL[3]));
                if(this_map != nullptr){
                    return ReturnMap(this_map, req);
                }else{
                    return NotFound(game_, req);
                }
            }
        }
    }

    return BadRequest(game_, req);
}

std::optional<http::response<http::file_body>> RequestHandler::MakeFileResponse(const http::request<http::string_body> &req
                                                 , const std::vector<std::string>& URL_path){
    std::filesystem::path path_to_content = MakePath(URL_path, path_to_content_);
    std::string string_for_open = path_to_content;
    const char* file_path = string_for_open.data();

    http::response<http::file_body> response{http::status::ok, req.version()};

    std::string ext = path_to_content.extension();
    response.set(http::field::content_type, MimeType(ext));

    http::file_body::value_type file;
    if(sys::error_code error; file.open(file_path, beast::file_mode::read, error), error){
       return std::nullopt;
    }
    
    response.body() = std::move(file);
    response.prepare_payload();

    return response;
}

http::response<http::string_body> NotFound(model::Game& game_, const http::request<http::string_body>& req){
    http::response<http::string_body> response{http::status::not_found, req.version()};
    response.set(http::field::content_type, ContentType::TYPE_JSON);
    response.body() = "{\"code\": \"mapNotFound\",\"message\": \"Resource not found\"}";
    return response;
}

http::response<http::string_body> FileNotFound(model::Game& game_, const http::request<http::string_body>& req){
    http::response<http::string_body> response{http::status::not_found, req.version()};
    response.set(http::field::content_type, "text/plain");
    response.body() = "404 Not found";
    return response;
}

http::response<http::string_body> BadRequest(model::Game& game_, const http::request<http::string_body>& req){
    http::response<http::string_body> response{http::status::bad_request, req.version()};
    response.set(http::field::content_type, ContentType::TYPE_JSON);
    response.body() = "{\"code\": \"badRequest\",\"message\": \"Bad request\"}";
    return response;
}

http::response<http::string_body> ReturnMapsArray(model::Game& game_, const http::request<http::string_body>& req){

    http::response<http::string_body> response{http::status::ok, req.version()};

    response.set(http::field::content_type, ContentType::TYPE_JSON);

    json::array maps;
    for(const auto& it : game_.GetMaps()){
        json::object map_it;
        map_it["id"] = *it.GetId();
        map_it["name"] = it.GetName();
        maps.push_back(map_it);
    };
    response.body() = json::serialize(maps);
    return response;
}

http::response<http::string_body> ReturnMap(const model::Map* const this_map, const http::request<http::string_body>& req){
    json::object map;
        map["id"] = *this_map->GetId();
        map["name"] = this_map->GetName();

    map["roads"] = ReturnRoads(this_map);
    map["buildings"] = ReturnBuildings(this_map);   
    map["offices"] = ReturnOffices(this_map);

    http::response<http::string_body> response{http::status::ok, req.version()};
    response.set(http::field::content_type, ContentType::TYPE_JSON);
    response.body() = json::serialize(map);
    return response;
}

json::array ReturnRoads(const model::Map* const this_map){
    json::array roads;
    for(const auto &it : this_map-> GetRoads()){
        json::object road;
        model::Point start_point = it.GetStart();
        road["x0"] = start_point.x;
        road["y0"] = start_point.y; 
       
        model::Point end_point = it.GetEnd();
        if(it.IsHorizontal()){
            road["x1"] = end_point.x;
        }else{
            road["y1"] = end_point.y;
        }
        roads.push_back(road);
    }
    return roads;
}

json::array ReturnBuildings(const model::Map* const this_map){
    json::array buildings;
    for(const auto &it : this_map-> GetBuildings()){
        json::object building;
        model::Rectangle bounds= it.GetBounds();
        building["x"] = bounds.position.x;
        building["x"] = bounds.position.y;
        building["w"] = bounds.size.width;
        building["h"] = bounds.size.height;
        buildings.push_back(building);
    }
    return buildings;
}

json::array ReturnOffices(const model::Map* const this_map){
    json::array offices;
    for(const auto &it : this_map-> GetOffices()){
        json::object office;
        office["id"] = *it.GetId();

        model::Point position = it.GetPosition();
        office["x"] = position.x;
        office["y"] = position.y;

        model::Offset offset = it.GetOffset();
        office["offsetX"] = offset.dx;
        office["offsetY"] = offset.dy;
        offices.push_back(office);
    }
    return offices;
}

std::string MimeType(std::string& expand){
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

void DownScale(std::string& string){
    for(int i = 0; i < string.size()-1; i++){
        if(string[i]>'A' && string[i]<'Z') {
            string[i]+='z'-'Z';
        }
    }
}

}  // namespace http_handler


   