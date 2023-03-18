#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "session/player_token.h"
#include "catch2_custom_types.h"

SCENARIO("PlayerToken class") {

    WHEN("token generates") {
        std::string token = game_session::GenerateToken();

        THEN("it's lengh is 32") {
            CHECK(token.size() == 32);
        }

        THEN("each symbol is hexademical") {
            for (size_t i = 0; i < 32; ++i) {
                INFO("symbol is " << token[i]);
                REQUIRE(((token[i] >= 'a') && (token[i] <= 'f') || 
                         (token[i] >= '0') && (token[i] <= '9')));
            }
        }
    }

    GIVEN("a dog") {
        game_session::Dog dog("name", 0, {0.0, 0.0}, 0.0);

        WHEN("player adds") {
            THEN("it sucessfuly emplases and we can find player via token") {
                game_session::PlayerTokens players;
                std::string token = players.AddPlayer(dog).GetToken();
                CHECK(players.FindPlayer(token) != std::nullopt);

                AND_THEN("we can't find player via wrong token") {
                    std::string wrong_token = "aaaaaaaaaaaaaaaa";
                    CHECK(players.FindPlayer(wrong_token) == std::nullopt);
                }
            }
        }   
    }
}