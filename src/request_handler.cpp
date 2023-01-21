 #include "request_handler.h"

namespace http_handler {

http::response<http::string_body> RequestHandler::MakeResponse(const http::request<http::string_body>& req){
    auto const not_found = [&req](){
        http::response<http::string_body> response{http::status::not_found, req.version()};
        response.set(http::field::content_type, ContentType::TYPE_JSON);
        response.body() = "{\"code\": \"mapNotFound\",\"message\": \"Map not found\"}";
        return response;
    }; 
    
    auto const bad_request = [&req](){
        http::response<http::string_body> response{http::status::bad_request, req.version()};
        response.set(http::field::content_type, ContentType::TYPE_JSON);
        response.body() = "{\"code\": \"badRequest\",\"message\": \"Bad request\"}";
        return response;
    }; 

    //if request if different from /api/v1/maps -> bad request
    std::string_view http_request = req.target();
    if((http_request.find("/api/v1/maps")) == std::string::npos){
        return bad_request();
    }

    //If request is /api/vi/maps -> return info about maps
    http_request.remove_prefix(7);
    if(http_request == "/maps"){
        return ReturnMapsArray(game_, req);
    }
    
    //If id is actual -> return info about ID. Otherwise -> NotFound.
    http_request.remove_prefix(6);
    const auto this_map = game_.FindMap(util::Tagged<std::string, model::Map>(static_cast<std::string>(http_request)));
    if(this_map != nullptr){
        return ReturnMap(this_map, req);
    }
    return not_found();
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
    std::cout<<json::serialize(map)<<std::endl;
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

}  // namespace http_handler


   