#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "catch2_custom_types.h"
#include "session/game_session.h"

SCENARIO("GameSession class") {
    using namespace model;

    GIVEN("a map with road") {
        using MapTag = util::Tagged<std::string, Map>;

        std::string ID {"map"};
        std::string name {"Map1"};
        size_t speed {5}, period {1}, probability {1}, bag_size {5};
        Map map(MapTag("id"), "name", speed, bag_size, period, probability);
        map.AddRoad(Road{Road::HORIZONTAL, Point{10,10}, 30});

        game_session::GameSession game {map};

        WHEN("dog adds to the session") {
            THEN("it spawns inside road") {
                bool random = 1;

                AND_WHEN("randomly") {
                    for (int i = 0; i < 10; ++i) {
                        game_session::Player player= game.AddDog("name", random);
                        REQUIRE(player.GetDogStart().x >= (10.0 - 0.4));
                        REQUIRE(player.GetDogStart().x <= (30.0 + 0.4)); 
                        REQUIRE(player.GetDogStart().y >= (10.0 - 0.4));
                        REQUIRE(player.GetDogStart().y <= (10.0 + 0.4));
                    }
                }  

                AND_WHEN("not randomly") {
                    random = 0;
                    for (int i = 0; i < 10; ++i) {
                        game_session::Player player= game.AddDog("name", random);
                        REQUIRE(player.GetDogStart().x == 10.0);
                        REQUIRE(player.GetDogStart().y == 10.0);
                    }
                }         
            }

            THEN("player addes sucessfuly") {
                game_session::Player player = game.AddDog("name", random);
                CHECK(game.GetPlayerTokens().GetPlayers().size() == 1);

                game_session::Player player2 = game.AddDog("name", random);
                CHECK(game.GetPlayerTokens().GetPlayers().size() == 2);

                AND_THEN("we can find these players via tokens") {
                    CHECK(game.FindPlayer(player.GetToken()) != std::nullopt);
                    CHECK(game.FindPlayer(player2.GetToken()) != std::nullopt);
                }

                AND_THEN("we can't find players with wrong tokens") {
                    CHECK(game.FindPlayer("aaaaaaaaaaaaaaaa") == std::nullopt);
                    CHECK(game.FindPlayer("aaaaaaaaaaaaaaaa") == std::nullopt);
                }
            }
        }

        WHEN("dog changes it's coordination during time") { 
            using DetectData = collision_detector::ItemGathererProvider;

            game_session::Player player = game.AddDog("name", false);
            player.SetDogSpeed("RIGHT");
            DetectData temp;

            AND_WHEN("target coordinate inside road") {
                THEN("dog moves normally") {
                    size_t time {1000};
                    game.MakeOffset(time, temp);
                    CHECK(player.GetDogStart().x == 15.0);
                    game.MakeOffset(time, temp);
                    CHECK(player.GetDogStart().x == 20.0);
                }   

                THEN("otherwise dog stop") {
                    size_t time {1000};
                    player.SetDogSpeed("UP");
                    game.MakeOffset(time, temp);
                    CHECK(player.GetDogStart().y == 9.6);
                    game.MakeOffset(time, temp);
                    CHECK(player.GetDogStart().y == 9.6);

                    player.SetDogSpeed("DOWN");
                    game.MakeOffset(time, temp);
                    CHECK(player.GetDogStart().y == 10.4);
                    game.MakeOffset(time, temp);
                    CHECK(player.GetDogStart().y == 10.4);

                    player.SetDogSpeed("LEFT");
                    game.MakeOffset(time, temp);
                    CHECK(player.GetDogStart().x == 9.6);
                    game.MakeOffset(time, temp);
                    CHECK(player.GetDogStart().x == 9.6);

                    player.SetDogSpeed("RIGHT");
                    time = 100000;
                    game.MakeOffset(time, temp);
                    CHECK(player.GetDogStart().x == 30.4);
                    game.MakeOffset(time, temp);
                    CHECK(player.GetDogStart().x == 30.4);
                }   
            }
        }

        GIVEN("a dog") {
            using DetectData = collision_detector::ItemGathererProvider;

            DetectData temp;
            game_session::Player player = game.AddDog("name", false);
            player.SetDogSpeed("RIGHT");
            size_t time = 1000;

            WHEN("dog runs near item") { 
                THEN("dog gets this item") {
                    game.GetMap().AddItem(0, {11.0, 10.29, 0, 0});
                    game.GetMap().AddItem(1, {15.0, 10.12, 0, 0});
                    game.MakeOffset(time, temp);
                    game.AddItemsToCollisionDetector(temp);
                    INFO("Dog runs from {10.0, 10.0} to {15.0, 10.0} and try to collect"
                            << "{11.0, 10.29} and {15.0, 10.12} items");
                    CHECK(collision_detector::FindGatherEvents(temp).size() == 2);
                }
            }

            WHEN("dog runs far from item") { 
                THEN("dog can't take items") {
                    game.GetMap().AddItem(0, {11.0, 10.3, 0, 0});
                    game.GetMap().AddItem(1, {16.0, 10.12, 0, 0});
                    game.MakeOffset(time, temp);
                    game.AddItemsToCollisionDetector(temp);
                    INFO("Dog runs from {10.0, 10.0} to {15.0, 10.0} and try to collect"
                            << "{11.0, 10.3} and {16.0, 10.12} items");
                    CHECK(collision_detector::FindGatherEvents(temp).size() == 0);
                }
            }

            GIVEN("an office") {
                using TagOffice = util::Tagged<std::string,model::Office>;

                map.AddOffice(Office{TagOffice("ID"), {15, 10}, {0,0}});
                game.AddOfficesToCollisionDetector(temp);

                WHEN("dog runs near office") { 
                    THEN("the collision detects") {
                        game.MakeOffset(time, temp);
                        INFO("Dog runs from {10.0, 10.0} to {15.0, 10.0} and try to drop"
                                << "items");
                        CHECK(collision_detector::FindGatherEvents(temp).size() == 1);
                    }
                }                    
            }            
        }
    }
}