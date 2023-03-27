#pragma once 

#include <boost/serialization/unordered_map.hpp>

#include "model/game.h"
#include "session/game_session.h"

namespace model {
    template <typename Archive>
    void serialize(Archive& ar, model::Item& item, [[maybe_unused]] const unsigned int) {
        ar& item.x;
        ar& item.y;
        ar& item.type;
        ar& item.ID;
    }
}  //namecpsece model

namespace serialization {
    
class LostThingsRepr {
public:
    explicit LostThingsRepr(const model::LostThings source);

    LostThingsRepr() = default;
    LostThingsRepr& operator=(const LostThingsRepr&) = default;

    template<typename Archive>
    void serialize(Archive& ar, [[maybe_unused]] const unsigned int) {
        ar& lost_things_;
        ar& generator_interval_;
        ar& generator_probability_;
        ar& items_types_count_;
        ar& id_;
    }

    model::LostThings Recover();

private:
    model::LostThings::Things lost_things_;
    size_t generator_interval_;
    double generator_probability_;
    size_t items_types_count_;
    size_t id_;
};
}  // namespace serialization