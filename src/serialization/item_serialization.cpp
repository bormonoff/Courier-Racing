#include "serialization/item_serialization.h"

namespace serialization {

LostThingsRepr::LostThingsRepr(const model::LostThings source)
    : lost_things_{source.GetLostThings()},
        generator_interval_{source.GetGeneratorInterval()},
        generator_probability_{source.GetGeneratorProbability()}, 
        items_types_count_{source.GetItemTypesCount()},
        id_{source.GetItemIdCount()} {}


model::LostThings LostThingsRepr::Recover() {
    model::LostThings result{generator_interval_,
                                generator_probability_};

    for (auto& [key, value] : lost_things_) {
        result.AddLostThing(key, value);
    }
    result.SetItemTypesCount(items_types_count_);
    result.SetId(id_);
    return result;
}
}  // namespace serialization