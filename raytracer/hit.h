#pragma once

#include "../raytracer-geom/intersection.h"
#include "../raytracer-reader/material.h"

struct Hit {
    Intersection intersection;
    Vector normal;
    const Material* material = nullptr;
};
