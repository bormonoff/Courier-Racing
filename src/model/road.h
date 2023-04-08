#pragma once

#include <compare>

namespace model {

using Dimension = int;
using Coord = Dimension;

struct Point {
    Coord x, y;

    constexpr auto operator<=>(const Point&) const = default;
};

class Road {
    struct HorizontalTag {
        HorizontalTag() = default;
    };

    struct VerticalTag {
        VerticalTag() = default;
    };

public:
    constexpr static HorizontalTag HORIZONTAL{};
    constexpr static VerticalTag VERTICAL{};

    Road(HorizontalTag, Point start, Coord end_x) noexcept
        : start_{start}
        , end_{end_x, start.y} {
    }

    Road(VerticalTag, Point start, Coord end_y) noexcept
        : start_{start}
        , end_{start.x, end_y} {
    }

    Road(const Road&) = default;
    Road& operator=(const Road&) = default;
    
    bool IsHorizontal() const noexcept {
        return start_.y == end_.y;
    }

    bool IsVertical() const noexcept {
        return start_.x == end_.x;
    }

    Point GetStart() const noexcept {
        return start_;
    }

    Point GetEnd() const noexcept {
        return end_;
    }

    void SetStart(Point point){ 
        start_ = point;
    }

    void SetEnd(Point point) {
        end_ = point;
    }

private:
    Point start_;
    Point end_;
};
}  // namespace model