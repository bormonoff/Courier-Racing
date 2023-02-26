#include "json_loader.h"
#include "util/literals_storage.h"

namespace json_loader {

using namespace http_handler;

model::Game LoadGame(const std::filesystem::path& json_path) {
    boost::json::value file_json{};
    ParseFile(json_path, file_json);
    model::Game game;
    CreateGame(file_json, game);
    return game;
}

void ParseFile(const std::filesystem::path& json_path, boost::json::value& file_json) {
    std::ifstream ios{json_path};
    if (!ios) {
        throw std::invalid_argument("Can't find JSON file");
    }
    std::string file_data((std::istreambuf_iterator<char>(ios)),
                 std::istreambuf_iterator<char>());
    boost::system::error_code ec;
    file_json = json::parse(file_data, ec);
    if (ec) {
        throw std::invalid_argument("Can't parse JSON file. Wrong format");
    }
}

void CreateGame(boost::json::value& file_json, model::Game& game) {
    for (const auto& maps : file_json.at(MAPS).as_array()) {
        std::string mapID = static_cast<std::string>(maps.at(ID).as_string());
        std::string name = static_cast<std::string>(maps.at(NAME).as_string());
        double speed = GetDogSpeed(file_json, maps);

        model::Map map_for_add(util::Tagged<std::string, model::Map>(mapID), name, speed);
        ReadRoadsIntoMap(maps, map_for_add);    
        ReadOfficesIntoMap(maps, map_for_add);
        ReadBuildingsIntoMap(maps, map_for_add);
        game.AddMap(map_for_add);
    }
}

double GetDogSpeed(boost::json::value& file_json, const json::value& maps) {
    double speed = file_json.at(DEFAULTDOGSPEED).as_double();
    try {
        speed = maps.at(DOGSPEED).as_double();
    } catch(...) {}
    return speed;
}

void ReadRoadsIntoMap(const json::value& maps, model::Map& map_for_add) {
    for (const auto& roads : maps.at(ROADS).as_array()) {
        int x = std::stoi(json::serialize(roads.at(X0)));
        int y = std::stoi(json::serialize(roads.at(Y0)));
        if (roads.as_object().if_contains(X1)) {
            int offset = std::stoi(json::serialize(roads.at(X1)));
            map_for_add.AddRoad(
                model::Road(model::Road::HORIZONTAL,model::Point{x,y}, offset));
        } else {
            int offset = std::stoi(json::serialize(roads.at(Y1)));
            map_for_add.AddRoad(
                model::Road(model::Road::VERTICAL,model::Point{x,y}, offset));
        }  
    }
}

void ReadBuildingsIntoMap(const json::value& maps, model::Map& map_for_add) {
    for (const auto& buildings : maps.at(BUILDINGS).as_array()) {
        int x = std::stoi(json::serialize(buildings.at(X)));
        int y = std::stoi(json::serialize(buildings.at(Y)));
        int width = std::stoi(json::serialize(buildings.at(WIDHT)));
        int height = std::stoi(json::serialize(buildings.at(HEIGHT)));
        map_for_add.AddBuilding(
            model::Building{model::Rectangle{model::Point{x,y},
                            model::Size{width,height}}});
    }
}

void ReadOfficesIntoMap(const json::value& maps, model::Map& map_for_add) {
    for (const auto& offices : maps.at(OFFICES).as_array()) {
        std::string officeID = static_cast<std::string>(offices.at("id").as_string());
        int x = std::stoi(json::serialize(offices.at(X)));
        int y = std::stoi(json::serialize(offices.at(Y)));
        int offsetX = std::stoi(json::serialize(offices.at(OFFSETX)));
        int offsetY = std::stoi(json::serialize(offices.at(OFFSETY)));
        map_for_add.AddOffice(
            model::Office{util::Tagged<std::string,model::Office>(officeID), 
                          model::Point{x,y}, model::Offset{offsetX, offsetY}});
    }
}
}  // namespace json_loader
