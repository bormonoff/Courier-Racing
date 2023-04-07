#pragma once 

#include <boost/serialization/vector.hpp>

#include "session/game_session.h"
#include "serialization/item_serialization.h"
#include "dog_serialization.h"

namespace game_session {
    template<typename Archive>
    void serialize(Archive& ar, Coordinate& coord, [[maybe_unused]] const unsigned int) {
        ar& coord.x;
        ar& coord.y;
    }

    template<typename Archive>
    void serialize(Archive& ar, Speed& speed, [[maybe_unused]] const unsigned int) {
        ar& speed.dx;
        ar& speed.dy;
    }
}  // namespace game_session

namespace serialization {
    
class DogRepr {
public:
    explicit DogRepr(const game_session::Dog source, size_t Id);

    DogRepr() = default;
    DogRepr& operator=(const DogRepr&) = default;

    template<typename Archive>
    void serialize(Archive& ar, [[maybe_unused]] const unsigned int) {
        ar& name_;
        ar& id_;
        ar& default_speed_;
        ar& coordinate_;
        ar& speed_;
        ar& direction_;
        ar& items_in_bag_;
        ar& scored_points_;
        ar& retriement_time_;
    }

    game_session::Dog Recover();

private:
    std::string name_;
    size_t id_;
    size_t retriement_time_;
    double default_speed_;
    
    game_session::Coordinate coordinate_;
    game_session::Speed speed_;
    size_t direction_;
    game_session::Dog::Items items_in_bag_;
    size_t scored_points_;
};
}  // namespace serialization