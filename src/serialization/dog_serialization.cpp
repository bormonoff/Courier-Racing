#include "serialization/dog_serialization.h"

namespace serialization {

DogRepr::DogRepr(const game_session::Dog source)
    : name_{source.GetName()},
        id_{source.GetID()},
        default_speed_{source.GetDefaultSpeed()}, 
        coordinate_{source.GetStartPoint()},
        speed_{source.GetSpeed()},
        direction_{source.GetDirectionViaInt()},
        items_in_bag_{source.GetItemsInBag()}, 
        scored_points_{source.GetDogScore()} {}


game_session::Dog DogRepr::Recover() {
    game_session::Dog result{name_, id_, coordinate_,
                                default_speed_};
    result.SetSpeed(speed_);
    result.SetDirectionViaInt(direction_);
    for (auto& it : items_in_bag_) {
        result.CollectItem(it);
    }
    result.AddScorePoints(scored_points_);
    return result;
}
}  // namespace serialization