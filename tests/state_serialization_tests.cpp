#include <sstream>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "serialization/item_serialization.h"
#include "serialization/dog_serialization.h"
#include "serialization/session_serialization.h"

using namespace boost::archive;
using namespace serialization;
using namespace game_session;
using namespace model;

SCENARIO("Serialization") {
    std::stringstream ss;
    text_oarchive oa{ss};

    GIVEN("an item") {
        Item item{0.25, 0.5, 5, 10};

        WHEN("an item is serialized") {
            oa << item;
    
            THEN("an item after serialization is iqual to the item before serialization") {
                Item r_item;
                text_iarchive ia{ss};
                ia >> r_item;
                CHECK(item == r_item);
            }
        }
    }

    GIVEN("a Coordinate") {
        Coordinate coord{0.0, 0.75};

        WHEN("a Coordinate is serialized") {
            oa << coord;
    
            THEN("a Coordinate after serialization is iqual to the Coordinate before serialization") {
                Coordinate r_coord;
                text_iarchive ia{ss};
                ia >> r_coord;
                CHECK(coord == r_coord);
            }
        }
    }

    GIVEN("a Speed") {
        Speed speed{0.0, 0.75};

        WHEN("a Speed is serialized") {
            oa << speed;
    
            THEN("a Speed after serialization is iqual to the Speed before serialization") {
                Speed r_speed;
                text_iarchive ia{ss};
                ia >> r_speed;
                CHECK(speed == r_speed);
            }
        }
    }

    GIVEN("LostThings object") {
        LostThings init{5, 0.5};
        init.AddLostThing(0, {0.0, 0.0, 0, 0});

        WHEN("LostThings are serialized") {
            LostThingsRepr repr_init{init};
            oa << repr_init;
    
            THEN("LostThings before and after seialization are equal") {
                LostThings result{0, 0.0};
                LostThingsRepr recover{};
                text_iarchive ia{ss};
                ia >> recover;
                result = recover.Recover();
                CHECK(init == result);
            }
        }
    }

    GIVEN("Dog object") {
        Dog init_dog("name", 0, {0.0, 0.0}, 5.0);

        WHEN("dog is serialized") {
            DogRepr ser_dog{init_dog};
            oa << ser_dog;
    
            THEN("Dogs before and after seialization are equal") {
                DogRepr recover{};
                text_iarchive ia{ss};
                ia >> recover;
                Dog result{recover.Recover()};
                CHECK(init_dog == result);
            }
        }
    }

    auto map = []{
        using MapTag = util::Tagged<std::string, Map>;

        size_t speed {5}, period {1}, probability {1}, bag_size {5};
        Map map(MapTag("id"), "name", speed, bag_size, period, probability);
        map.AddRoad(Road{Road::HORIZONTAL, Point{10,10}, 30});
        map.AddItem(0, {3.3, 5.5, 0, 0});
        return map;
    }();
    
    auto session = [&map]{
        game_session::GameSession session{map};
        session.AddDog("name", true);
        return session;
    }();

    GIVEN("Game session object") {
        GameSession init_session = session;

        WHEN("game session is serialized") {
            GameSessionRepr repr_session{init_session};
            oa << repr_session;
    
            THEN("Dogs before and after seialization are equal") {
                GameSessionRepr recover{};
                text_iarchive ia{ss};
                ia >> recover;
                game_session::GameSession res_session{map};
                recover.Recover(res_session);
                auto it = res_session;
                CHECK(init_session.GetDogs() == res_session.GetDogs());
                CHECK(init_session.GetMap().GetLostThings() == res_session.GetMap().GetLostThings());
                CHECK(init_session.GetPlayerTokens().GetPlayers().begin() -> first
                        == res_session.GetPlayerTokens().GetPlayers().begin() -> first);
            }
        }
    }

    auto session_repr = [&session]{
        GameSessionRepr represent_session{session};
        return represent_session;
    }();

    GIVEN("Several session objects") {
        GameSessionStorage init_sessions{};
        for (int i = 0; i < 3; ++i) {
            init_sessions.AddSession(session_repr);
        }

        WHEN("game session is serialized") {
            oa << init_sessions;
    
            THEN("Dogs before and after seialization are equal") {
                GameSessionStorage recover_sessions;
                text_iarchive ia{ss};
                ia >> recover_sessions;

                GameSession result_session{map};
                recover_sessions.GetSessions().at(1).Recover(result_session);
                for (auto& it : recover_sessions.GetSessions()) {
                    GameSession result_session{map};
                    it.Recover(result_session);
                    CHECK(session.GetDogs() == result_session.GetDogs());
                    CHECK(session.GetMap().GetLostThings() == result_session.GetMap().GetLostThings());
                    CHECK(session.GetPlayerTokens().GetPlayers().begin() -> first
                            == result_session.GetPlayerTokens().GetPlayers().begin() -> first);
                }
            }
        }
    }
}

