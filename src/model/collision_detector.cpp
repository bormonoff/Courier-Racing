#include "model/collision_detector.h"

#include <cassert>
#include <math.h>

namespace collision_detector {

CollectionResult TryCollectPoint(Point2D a, Point2D b, Point2D c) {
    // Vecor lengh is equal to end point - start point
    const double u_x = c.x - a.x;
    const double u_y = c.y - a.y;
    const double v_x = b.x - a.x;
    const double v_y = b.y - a.y;

    // Dot vectors x & y product is equal to x.x * y.x + x.y + y.y
    const double u_dot_v = u_x * v_x + u_y * v_y;
    const double u_len2 = u_x * u_x + u_y * u_y;
    const double v_len2 = v_x * v_x + v_y * v_y;

    // Absolute lengh of move vector. If the result is lower than 0
    // or higher than 1 - we don't collect an item
    const double proj_ratio = u_dot_v / v_len2;

    // The square distance between v vector and the collect item
    const double sq_distance = u_len2 - (u_dot_v * u_dot_v) / v_len2;

    return CollectionResult(sq_distance, proj_ratio);
}

std::vector<GatheringEvent> FindGatherEvents(const ItemGathererProvider& provider) {
    std::vector<GatheringEvent> result;
    double time {0};
    if (provider.GatherersCount() == 0 || provider.ItemsCount() == 0) { return result; }
    for (size_t gather = 0; gather <= provider.GatherersCount() - 1; ++gather) {
        for (size_t item = 0; item <= provider.ItemsCount() - 1; ++item) {
            auto collection = TryCollectPoint(provider.GetGatherer(gather).start_pos,
                                              provider.GetGatherer(gather).end_pos,
                                              provider.GetItem(item).position);
            if (collection.proj_ratio >= 0.0 && collection.proj_ratio <= 1.0) {
                if (std::sqrt(collection.sq_distance) 
                        <= (provider.GetGatherer(gather).width + 
                            provider.GetItem(item).width)) {
                    time += 0.01;
                    result.emplace_back(provider.GetItem(item).id,
                                        provider.GetGatherer(gather).id, 
                                        collection.sq_distance, time);
                }
            }
        }
    }
    return result;
}
}  // namespace collision_detector