#include <catch2/catch_test_macros.hpp>

#include "model/model.h"

using namespace std::literals;

SCENARIO("Model") {
    using namespace model;

    GIVEN("a map") {
        std::string ID {"map"};
        std::string name {"Map1"};
        size_t speed {5}, period {1}, probability {1}, bag_size {5};
        Map map(util::Tagged<std::string, Map>(ID),
                       name, speed, bag_size, period, probability);
        map.AddRoad(Road{Road::HORIZONTAL, Point{30,0}, 0});

        SECTION("RoadMap class tests") {
            WHEN("emplace road in map") {
                THEN("start point is always smaller than finish") {
                    CHECK(map.GetRoadMap()[0].GetStart().x == 0);
                    CHECK(map.GetRoadMap()[0].GetEnd().x == 30);
                }
            }

            WHEN("add road in map") {
                AND_WHEN("start of the first road is equal to finish of the second and their direction is the same") {
                    THEN("they merge together and the result road have start of the first finish of the second") {
                        map.AddRoad((Road{Road::HORIZONTAL, Point{50,0}, 30}));
                        CHECK(map.GetRoadMap()[0].GetStart().x == 0);
                        CHECK(map.GetRoadMap()[0].GetEnd().x == 50);

                        AND_THEN("only one road is added") {
                            CHECK(map.GetRoadMap().size() == 1);
                        }
                    }
                }
                
                AND_WHEN("start of the first road is equal to finish of the second and their directional is not the same") {
                    THEN("they don't merge") {
                        map.AddRoad((Road{Road::VERTICAL, Point{0,0}, 30}));
                        CHECK(map.GetRoadMap().size() == 2);
                    }
                }
            }
        }

        SECTION("LostThings class tests") { 
            WHEN("add a new thing") {
                THEN("it spawns inside road") {
                    size_t player_count {10}, delta_time {1000};
                    map.GenerateThings(delta_time, player_count);

                    // GetLostThings returns std::unordered_multimap<int, std::pair<double, double>>
                    for(auto it : map.GetLostThings()) {
                        REQUIRE(it.second.x >= (map.GetRoadMap()[0].GetStart().x - 0.4));
                        REQUIRE(it.second.x <= (map.GetRoadMap()[0].GetEnd().x + 0.4));
                        REQUIRE(it.second.y >= (map.GetRoadMap()[0].GetStart().y - 0.4));
                        REQUIRE(it.second.y <= (map.GetRoadMap()[0].GetEnd().y + 0.4));
                    }
                }

                THEN("items count is smaller than players count") {
                    for(unsigned players = 0; players < 10; ++players) {
                        size_t delta_time {1000};
                        map.GenerateThings(delta_time, players);
                        REQUIRE(map.GetLostThings().size() <= players);
                    }
                }
            }
        }
    }
}