#pragma once

#include "../raytracer-geom/geometry.h"
#include "../raytracer-reader/scene.h"
#include "createray.h"
#include "hit.h"
#include "searchminintersection.h"
#include "helperfunc.h"

#include <cmath>
#include <algorithm>

constexpr double kEps = 1e-6;

Vector Illuminance(const Ray& ray, const Scene& scene, const Vector& point, const Vector& normal,
                   const Material* material) {
    Vector diff_sum(0, 0, 0);
    Vector spect_sum(0, 0, 0);

    for (auto& light : scene.GetLights()) {
        Vector light_dist = Sub(light.position, point);
        double dist_to_light = Length(light_dist);
        light_dist.Normalize();

        Ray shadow_dist(Add(point, MulNum(light_dist, kEps)), light_dist);
        auto sh_hit = SearchMinIntersection(shadow_dist, scene);

        if (sh_hit.has_value() && sh_hit->intersection.GetDistance() + kEps < dist_to_light) {
            continue;
        }
        double local_diff_koef = DotProduct(light_dist, normal);

        local_diff_koef = std::max(0.0, local_diff_koef);

        diff_sum = Add(diff_sum, MulNum(light.intensity, local_diff_koef));

        Vector v_e = Negative(ray.GetDirection());
        Vector v_lr = Reflect(Negative(light_dist), normal);

        double spect_koef = DotProduct(v_e, v_lr);
        spect_koef = std::max(spect_koef, 0.0);

        spect_koef = std::pow(spect_koef, material->specular_exponent);
        spect_sum = Add(spect_sum, MulNum(light.intensity, spect_koef));
    }

    Vector first_part = Mul(diff_sum, material->diffuse_color);
    Vector second_part = Mul(spect_sum, material->specular_color);
    Vector light_part = MulNum(Add(first_part, second_part), material->albedo[0]);
    light_part = Add(light_part, material->ambient_color);
    light_part = Add(light_part, material->intensity);

    return light_part;
}

Vector CreateColour(const Ray& ray, const Scene& scene, int depth, bool inside = false) {
    auto hit = SearchMinIntersection(ray, scene);

    if (!hit.has_value()) {
        return Vector(0, 0, 0);
    }

    const Material* material = hit->material;

    if (material == nullptr) {
        return Vector(0, 0, 0);
    }

    Vector point = hit->intersection.GetPosition();
    Vector normal = hit->normal;

    Vector answer = Illuminance(ray, scene, point, normal, material);

    if (depth == 0) {
        return answer;
    }

    if (!inside && material->albedo[1] != 0) {
        Vector refl_dir = Reflect(ray.GetDirection(), normal);
        Ray refl_ray(Add(point, MulNum(normal, kEps)), refl_dir);
        Vector refl_colour = CreateColour(refl_ray, scene, depth - 1);
        answer = Add(answer, MulNum(refl_colour, material->albedo[1]));
    }
    if (material->albedo[2] != 0) {
        double transp;
        double eta;

        if (inside) {
            eta = material->refraction_index;
            transp = 1.0;
        } else {
            eta = 1.0 / material->refraction_index;
            transp = material->albedo[2];
        }

        auto refr_dir = Refract(ray.GetDirection(), normal, eta);

        if (refr_dir.has_value()) {
            Ray refr_ray(Add(point, MulNum(*refr_dir, kEps)), *refr_dir);
            Vector refr_colour = CreateColour(refr_ray, scene, depth - 1, !inside);

            answer = Add(answer, MulNum(refr_colour, transp));
        }
    }
    return answer;
}
