#include "model/road.h"

namespace model {

struct Size {
    Dimension width, height;
};

struct Rectangle {
    Point position;
    Size size;
};

class Building {
public:
    explicit Building(Rectangle bounds) noexcept
        : bounds_{bounds} {
    }

    Building(const Building&) = default;
    Building& operator=(const Building&) = default;

    const Rectangle& GetBounds() const noexcept {
        return bounds_;
    }

private:
    Rectangle bounds_;
};

}  // namespace model