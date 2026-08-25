#pragma once

#include "triangle.h"
#include "material.h"
#include "sphere.h"
#include "vector.h"

#include <array>

struct Object {
    const Material* material = nullptr;
    Triangle polygon;
    std::array<Vector, 3> normals;
    std::array<bool, 3> has_normal = {false, false, false};

    Object(const Material* mat, const Triangle& tr) : material(mat), polygon(tr) {
    }

    Object(const Material* mat, const Triangle& tr, const Vector* n0, const Vector* n1,
           const Vector* n2)
        : material(mat), polygon(tr) {
        if (n0 != nullptr) {
            has_normal[0] = true;
            normals[0] = *n0;
        }
        if (n1 != nullptr) {
            has_normal[1] = true;
            normals[1] = *n1;
        }
        if (n2 != nullptr) {
            has_normal[2] = true;
            normals[2] = *n2;
        }
    }

    const Vector* GetNormal(size_t index) const {
        if (has_normal[index]) {
            return &normals[index];
        }
        return nullptr;
    }
};

struct SphereObject {
    SphereObject(const Material* mat, const Sphere& sp) : material(mat), sphere(sp) {
    }
    const Material* material = nullptr;
    Sphere sphere;
};
