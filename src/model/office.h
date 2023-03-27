#pragma once 

#include "model/building.h"
#include "util/tagged.h"

namespace model {

struct Offset {
    Dimension dx, dy;
};

class Office {
public:
    using Id = util::Tagged<std::string, Office>;

    Office(Id id, Point position, Offset offset) noexcept
        : id_{std::move(id)}
        , position_{position}
        , offset_{offset} {
    }

    Office(const Office&) = default;
    Office& operator=(const Office&) = default;

    const Id& GetId() const noexcept {
        return id_;
    }

    Point GetPosition() const noexcept {
        return position_;
    }

    Offset GetOffset() const noexcept {
        return offset_;
    }

private:
    Id id_;
    Point position_;
    Offset offset_;
};
}  // namespace model