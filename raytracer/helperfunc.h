#pragma once

#include "../raytracer-geom/vector.h"

#include <cmath>

Vector Add(const Vector& a, const Vector& b) {
    return Vector(a[0] + b[0], a[1] + b[1], a[2] + b[2]);
}
Vector Sub(const Vector& a, const Vector& b) {
    return Vector(a[0] - b[0], a[1] - b[1], a[2] - b[2]);
}
Vector Mul(const Vector& a, const Vector& b) {
    return Vector(a[0] * b[0], a[1] * b[1], a[2] * b[2]);
}
Vector MulNum(const Vector& a, double k) {
    return Vector(a[0] * k, a[1] * k, a[2] * k);
}
Vector Negative(const Vector& a) {
    return Vector(-a[0], -a[1], -a[2]);
}

double Clamp(double k) {
    if (k < 0) {
        return 0;
    }
    if (k > 1) {
        return 1;
    }
    return k;
}

double Tone(double k, double max_colour) {
    if (max_colour == 0) {
        return 0;
    } else {
        return k * (1 + k / (max_colour * max_colour)) / (1 + k);
    }
}

int ToneToRGB(double k, double max_colour) {
    double val = Tone(k, max_colour);
    val = Clamp(val);
    val = std::pow(val, 1.0 / 2.2);

    return static_cast<int>(255 * val);
}
