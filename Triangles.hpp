#ifndef TRIANGLES_H
#define TRIANGLES_H

#include <vector>
#include <array>

#include "LinearAlgebra.hpp"


class Triangle {
public:
    Triangle(v3<float> a, v3<float> b, v3<float> c)
	:a(a), b(b), c(c) {}
    v3<float> a;
    v3<float> b;
    v3<float> c;
};

class Triangles {
public:

    int size() {
        return triangles.size();
    }

    void add_triangle(v3<float> a, v3<float> b, v3<float> c) {
        bool aFound = false;
        bool bFound = false;
        bool cFound = false;
        int outA;
        int outB;
        int outC;

        int i = 0;
        while ((!aFound || !bFound || !cFound) && i < (int) vertices.size()) {
            if (!aFound && vertices[i] == a) {
                outA = i;
            }
            if (!bFound && vertices[i] == b) {
                outB = i;
            }
            if (!cFound && vertices[i] == c) {
                outC = i;
            }
        }
        if (aFound && bFound && cFound) {
            std::array<int, 3> output{outA, outB, outC};
            triangles.push_back(output);
        } else {
            int counter = 0;
            if (!aFound) {
                outA = vertices.size() + counter;
                vertices.push_back(a);
                counter++;
            }
            if (!bFound) {
                outB = triangles.size() + counter;
                vertices.push_back(b);
                counter++;
            }
            if (!cFound) {
                outC = triangles.size() + counter;
                vertices.push_back(c);
                counter++;
            }
            std::array<int, 3> output{outA, outB, outC};
            triangles.push_back(output);
        }
    }

    Triangle at(int i) {
        if (i >= (int) triangles.size() || i < 0) {
            class TrianglesIndexPastEndError{};
            throw TrianglesIndexPastEndError{};
        } else {
            Triangle output(vertices[triangles[i][0]], vertices[triangles[i][1]], vertices[triangles[i][2]]);

            return output;
        }
    }

private:
    std::vector<v3<float>> vertices;

    // triagles use the index of each vertex in the vertices vector. 
    std::vector<std::array<int, 3>> triangles;
};

#endif
