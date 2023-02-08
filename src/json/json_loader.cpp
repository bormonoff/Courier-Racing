#include "json_loader.h"

namespace json_loader {


model::Game LoadGame(const std::filesystem::path& json_path) {
    boost::json::value file_json{};
    ParseFile(json_path, file_json);

    model::Game game;
    CreateGame(file_json, game);

    return game;
}

void ParseFile(const std::filesystem::path& json_path, boost::json::value& file_json){
    std::ifstream ios{json_path};

    if(!ios){
        throw std::invalid_argument("Can't find JSON file");
    }

    std::string file_data((std::istreambuf_iterator<char>(ios)),
                 std::istreambuf_iterator<char>());

    boost::system::error_code ec;
    file_json = json::parse(file_data, ec);
    
    if(ec){
        throw std::invalid_argument("Can't parse JSON file. Wrong format");
    }
}

void CreateGame(boost::json::value& file_json, model::Game& game){
    for(const auto& maps : file_json.at("maps").as_array()){
        
        std::string ID = static_cast<std::string>(maps.at("id").as_string());
        std::string name = static_cast<std::string>(maps.at("name").as_string());
        double speed = GetDogSpeed(file_json, maps);

        model::Map map_for_add(util::Tagged<std::string, model::Map>(ID), name, speed);

        ReadRoadsIntoMap(maps, map_for_add);    
        ReadOfficesIntoMap(maps, map_for_add);
        ReadBuildingsIntoMap(maps, map_for_add);
        game.AddMap(map_for_add);
    }
}

double GetDogSpeed(boost::json::value& file_json, const json::value& maps){
    double speed = file_json.at("defaultDogSpeed").as_double();
    try{
        speed = maps.at("dogSpeed").as_double();
    }catch(...){}
    return speed;
}

void ReadRoadsIntoMap(const json::value& maps, model::Map& map_for_add){
    for(const auto& roads : maps.at("roads").as_array()){
        int x = std::stoi(json::serialize(roads.at("x0")));
        int y = std::stoi(json::serialize(roads.at("y0")));
            
        if(roads.as_object().if_contains("x1")){
            int offset = std::stoi(json::serialize(roads.at("x1")));
            map_for_add.AddRoad(model::Road(model::Road::HORIZONTAL,model::Point{x,y}, offset));
        }else{
            int offset = std::stoi(json::serialize(roads.at("y1")));
            map_for_add.AddRoad(model::Road(model::Road::VERTICAL,model::Point{x,y}, offset));
        }  
    }
}

void ReadBuildingsIntoMap(const json::value& maps, model::Map& map_for_add){
    for(const auto& buildings : maps.at("buildings").as_array()){
        int x = std::stoi(json::serialize(buildings.at("x")));
        int y = std::stoi(json::serialize(buildings.at("y")));
        int width = std::stoi(json::serialize(buildings.at("w")));
        int height = std::stoi(json::serialize(buildings.at("h")));
        map_for_add.AddBuilding(model::Building{model::Rectangle{model::Point{x,y},model::Size{width,height}}});
    }
}

void ReadOfficesIntoMap(const json::value& maps, model::Map& map_for_add){
for(const auto& offices : maps.at("offices").as_array()){
        std::string officeID = static_cast<std::string>(offices.at("id").as_string());
        int x = std::stoi(json::serialize(offices.at("x")));
        int y = std::stoi(json::serialize(offices.at("y")));
        int offsetX = std::stoi(json::serialize(offices.at("offsetX")));
        int offsetY = std::stoi(json::serialize(offices.at("offsetY")));
        map_for_add.AddOffice(model::Office{util::Tagged<std::string,model::Office>(officeID), model::Point{x,y}, model::Offset{offsetX, offsetY}});
    }
}

}  // namespace json_loader
