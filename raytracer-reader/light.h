#pragma once

#include "vector.h"

struct Light {
    Light() = default;
    Light(const Vector& pos, const Vector& inten) : position(pos), intensity(inten) {
    }

    Vector position;
    Vector intensity;
};
