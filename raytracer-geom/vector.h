#pragma once

#include <array>
#include <cstddef>
#include <cmath>

class Vector;
double Length(const Vector& v);

class Vector {
public:
    Vector() : data_{0, 0, 0} {
    }
    Vector(double x, double y, double z) : data_{x, y, z} {
    }

    double& operator[](size_t ind) {
        return data_[ind];
    }
    double operator[](size_t ind) const {
        return data_[ind];
    }

    void Normalize() {
        double len = Length(*this);
        if (len == 0) {
            return;
        }

        data_[0] /= len;
        data_[1] /= len;
        data_[2] /= len;
    }

private:
    std::array<double, 3> data_;
};

double DotProduct(const Vector& a, const Vector& b) {
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}
Vector CrossProduct(const Vector& a, const Vector& b) {
    return Vector(a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0]);
}
double Length(const Vector& v) {
    return std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}
