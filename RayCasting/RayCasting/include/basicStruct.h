#pragma once

struct TriangleIndex {
    TriangleIndex() {
        x[0] = 0; x[1] = 0; x[2] = 0;
    }
    TriangleIndex(int x1, int x2, int x3) {
        x[0] = x1, x[1] = x2, x[2] = x3;
    }
    int& operator[](const int i) { return x[i]; }
    // By Computer Graphics convention, counterclockwise winding is front face
    int x[3];
};

