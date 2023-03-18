#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "session/dog.h"

SCENARIO("Dog class") {
    
    GIVEN("a dog") {
        size_t id {0};
        game_session::Coordinate start {0.0, 0.0};
        double default_speed {5.0};
        game_session::Dog dog("name", id, start, default_speed);

        WHEN("dog spawns") {
            THEN("initial speed is 0.0") {
                CHECK(((dog.GetSpeed().dx == 0.0) && 
                       (dog.GetSpeed().dy == 0.0)));
            }

            THEN("initial direction is UP") {
                CHECK(dog.GetDirection() == "U");
            } 
        }

        WHEN("dog's speed changes") {
            THEN("the dog's direction and speed set up corretly") {
                dog.SetSpeed("RIGHT");
                CHECK(dog.GetDirection() == "R");
                CHECK(((dog.GetSpeed().dx == 5.0) && 
                       (dog.GetSpeed().dy == 0.0)));

                dog.SetSpeed("LEFT");
                CHECK(dog.GetDirection() == "L");
                CHECK(((dog.GetSpeed().dx == -5.0) && 
                       (dog.GetSpeed().dy == 0.0)));

                dog.SetSpeed("UP");
                CHECK(dog.GetDirection() == "U");
                CHECK(((dog.GetSpeed().dx == 0.0) && 
                       (dog.GetSpeed().dy == -5.0)));

                dog.SetSpeed("DOWN");
                CHECK(dog.GetDirection() == "D");
                CHECK(((dog.GetSpeed().dx == 0.0) && 
                       (dog.GetSpeed().dy == 5.0)));
            }
            
            AND_WHEN("dog's target direction is empty") {
                THEN("speed becomes 0.0") {
                    dog.SetSpeed("");
                    CHECK(((dog.GetSpeed().dx == 0.0) && 
                           (dog.GetSpeed().dy == 0.0)));
                }
            }
        }

        WHEN("dog has to make offset") {
            THEN("it's position changes correctly") {
                dog.SetCoordX(0.25);
                dog.SetCoordY(0.4);
                CHECK(((dog.GetStartPoint().x == 0.25) && 
                       (dog.GetStartPoint().y == 0.4)));
            }
        }

        WHEN("dog iterats with an item") {
            THEN("it adds properly") {
                using Tag = util::Tagged<std::string, model::Map>;
                for (size_t type = 0; type < 3; ++type) {
                    model::Item item{0.0, 0.0, type, 1};
                    dog.CollectItem(item);
                }
                CHECK(dog.GetItemCount() == 3);

                GIVEN("a map with item scores") {
                    model::Map map{Tag("map"), "map1", 0,0,0,0};
                    map.AddItemCost(25);
                    map.AddItemCost(50);
                    map.AddItemCost(75);

                    THEN("bag clears properly") {
                        dog.ClearBag(map);
                        CHECK(dog.GetItemCount() == 0);

                        // Dog collects points accordingly to item types
                        AND_THEN("dog collects points") {
                            CHECK(dog.GetDogScore() == 150);
                        }
                    }
                }               
            }
        }
    }
}