#pragma once

#include "vector.h"

#include <cstddef>

class Triangle {
public:
    Triangle(const Vector& a, const Vector& b, const Vector& c) : a_(a), b_(b), c_(c) {
    }

    const Vector& operator[](size_t ind) const {
        if (ind == 0) {
            return a_;
        }
        if (ind == 1) {
            return b_;
        }

        return c_;
    }
    double Area() const {
        Vector ab(b_[0] - a_[0], b_[1] - a_[1], b_[2] - a_[2]);
        Vector ac(c_[0] - a_[0], c_[1] - a_[1], c_[2] - a_[2]);

        return Length(CrossProduct(ab, ac)) / 2;
    }

private:
    Vector a_;
    Vector b_;
    Vector c_;
};
