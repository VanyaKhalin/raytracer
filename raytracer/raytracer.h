#pragma once

#include "../raytracer-reader/scene.h"
#include "../raytracer-geom/ray.h"
#include "options/camera_options.h"
#include "options/render_options.h"
#include "image.h"
#include "createray.h"
#include "searchminintersection.h"
#include "helperfunc.h"
#include "create_colour.h"

#include <filesystem>
#include <optional>
#include <vector>
#include <algorithm>

Image Render(const std::filesystem::path& path, const CameraOptions& camera_options,
             const RenderOptions& render_options) {
    Scene scene = ReadScene(path);

    Image image(camera_options.screen_width, camera_options.screen_height);

    if (render_options.mode == RenderMode::kFull) {
        std::vector<Vector> colours(camera_options.screen_height * camera_options.screen_width);
        double max_value = 0;
        for (int y = 0; y < camera_options.screen_height; y++) {
            for (int x = 0; x < camera_options.screen_width; x++) {
                Ray ray = CreateRay(x, y, camera_options);
                Vector colour = CreateColour(ray, scene, render_options.depth);

                colours[y * camera_options.screen_width + x] = colour;

                max_value = std::max(colour[0], max_value);
                max_value = std::max(colour[1], max_value);
                max_value = std::max(colour[2], max_value);
            }
        }

        for (int y = 0; y < camera_options.screen_height; y++) {
            for (int x = 0; x < camera_options.screen_width; x++) {
                Vector colour = colours[y * camera_options.screen_width + x];

                image.SetPixel(RGB{ToneToRGB(colour[0], max_value), ToneToRGB(colour[1], max_value),
                                   ToneToRGB(colour[2], max_value)},
                               y, x);
            }
        }
        return image;
    }

    std::vector<std::optional<Hit>> hits(camera_options.screen_width *
                                         camera_options.screen_height);

    for (int y = 0; y < camera_options.screen_height; y++) {
        for (int x = 0; x < camera_options.screen_width; x++) {
            Ray ray = CreateRay(x, y, camera_options);
            hits[y * camera_options.screen_width + x] = SearchMinIntersection(ray, scene);
        }
    }
    if (render_options.mode == RenderMode::kDepth) {
        double max_dist = 0;
        for (int i = 0; i < camera_options.screen_height; i++) {
            for (int j = 0; j < camera_options.screen_width; j++) {
                auto& hit = hits[i * camera_options.screen_width + j];

                if (hit.has_value()) {
                    double local_dist = hit->intersection.GetDistance();

                    if (local_dist > max_dist) {
                        max_dist = local_dist;
                    }
                }
            }
        }

        if (max_dist == 0) {
            for (int i = 0; i < camera_options.screen_height; i++) {
                for (int j = 0; j < camera_options.screen_width; j++) {
                    image.SetPixel(RGB{255, 255, 255}, i, j);
                }
            }
            return image;
        }

        for (int i = 0; i < camera_options.screen_height; i++) {
            for (int j = 0; j < camera_options.screen_width; j++) {
                auto& hit = hits[i * camera_options.screen_width + j];
                if (!hit.has_value()) {
                    image.SetPixel(RGB{255, 255, 255}, i, j);
                } else {
                    double colour = hit->intersection.GetDistance() / max_dist;
                    int num = static_cast<int>(255 * colour);

                    image.SetPixel(RGB{num, num, num}, i, j);
                }
            }
        }

        return image;
    }

    if (render_options.mode == RenderMode::kNormal) {
        for (int i = 0; i < camera_options.screen_height; i++) {
            for (int j = 0; j < camera_options.screen_width; j++) {
                auto& hit = hits[i * camera_options.screen_width + j];
                if (!hit.has_value()) {
                    image.SetPixel(RGB{0, 0, 0}, i, j);
                } else {
                    Vector normal = hit->normal;

                    int r, g, b;
                    r = static_cast<int>(255 * (normal[0] * 0.5 + 0.5));
                    g = static_cast<int>(255 * (normal[1] * 0.5 + 0.5));
                    b = static_cast<int>(255 * (normal[2] * 0.5 + 0.5));

                    image.SetPixel(RGB{r, g, b}, i, j);
                }
            }
        }
    }
    return image;
}
