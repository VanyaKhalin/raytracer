#pragma once

#include "vector.h"
#include "sphere.h"
#include "intersection.h"
#include "triangle.h"
#include "ray.h"

#include <cmath>
#include <algorithm>
#include <optional>

std::optional<Intersection> GetIntersection(const Ray& ray, const Sphere& sphere) {
    Vector oc(ray.GetOrigin()[0] - sphere.GetCenter()[0],
              ray.GetOrigin()[1] - sphere.GetCenter()[1],
              ray.GetOrigin()[2] - sphere.GetCenter()[2]);

    double a = DotProduct(ray.GetDirection(), ray.GetDirection());
    double b = 2 * DotProduct(ray.GetDirection(), oc);
    double c = DotProduct(oc, oc) - sphere.GetRadius() * sphere.GetRadius();

    double d = b * b - 4 * a * c;

    if (d < 0) {
        return std::nullopt;
    }

    double x1 = (-b + std::sqrt(d)) / (2 * a);
    double x2 = (-b - std::sqrt(d)) / (2 * a);

    if (x1 < 0 && x2 < 0) {
        return std::nullopt;
    }

    double distance = std::min(x1, x2);

    if (distance < 0) {
        distance = std::max(x1, x2);
    }

    Vector position(ray.GetOrigin()[0] + ray.GetDirection()[0] * distance,
                    ray.GetOrigin()[1] + ray.GetDirection()[1] * distance,
                    ray.GetOrigin()[2] + ray.GetDirection()[2] * distance);

    Vector normal(position[0] - sphere.GetCenter()[0], position[1] - sphere.GetCenter()[1],
                  position[2] - sphere.GetCenter()[2]);
    normal.Normalize();

    if (DotProduct(normal, ray.GetDirection()) > 0) {
        normal = Vector(-normal[0], -normal[1], -normal[2]);
    }

    return Intersection(position, normal, distance);
}
std::optional<Intersection> GetIntersection(const Ray& ray, const Triangle& triangle) {
    const Vector a = triangle[0];
    const Vector b = triangle[1];
    const Vector c = triangle[2];

    Vector ab(b[0] - a[0], b[1] - a[1], b[2] - a[2]);
    Vector ac(c[0] - a[0], c[1] - a[1], c[2] - a[2]);

    Vector pvec = CrossProduct(ray.GetDirection(), ac);
    double det = DotProduct(ab, pvec);
    if (det < 1e-8 && det > -1e-8) {
        return std::nullopt;
    }

    double inv_det = 1 / det;
    Vector tvec(ray.GetOrigin()[0] - a[0], ray.GetOrigin()[1] - a[1], ray.GetOrigin()[2] - a[2]);
    double u = DotProduct(tvec, pvec) * inv_det;

    if (u < 0 || u > 1) {
        return std::nullopt;
    }

    Vector qvec = CrossProduct(tvec, ab);
    double v = DotProduct(ray.GetDirection(), qvec) * inv_det;
    if (v < 0 || u + v > 1) {
        return std::nullopt;
    }

    double t = DotProduct(ac, qvec) * inv_det;

    if (t < 0) {
        return std::nullopt;
    }
    const Vector& origin = ray.GetOrigin();
    const Vector& directonal = ray.GetDirection();

    Vector position(origin[0] + directonal[0] * t, origin[1] + directonal[1] * t,
                    origin[2] + directonal[2] * t);

    Vector normal = CrossProduct(ab, ac);
    normal.Normalize();
    if (DotProduct(normal, directonal) > 0) {
        normal = Vector(-normal[0], -normal[1], -normal[2]);
    }
    return Intersection(position, normal, t);
}

Vector Reflect(const Vector& ray, const Vector& normal) {
    double x = 2.0 * DotProduct(normal, ray);
    Vector reflect(ray[0] - x * normal[0], ray[1] - x * normal[1], ray[2] - x * normal[2]);

    reflect.Normalize();
    return reflect;
}

std::optional<Vector> Refract(const Vector& ray, const Vector& normal, double eta) {
    Vector r = ray;
    Vector n = normal;
    r.Normalize();
    n.Normalize();
    double dot = DotProduct(n, r);
    double k = 1.0 - eta * eta * (1.0 - dot * dot);
    if (k < 0.0) {
        return std::nullopt;
    }

    Vector result(eta * r[0] - (eta * dot + std::sqrt(k)) * n[0],
                  eta * r[1] - (eta * dot + std::sqrt(k)) * n[1],
                  eta * r[2] - (eta * dot + std::sqrt(k)) * n[2]);

    result.Normalize();

    return result;
}
Vector GetBarycentricCoords(const Triangle& triangle, const Vector& point) {
    const Vector& a = triangle[0];
    const Vector& b = triangle[1];
    const Vector& c = triangle[2];

    Vector ab(b[0] - a[0], b[1] - a[1], b[2] - a[2]);
    Vector ac(c[0] - a[0], c[1] - a[1], c[2] - a[2]);
    Vector ap(point[0] - a[0], point[1] - a[1], point[2] - a[2]);

    // AP = v * AB + w * AC
    double d0 = DotProduct(ab, ab);
    double d1 = DotProduct(ab, ac);
    double d2 = DotProduct(ac, ac);
    double d3 = DotProduct(ap, ab);
    double d4 = DotProduct(ap, ac);

    double den = d0 * d2 - d1 * d1;
    double y = (d2 * d3 - d1 * d4) / den;
    double z = (d0 * d4 - d1 * d3) / den;
    double x = 1 - y - z;

    return Vector(x, y, z);
}
