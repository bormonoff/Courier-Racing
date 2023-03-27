#pragma once

#include <map>
#include <optional>
#include <string>
#include <sstream>

#include "session/player.h"
#include "util/utils.h"

namespace game_session{

std::string GenerateToken();

class PlayerTokens{
public:
    PlayerTokens() = default;

    PlayerTokens(const PlayerTokens&) = default;
    PlayerTokens& operator=(const PlayerTokens&) = default;

    const std::map<std::string, Player>& GetPlayers() const {
        return players_;
    }

    const Player& AddPlayer(Dog& dog);
    void AddPlayer(Player& player);
    const std::optional<Player> FindPlayer(std::string token);

private:
    std::map<std::string, Player> players_;
};
} // namespace

