#pragma once

#include <random>
#include <vecmath.h>

inline double rand0_1() {
    return (double)rand() / (double)RAND_MAX;
}

inline double rand0_x(double x){
    return rand0_1() * x;
}

inline double rand0_x_concentrated(double x) {
    double rand_ = rand0_1();
    return rand_ * rand_ * x;
}

