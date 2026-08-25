#pragma once

#include <cstddef>
#include <sstream>
#include <string>
#include <vector>

struct FacePt {
    int normal_index = -1;
    int vertex_index = -1;
};

inline int ConvertIndex(int index, size_t size) {
    if (index > 0) {
        return index - 1;
    }
    return static_cast<int>(size) + index;
}

FacePt ParseFaceToken(const std::string& token, size_t points_count, size_t normals_count) {
    FacePt pnt;

    auto ch = token.begin();

    std::string number;
    while (ch != token.end() && *ch != '/') {
        number += *ch;
        ++ch;
    }

    pnt.vertex_index = ConvertIndex(std::stoi(number), points_count);

    if (ch != token.end()) {
        ++ch;

        while (ch != token.end() && *ch != '/') {
            ++ch;
        }

        if (ch != token.end()) {
            ++ch;

            std::string second_number;
            while (ch != token.end()) {
                second_number += *ch;
                ++ch;
            }

            if (!second_number.empty()) {
                pnt.normal_index = ConvertIndex(std::stoi(second_number), normals_count);
            }
        }
    }

    return pnt;
}

std::vector<FacePt> ParseFace(const std::string& facet, size_t points_count, size_t normals_count) {
    std::stringstream ss(facet);
    std::vector<FacePt> result;

    std::string token;
    while (ss >> token) {
        result.push_back(ParseFaceToken(token, points_count, normals_count));
    }

    return result;
}
