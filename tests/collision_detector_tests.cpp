#include <math.h>
#include <unordered_map>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "model/collision_detector.h"

SCENARIO("FindGatherEvents function") {
    using namespace collision_detector;
    
    GIVEN("items and gothers") {
        ItemGathererProvider temp;
        temp.AddItem({3.0, 3.0}, 1.0, 1);
        temp.AddItem({4.0, 5.0}, 1.0, 1);
        temp.AddGatherer({0.0, 0.0}, {6.0, 0.0}, 1.0, 1);
        temp.AddGatherer({0.0, 0.0}, {5.0, 0.0}, 3.0, 2);
        temp.AddGatherer({0.0, 0.0}, {0.0, 5.0}, 3.0, 3);
        temp.AddGatherer({10.0, 0.0}, {10.0, 5.0}, 2.0, 4);
        
        WHEN("this function calls") {
            auto result = FindGatherEvents(temp);

            THEN("it detects all events") {
                CHECK(result.size() == 3);
            }

            THEN("it detects events is chronological order") {
                for (int i = 1; i < result.size() - 1; ++i) {
                    REQUIRE(result[i-1].time <= result[i].time);
                }
            }

            THEN("Events have right data") {
                AND_THEN("time in range between 0 and 1") {
                    for (auto it : result) {
                        REQUIRE(((it.time >= 0.0) && (it.time <= 1.0)));
                    }
                }

                AND_THEN("distance is correct") {
                    using Catch::Matchers::WithinAbs;
                    
                    ItemGathererProvider distance;
                    distance.AddItem({1.5, 1.5}, 1.0, 1);
                    distance.AddGatherer({0.0, 0.0}, {5.0, 0.0}, 1.0, 1);
                    CHECK_THAT(std::sqrt(FindGatherEvents(distance)[0].sq_distance), WithinAbs(1.5, 1e-10));
                }
            }
        }
    }
    
}