#pragma once

#include <stdlib.h>
#include <Logic.hpp>

struct Vec2 {
    int32_t x, y;

    constexpr bool operator==(const Vec2& o) const noexcept { return x == o.x && y == o.y; }
};

struct Vec3 {
    int32_t x, y, z;

    constexpr bool operator==(const Vec3& o) const noexcept { return x == o.x && y == o.y && z == o.z; }
}