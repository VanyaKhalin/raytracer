#pragma once

#include "options/camera_options.h"
#include "options/render_options.h"
#include "image.h"
#include "../raytracer-geom/ray.h"
#include "../raytracer-geom/geometry.h"

#include <cmath>

Ray CreateRay(int x, int y, const CameraOptions& camera_options) {
    Vector orig = camera_options.look_from;

    double sheight_real = 2 * std::tan(camera_options.fov / 2);
    double swidth_real = sheight_real * static_cast<double>(camera_options.screen_width) /
                         camera_options.screen_height;

    double pixel_width = swidth_real / camera_options.screen_width;
    double pixel_height = sheight_real / camera_options.screen_height;

    double local_x = -1 * (swidth_real / 2 - pixel_width * (x + 0.5));
    double local_y = sheight_real / 2 - pixel_height * (y + 0.5);

    Vector ahead(camera_options.look_to[0] - camera_options.look_from[0],
                 camera_options.look_to[1] - camera_options.look_from[1],
                 camera_options.look_to[2] - camera_options.look_from[2]);

    ahead.Normalize();

    Vector const_up(0, 1, 0);
    Vector right = CrossProduct(ahead, const_up);

    if (Length(right) < 1e-10) {
        if (ahead[1] > 0) {
            const_up = Vector(0, 0, 1);
        } else {
            const_up = Vector(0, 0, -1);
        }
        right = CrossProduct(ahead, const_up);
    }

    right.Normalize();

    Vector up = CrossProduct(right, ahead);
    up.Normalize();

    Vector dir(ahead[0] + right[0] * local_x + up[0] * local_y,
               ahead[1] + right[1] * local_x + up[1] * local_y,
               ahead[2] + right[2] * local_x + up[2] * local_y);

    dir.Normalize();

    return Ray(orig, dir);
}
