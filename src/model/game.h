#pragma once

#include "model/map.h"

namespace model {

class Game {
public:
    using Maps = std::vector<Map>;
    
    Game() = default;

    Game(const Game&) = default;
    Game& operator=(const Game&) = default;

    const Maps& GetMaps() const noexcept {
        return maps_;
    }

    void AddMap(Map map);
    const Map* FindMap(const Map::Id& id) const noexcept;

private:
    using MapIdHasher = util::TaggedHasher<Map::Id>;
    using MapIdToIndex = std::unordered_map<Map::Id, size_t, MapIdHasher>;

    std::vector<Map> maps_;
    MapIdToIndex map_id_to_index_;
};
}  // namespace model