#pragma once

#include <iostream>
#include <filesystem>
#include <fstream>

#include <boost/json.hpp>

#include "json/json_loot_types_storage.h"
#include "model/game.h"

namespace json_loader { 

namespace json = boost::json;

void ParseFile(const std::filesystem::path& json_path, boost::json::value& file_json);
void CreateGame(boost::json::value& file_json, model::Game& game);
void ReadRoadsIntoMap(const json::value& maps, model::Map& map);
void ReadBuildingsIntoMap(const json::value& maps, model::Map& map_for_add);
void ReadOfficesIntoMap(const json::value& maps, model::Map& map_for_add);
void ReadItemsCostIntoMap(const json::value& maps, model::Map& map_for_add);
void ReadItemsCount(const json::value& maps, model::Map& map_for_add); 
std::string ReadMapID(const json::value& map);
std::string ReadMapName(const json::value& map);
double ReadLootGeneratorProbability(const json::value& file_json);
size_t ReadLootGeneratorPeriod(const json::value& file_json);
void LoadGame(const std::filesystem::path& json_path, model::Game& game, 
              json_loot::LootTypes& loot_storage);
void GenerateLootTypes(boost::json::value& file_json, json_loot::LootTypes& loot_storage);
double GetDogSpeed(boost::json::value& file_json, const json::value& maps);
size_t GetDogBagSize(boost::json::value& file_json, const json::value& maps);
}  // namespace json_loader
