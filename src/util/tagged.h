#pragma once
#include <compare>

namespace util {

template <typename Value, typename Tag>
class Tagged {
public:
    using ValueType = Value;
    using TagType = Tag;

    explicit Tagged(Value&& v)
        : value_(std::move(v)) {
    }
    explicit Tagged(const Value& v)
        : value_(v) {
    }

    const Value& operator*() const {
        return value_;
    }

    Value& operator*() {
        return value_;
    }

    auto operator<=>(const Tagged<Value, Tag>&) const = default;

private:
    Value value_;
};

template <typename TaggedValue>
struct TaggedHasher {
    size_t operator()(const TaggedValue& value) const {
        return std::hash<typename TaggedValue::ValueType>{}(*value);
    }
};

}  // namespace util
