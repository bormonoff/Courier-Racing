#pragma once

#include <unordered_map>
#include <string>

#include <boost/json.hpp>

namespace json_loot { 

struct LootTypes {
    std::unordered_map<std::string, boost::json::value> loot_types_;
};
}  // namespace json_loot
