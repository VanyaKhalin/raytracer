#pragma once

#include "../raytracer-geom/geometry.h"
#include "../raytracer-reader/scene.h"
#include "hit.h"

#include <limits>
#include <optional>

constexpr double kMinraydist = 1e-6;

Vector GetNormal(const Object& obj, const Intersection& intersection, const Ray& ray) {
    Vector normal = intersection.GetNormal();

    const Vector* n0 = obj.GetNormal(0);
    const Vector* n1 = obj.GetNormal(1);
    const Vector* n2 = obj.GetNormal(2);

    if (n0 != nullptr && n1 != nullptr && n2 != nullptr) {
        Vector coords = GetBarycentricCoords(obj.polygon, intersection.GetPosition());

        normal = Vector((*n0)[0] * coords[0] + (*n1)[0] * coords[1] + (*n2)[0] * coords[2],
                        (*n0)[1] * coords[0] + (*n1)[1] * coords[1] + (*n2)[1] * coords[2],
                        (*n0)[2] * coords[0] + (*n1)[2] * coords[1] + (*n2)[2] * coords[2]);

        normal.Normalize();

        if (DotProduct(normal, ray.GetDirection()) > 0) {
            normal = Vector(-normal[0], -normal[1], -normal[2]);
        }
    }

    return normal;
}

std::optional<Hit> SearchMinIntersection(const Ray& ray, const Scene& scene) {
    std::optional<Hit> min_hit;
    double min_dist = std::numeric_limits<double>::max();

    for (const auto& obj : scene.GetObjects()) {
        auto local_intersection = GetIntersection(ray, obj.polygon);

        if (local_intersection.has_value() && local_intersection->GetDistance() > kMinraydist &&
            local_intersection->GetDistance() < min_dist) {
            min_dist = local_intersection->GetDistance();

            Vector normal = GetNormal(obj, *local_intersection, ray);
            min_hit = Hit{*local_intersection, normal, obj.material};
        }
    }

    for (const auto& obj : scene.GetSphereObjects()) {
        auto local_intersection = GetIntersection(ray, obj.sphere);

        if (local_intersection.has_value() && local_intersection->GetDistance() > kMinraydist &&
            local_intersection->GetDistance() < min_dist) {
            min_dist = local_intersection->GetDistance();

            Vector normal = local_intersection->GetNormal();
            min_hit = Hit{*local_intersection, normal, obj.material};
        }
    }

    return min_hit;
}
