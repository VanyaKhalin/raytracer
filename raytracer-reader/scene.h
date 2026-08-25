#pragma once

#include "material.h"
#include "vector.h"
#include "object.h"
#include "light.h"
#include "face_parser.h"

#include <vector>
#include <unordered_map>
#include <string>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iterator>
#include <utility>

class Scene {
public:
    void SetObject(const Object& obj) {
        object_.push_back(obj);
    }
    void SetSphereObject(const SphereObject& obj) {
        sphere_object_.push_back(obj);
    }
    void SetLights(const Light& light) {
        light_.push_back(light);
    }
    void SetMaterials(const std::unordered_map<std::string, Material>& mapa) {
        material_ = mapa;
    }
    const std::vector<Object>& GetObjects() const {
        return object_;
    }
    const std::vector<SphereObject>& GetSphereObjects() const {
        return sphere_object_;
    }
    const std::vector<Light>& GetLights() const {
        return light_;
    }
    const std::unordered_map<std::string, Material>& GetMaterials() const {
        return material_;
    }

private:
    std::vector<Object> object_;
    std::vector<SphereObject> sphere_object_;
    std::vector<Light> light_;
    std::unordered_map<std::string, Material> material_;
};

std::unordered_map<std::string, Material> ReadMaterials(const std::filesystem::path& path) {
    std::unordered_map<std::string, Material> materials;
    std::ifstream input(path);

    std::string str;
    std::string local_mat;

    while (input >> str) {

        if (!str.empty() && str[0] == '#') {
            std::getline(input, str);
            continue;
        }

        if (str == "newmtl") {
            input >> local_mat;

            Material material{
                local_mat, Vector(0, 0, 0), Vector(0, 0, 0), Vector(0, 0, 0), Vector(0, 0, 0), 1,
                1,         Vector(1, 0, 0)};
            materials.emplace(local_mat, material);
        } else if (str == "Ka") {
            double r, g, b;
            input >> r >> g >> b;
            materials[local_mat].ambient_color = Vector(r, g, b);
        } else if (str == "Kd") {
            double r, g, b;
            input >> r >> g >> b;
            materials[local_mat].diffuse_color = Vector(r, g, b);
        } else if (str == "Ks") {
            double r, g, b;
            input >> r >> g >> b;
            materials[local_mat].specular_color = Vector(r, g, b);
        } else if (str == "Ke") {
            double r, g, b;
            input >> r >> g >> b;
            materials[local_mat].intensity = Vector(r, g, b);
        } else if (str == "Ns") {
            double x;
            input >> x;
            materials[local_mat].specular_exponent = x;
        } else if (str == "Ni") {
            double x;
            input >> x;
            materials[local_mat].refraction_index = x;
        } else if (str == "al") {
            double r, g, b;
            input >> r >> g >> b;
            materials[local_mat].albedo = Vector(r, g, b);
        }
    }

    return materials;
}
Scene ReadScene(const std::filesystem::path& path) {
    Scene scene;
    std::vector<Vector> norm;
    std::vector<Vector> point;

    std::string str;
    std::string local_material;
    std::ifstream input(path);

    while (input >> str) {
        if (str == "mtllib") {
            std::string filename;
            input >> filename;
            auto mtl_path = path.parent_path() / filename;
            auto materials = ReadMaterials(mtl_path);

            scene.SetMaterials(materials);
        }
        if (str == "usemtl") {
            input >> local_material;
        }
        if (str == "v") {
            double x, y, z;
            input >> x >> y >> z;
            point.push_back(Vector(x, y, z));
        }
        if ((str == "vn")) {
            double x, y, z;
            input >> x >> y >> z;
            norm.push_back(Vector(x, y, z));
        }
        if ((str == "S")) {
            double x, y, z, r;
            input >> x >> y >> z >> r;
            Sphere sp(Vector(x, y, z), r);
            const Material* mat = &scene.GetMaterials().at(local_material);

            scene.SetSphereObject(SphereObject(mat, sp));
        }
        if (str == "P") {
            double x, y, z, r, g, b;
            input >> x >> y >> z >> r >> g >> b;
            scene.SetLights(Light(Vector(x, y, z), Vector(r, g, b)));
        }

        if (str == "f") {
            std::string facet;
            std::getline(input, facet);

            std::vector<FacePt> fs = ParseFace(facet, point.size(), norm.size());

            const Material* mat = &scene.GetMaterials().at(local_material);

            for (size_t i = 1; i + 1 < fs.size(); i++) {
                const Vector* n0 = nullptr;
                const Vector* n1 = nullptr;
                const Vector* n2 = nullptr;
                if (fs[0].normal_index >= 0) {
                    n0 = &norm[fs[0].normal_index];
                }
                if (fs[i].normal_index >= 0) {
                    n1 = &norm[fs[i].normal_index];
                }
                if (fs[i + 1].normal_index >= 0) {
                    n2 = &norm[fs[i + 1].normal_index];
                }
                Triangle tr(point[fs[0].vertex_index], point[fs[i].vertex_index],
                            point[fs[i + 1].vertex_index]);
                scene.SetObject(Object(mat, tr, n0, n1, n2));
            }
        }
    }

    return scene;
}
