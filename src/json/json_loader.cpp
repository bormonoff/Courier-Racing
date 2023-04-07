#include "json_loader.h"

#include "json/json_loader.h"
#include "util/literals_storage.h"

namespace json_loader {

using namespace http_handler;

void LoadGame(const std::filesystem::path& json_path, model::Game& game, 
              json_loot::LootTypes& loot_storage) {
    boost::json::value file_json{};
    ParseFile(json_path, file_json);
    CreateGame(file_json, game);
    GenerateLootTypes(file_json, loot_storage);
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
        std::string mapID = ReadMapID(maps);
        std::string name = ReadMapName(maps);
        double speed = GetDogSpeed(file_json, maps);
        size_t bag_capacity = GetDogBagSize(file_json, maps);
        size_t period = ReadLootGeneratorPeriod(file_json);
        size_t lifetime = ReadDogLifetime(file_json);
        double probability = ReadLootGeneratorProbability(file_json);

        model::Map map_for_add(util::Tagged<std::string, model::Map>(mapID), name, 
                               speed, period, bag_capacity, probability, lifetime);
        ReadRoadsIntoMap(maps, map_for_add);    
        ReadOfficesIntoMap(maps, map_for_add);
        ReadBuildingsIntoMap(maps, map_for_add);
        ReadItemsCostIntoMap(maps, map_for_add);
        ReadItemsCount(maps, map_for_add);
        game.AddMap(map_for_add);
    }
}

std::string ReadMapID(const json::value& map) {
    std::string MapID;
    try {
        MapID = static_cast<std::string>(map.at(ID).as_string());;
    } catch(std::exception& ex) {
        throw std::runtime_error("Can't read mapID from config");
    }
    return MapID;
}

std::string ReadMapName(const json::value& map) {
    std::string MapName;
    try {
        MapName = static_cast<std::string>(map.at(NAME).as_string());
    } catch(std::exception& ex) {
        throw std::runtime_error("Can't read map name from config");
    }
    return MapName;
}

void ReadItemsCostIntoMap(const json::value& maps, model::Map& map_for_add) {
    try {
        for (const auto& roads : maps.at(LOOTTYPES).as_array()) {
            int item_cost = std::stoi(json::serialize(roads.at(VALUE)));
            map_for_add.AddItemCost(item_cost);
        }
    } catch(std::exception& ex) {
        throw std::runtime_error("Can't read items cost (values) from config");
    }
}

size_t ReadLootGeneratorPeriod(const json::value& file_json) {
    size_t period;
    try {
        period = std::stoi(json::serialize(file_json.at(LOOTCONFIG).at(PERIOD)));
    } catch(std::exception& ex) {
        throw std::runtime_error("Can't read spawn items period from config");
    }
    return period;
}

size_t ReadDogLifetime(const json::value& file_json) {
    double lifetime;
    try {
        lifetime = std::stod(json::serialize(file_json.at("dogRetirementTime")));
    } catch(std::exception& ex) {
        throw std::runtime_error("Can't read dog retriement time from config");
    }
    // Conversion from seconds to milliseconds
    return lifetime *= 1000;
}

double ReadLootGeneratorProbability(const json::value& file_json) {
    double probability;
    try {
        probability = std::stod(json::serialize(file_json.at(LOOTCONFIG).at(PROBABILITY)));
    } catch(std::exception& ex) {
        throw std::runtime_error("Can't read spawn items probability from config");
    }
    return probability;
}

void GenerateLootTypes(boost::json::value& file_json, json_loot::LootTypes& loot_storage) {
    try {
        for (const auto& maps : file_json.at(MAPS).as_array()) {
            std::string mapID = static_cast<std::string>(maps.at(ID).as_string());
            loot_storage.loot_types_[mapID] = maps.at(LOOTTYPES);
        }
    } catch(std::exception& ex) {
        throw std::runtime_error("Can't read loot arrays from config");
    }
}

double GetDogSpeed(boost::json::value& file_json, const json::value& maps) {
    double speed;
    try {
        speed = file_json.at(DEFAULTDOGSPEED).as_double();
    } catch(std::exception& ex) {
        throw std::runtime_error("Can't read default dog speed from config");
    }

    try {
        speed = maps.at(DOGSPEED).as_double();
    } catch(std::exception& ex) {}
    return speed;
}

size_t GetDogBagSize(boost::json::value& file_json, const json::value& maps) {
    size_t bag_capacity = 3;
    try {
        bag_capacity = file_json.at(DEFAULTBAGCAPACITY).as_int64();
    } catch(std::exception& ex) {}
    try {
        bag_capacity = maps.at(BAGCAPACITY).as_int64();
    } catch(std::exception& ex) {}
    return bag_capacity;
}

void ReadItemsCount(const json::value& maps, model::Map& map_for_add) {
    try {
        map_for_add.SetTypesCount(std::distance(maps.at(LOOTTYPES).as_array().begin(), 
                                                maps.at(LOOTTYPES).as_array().end()));  
    } catch(std::exception& ex) {
        throw std::runtime_error("Can't parce lootType arrays from config");
    }
}

void ReadRoadsIntoMap(const json::value& maps, model::Map& map_for_add) {
    try {
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
    } catch(std::exception& ex) {
        throw std::runtime_error("Can't parce Road arrays from config");
    }
}

void ReadBuildingsIntoMap(const json::value& maps, model::Map& map_for_add) {
    try {
        for (const auto& buildings : maps.at(BUILDINGS).as_array()) {
            int x = std::stoi(json::serialize(buildings.at(X)));
            int y = std::stoi(json::serialize(buildings.at(Y)));
            int width = std::stoi(json::serialize(buildings.at(WIDHT)));
            int height = std::stoi(json::serialize(buildings.at(HEIGHT)));
            map_for_add.AddBuilding(
                model::Building{model::Rectangle{model::Point{x,y},
                                model::Size{width,height}}});
        }
    } catch(std::exception& ex) {
        throw std::runtime_error("Can't parce Building arrays from config");
    }
}

void ReadOfficesIntoMap(const json::value& maps, model::Map& map_for_add) {
    try {
        for (const auto& offices : maps.at(OFFICES).as_array()) {
            std::string officeID = static_cast<std::string>(offices.at(ID).as_string());
            int x = std::stoi(json::serialize(offices.at(X)));
            int y = std::stoi(json::serialize(offices.at(Y)));
            int offsetX = std::stoi(json::serialize(offices.at(OFFSETX)));
            int offsetY = std::stoi(json::serialize(offices.at(OFFSETY)));
            map_for_add.AddOffice(
                model::Office{util::Tagged<std::string,model::Office>(officeID), 
                            model::Point{x,y}, model::Offset{offsetX, offsetY}});
        }
    } catch(std::exception& ex) {
        throw std::runtime_error("Can't parce Office arrays from config");
    }
}
}  // namespace json_loader
